#!/usr/bin/env python3
"""Check native window focus and DALi focus using keyboard input only."""
import argparse
import json
import os
import re
import subprocess
import time
from pathlib import Path

from PIL import ImageGrab

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--display", default=":95")
parser.add_argument("--xdotool", default="xdotool")
args = parser.parse_args()
root = Path(__file__).resolve().parents[2]
output = root / "artifacts/preview"
output.mkdir(parents=True, exist_ok=True)
trace = output / "keyboard-focus.log"
env = dict(os.environ, DISPLAY=args.display, BUXBY_PREVIEW_TRACE="1",
           BUXBY_PREVIEW_LOCALE="en_US.UTF-8", BUXBY_PREVIEW_SECONDS="90")
checks = []


def xdo(*commands):
    return subprocess.check_output([args.xdotool, *commands], env=env, text=True, stderr=subprocess.PIPE, timeout=5).strip()


def expect(window, focus):
    deadline = time.monotonic() + 3
    while time.monotonic() < deadline:
        clean = re.sub(r"\x1b\[[0-9;]*m", "", trace.read_text())
        names = re.findall(r"^FOCUS (.+)$", clean, re.M)
        actual = xdo("getwindowfocus", "getwindowname")
        if actual == window and names and names[-1] == focus:
            checks.append(dict(window=window, focus=focus))
            return
        time.sleep(0.08)
    raise AssertionError(f"Wanted {window}/{focus}; got {actual}/{names[-3:]}")


def key(value, window, focus):
    xdo("key", "--clearmodifiers", value)
    expect(window, focus)


with trace.open("w") as log:
    process = subprocess.Popen([str(root / "tools/run-preview.sh")], env=env,
                               stdout=log, stderr=subprocess.STDOUT)
    try:
        time.sleep(2.3)
        main = "Samsong Buxby"
        expect(main, "en_US.UTF-8")
        key("Right", main, "ko_KR.UTF-8")
        key("Return", main, "ko_KR.UTF-8")
        time.sleep(0.4)
        assert "PREVIEW LOCALE ko_KR.UTF-8" in trace.read_text(), "Enter did not activate language button"
        key("Tab", "Buxby Day", "day")
        time.sleep(2.1)  # Capture after text animation and the native focus frame have rendered.
        ImageGrab.grab(xdisplay=args.display).save(output / "focus-day.png")
        key("Down", "Buxby Activity", "activity")
        key("Right", main, "ko_KR.UTF-8")
        key("Left", main, "en_US.UTF-8")
        key("Left", "Buxby Day", "day")
        key("Tab", "Buxby Activity", "activity")
        key("shift+Tab", "Buxby Day", "day")
        key("shift+Tab", main, "en_US.UTF-8")
        key("Right", main, "ko_KR.UTF-8")
        key("Right", main, "ar_AE.UTF-8")
        key("Return", main, "ar_AE.UTF-8")
        time.sleep(0.5)
        assert "PREVIEW LOCALE ar_AE.UTF-8" in trace.read_text()
        # RTL places Arabic on the left. Physical arrows must follow the mirrored row.
        key("Right", main, "ko_KR.UTF-8")
        key("Right", main, "en_US.UTF-8")
        key("Left", main, "ko_KR.UTF-8")
        key("Left", main, "ar_AE.UTF-8")
        key("Left", "Buxby Day", "day")
        key("Down", "Buxby Activity", "activity")
        key("Up", "Buxby Day", "day")
        key("Escape", main, "ar_AE.UTF-8")
        key("Up", main, "suggestion-1")
        key("Return", main, "suggestion-1")
        key("Up", main, "suggestion-0")
        key("Up", "Buxby Day", "day")
        key("Right", main, "suggestion-0")
        # Repeat activation to check stale targets and duplicate signal connections.
        for _ in range(3):
            key("Tab", "Buxby Day", "day")
            key("Tab", "Buxby Activity", "activity")
            key("Tab", main, "suggestion-0")
        time.sleep(0.3)
        ImageGrab.grab(xdisplay=args.display).save(output / "focus-rtl.png")
        xdo("key", "Escape")
        process.wait(timeout=5)
        assert process.returncode == 0
        assert re.findall(r"button: 528 x (\d+)", trace.read_text().split("PREVIEW LOCALE ")[-1]) == ["82", "150"]
    finally:
        if process.poll() is None:
            process.terminate()
            process.wait(timeout=5)

report = dict(passed=True, assertions=len(checks), checks=checks,
              limitation="Actual X11 activation and DALi focus; Team Resume/OnResume delivery still needs TV validation.")
(output / "focus-checks.json").write_text(json.dumps(report, indent=2) + "\n")
print(f"PASS: {len(checks)} native/window focus checks; keyboard Enter, RTL, repeat navigation and Escape")
