#!/usr/bin/env python3
"""Exercise real DALi/X11 windows; appfw locale delivery is not simulated here."""
import argparse
import gettext
import json
import os
import re
import subprocess
import time
from pathlib import Path

from PIL import ImageGrab

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--display", default=os.getenv("DISPLAY", ":95"))
parser.add_argument("--xdotool", default="xdotool")
args = parser.parse_args()
project = Path(__file__).resolve().parents[2]
output = project / "artifacts/preview"
output.mkdir(parents=True, exist_ok=True)
env = dict(os.environ, DISPLAY=args.display, BUXBY_PREVIEW_TRACE="1",
           BUXBY_PREVIEW_LOCALE="en_US.UTF-8")
windows = (("Samsong Buxby", (1320, 0, 600, 1080)),
           ("Buxby Day", (736, 64, 520, 448)),
           ("Buxby Activity", (736, 568, 520, 448)))


def action(*arguments):
    subprocess.run([args.xdotool, *arguments], env=env, check=True, timeout=5)


def capture(name):
    screen = ImageGrab.grab(xdisplay=args.display)
    assert screen.size == (1920, 1080), "Use a 1920 × 1080 X display for geometry checks"
    screen.save(output / f"{name}.png")
    for x, y in ((1330, 100), (750, 200), (750, 700)):
        assert sum(screen.getpixel((x, y))[:3]) > 60, (name, "Window content disappeared")
    for title, bounds in windows:
        native = subprocess.check_output(
            ["xwininfo", "-display", args.display, "-name", title], text=True, timeout=5)
        (output / f"{name}-{title.replace(' ', '-')}.txt").write_text(native)
        assert "Map State: IsViewable" in native, native
        for key, value in zip(("Absolute upper-left X", "Absolute upper-left Y", "Width", "Height"), bounds):
            assert re.search(rf"{key}:\s+{value}\b", native), native


def run_case(name, fail=False):
    case_env = dict(env, BUXBY_PREVIEW_SECONDS="7" if fail else "20")
    if fail:
        case_env["BUXBY_PREVIEW_SET_FAILURE"] = "1"
    else:
        case_env.pop("BUXBY_PREVIEW_SET_FAILURE", None)
    with (output / f"{name}.log").open("w") as log:
        process = subprocess.Popen([str(project / "tools/run-preview.sh")], env=case_env,
                                   stdout=log, stderr=subprocess.STDOUT)
        try:
            time.sleep(4.0)
            window_id = subprocess.check_output(
                [args.xdotool, "search", "--onlyvisible", "--name", "^Samsong Buxby$"],
                env=env, text=True, timeout=5).strip()
            action("windowactivate", "--sync", window_id)
            if fail:
                action("key", "2")
                time.sleep(0.8)
                capture("failed-set")
            else:
                capture("en_US")
                for key, locale in (("2", "ko_KR"), ("3", "ar_AE")):
                    action("key", key)
                    time.sleep(3.7)
                    capture(locale)
                action("key", "1")
                time.sleep(3.7)
                # Click the second recommendation, then allow LayoutTransition to settle.
                action("mousemove", "1500", "710", "mousedown", "1")
                time.sleep(0.3)
                capture("pressed-card")
                action("mouseup", "1")
                # The screenshot hold exceeds DALi's tap threshold; send a separate short tap.
                action("click", "1")
                time.sleep(0.7)
                capture("expanded-card")
            process.wait(timeout=10)
            assert process.returncode == 0, f"{name}: exit {process.returncode}"
        finally:
            if process.poll() is None:
                process.terminate()
                process.wait(timeout=5)
    return (output / f"{name}.log").read_text()


live = run_case("live-language-switch")
assert not re.search(r"^label: SID_", live, re.M), "Unresolved message ID in a visible label"
headings = (("main", "SID_TITLE"),
            ("day", "SID_TITLE"),
            ("activity", "SID_DESCRIPTION"))
for locale in ("en_US", "ko_KR", "ar_AE"):
    snapshots = re.findall(rf"PREVIEW LOCALE {re.escape(locale)}\.UTF-8\n(.*?)END PREVIEW", live, re.S)
    assert snapshots, f"Missing live locale transition: {locale}"
    for index, (member, message) in enumerate(headings):
        with (project / f"build-host/locale/{locale}/LC_MESSAGES/dali-buxby-{member}.mo").open("rb") as stream:
            translated = gettext.GNUTranslations(stream).gettext(message)
        section = snapshots[-1].split(f"WINDOW {index}\n")[1].split("WINDOW ")[0]
        assert "label: " + translated in section, f"{locale}/{member} did not refresh"
        direction = "rtl" if locale == "ar_AE" else "ltr"
        assert f"window-direction: {direction}" in section, f"Adaptor direction: {locale}/{member}"
final = live.split("PREVIEW LOCALE ")[-1].split("WINDOW 1")[0]
assert re.findall(r"button: 528 x (\d+)", final) == ["82", "150"], "Card transition did not complete"
for tick in ("0", "35", "70"):
    assert re.search(rf"label: {tick}\nlabel-style: SamsungOneUI_400 direction=1 fill=1", live), "Missing chart tick style"

failed = run_case("setter-failure", fail=True)
assert "PREVIEW LOCALE ko_KR" not in failed
assert "label: Couldn't change the language. Please try again." in failed
report = dict(passed=True, cases=["live en→ko→ar→en in all three scenes", "card expansion 150/82→82/150",
                               "generated chart tick font and text fill",
                               "failed setter retains locale and shows error", "native window geometry and visibility", "adaptor locale changes all window root directions"],
              limitation="Real host DALi/X11 and gettext. No Team/appfw callback, TV compositor or Samsung fonts validation.")
(output / "checks.json").write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n")
print(json.dumps(report, ensure_ascii=False, indent=2))
