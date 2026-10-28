#!/usr/bin/env python3
"""Observe real animation progress, lifecycle completion and rapid locale replacement."""
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
project = Path(__file__).resolve().parents[2]
output = project / "artifacts/preview"
output.mkdir(parents=True, exist_ok=True)
trace = output / "motion.log"
env = dict(os.environ, DISPLAY=args.display, BUXBY_PREVIEW_MOTION="1",
           BUXBY_PREVIEW_LOCALE="en_US.UTF-8", BUXBY_PREVIEW_SECONDS="40")
shimmer_samples = {}


def key(value):
    subprocess.run([args.xdotool, "key", "--clearmodifiers", value], env=env, check=True, timeout=5)


def capture(name, seconds=4.0):
    frames = []
    white_pixels = []
    start = time.monotonic()
    while time.monotonic() - start < seconds:
        tick = time.monotonic()
        screen = ImageGrab.grab(xdisplay=args.display)
        frames.append(screen.crop((704, 0, 1920, 1080)).resize((760, 675)))
        headline = screen.crop((1356, 193, 1884, 341)).convert("RGB")
        white_pixels.append(sum(min(rgb) > 240 for rgb in headline.getdata()))
        time.sleep(max(0, 0.1 - (time.monotonic() - tick)))
    frames[0].save(output / f"{name}.gif", save_all=True, append_images=frames[1:], duration=100, loop=0)
    for i in (2, 5, 9, len(frames) - 1):
        frames[i].save(output / f"{name}-{i:02}.png")
    peak = max(range(len(white_pixels)), key=white_pixels.__getitem__)
    frames[peak].save(output / f"{name}-shimmer.png")
    assert white_pixels[peak] > 200, (name, "No visible white shimmer band")
    assert white_pixels[-1] < white_pixels[peak] * 0.25, (name, "Shimmer did not return to the colored fill")
    shimmer_samples[name] = dict(peak_white_pixels=white_pixels[peak], final_white_pixels=white_pixels[-1])
    # A live opacity property alone cannot prove the chart's internal canvas still renders.
    plot = screen.crop((764, 748, 1228, 928)).convert("RGB")
    mint_pixels = sum(g > 120 and b > 100 and r < g * 0.85 for r, g, b in plot.getdata())
    assert mint_pixels > 150, (name, "Chart disappeared during the subtree ENTER transition")
    # END text alignment moves the digits within their bounds when the locale becomes RTL.
    ticks = screen.crop((760, 748, 800, 904)).convert("RGB")
    assert sum(min(rgb) > 130 for rgb in ticks.getdata()) > 50, (name, "Chart ticks are dark or missing")


def read_samples(text):
    text = re.sub(r"\x1b\[[0-9;]*m", "", text)
    return [line.split()[1:] for line in text.splitlines() if line.startswith("MOTION ")]


def verify_reveal(text, locale):
    samples = read_samples(text)
    for window in range(3):
        rows = [row for row in samples if row[0] == locale and int(row[1]) == window]
        assert len(rows) >= 8, (locale, window, "Too few actual animation samples")
        assert all(int(row[3]) == 0 for row in rows), (locale, window, "Missing/incorrect Reveal or SCREEN")
        assert all(int(row[4]) == 1 for row in rows), (locale, window, "Missing headline shimmer")
        assert any(float(row[6]) < 0.8 for row in rows), (locale, window, "Not all labels restarted")
        assert any(0 < float(row[5]) < 0.98 for row in rows), (locale, window, "No intermediate Reveal")
        assert float(rows[-1][5]) == 1, (locale, window, "Reveal did not finish")


with trace.open("w") as log:
    process = subprocess.Popen([str(project / "tools/run-preview.sh")], env=env,
                               stdout=log, stderr=subprocess.STDOUT)
    try:
        capture("motion-launch", 4.0)
        initial = trace.read_text()
        verify_reveal(initial, "en_US.UTF-8")
        starts = set(re.findall(r"TRANSITION start (\d+) 0", initial))
        ends = set(re.findall(r"TRANSITION end (\d+) 0", initial))
        assert len(starts) >= 40 and starts == ends, ("Incomplete initial ENTER", starts - ends)
        for number, locale in (("2", "ko_KR.UTF-8"), ("3", "ar_AE.UTF-8")):
            offset = trace.stat().st_size
            key(number)
            capture("motion-" + locale[:2])
            verify_reveal(trace.read_bytes()[offset:].decode(), locale)
        for number in ("1", "2", "3", "2", "1"):
            key(number)
            time.sleep(0.12)
        time.sleep(2.3)
        last = read_samples(trace.read_text())[-3:]
        assert all(row[0] == "en_US.UTF-8" and float(row[5]) == 1 for row in last), "Rapid replacement left text hidden"
        chart = [float(row[7]) for row in read_samples(trace.read_text()) if row[1] == "2"]
        assert min(chart) <= 0.80 and max(chart) >= 0.98, "Chart did not pulse"
        minima = [i for i in range(1, len(chart)) if chart[i] < 0.80 <= chart[i - 1]]
        assert len(minima) >= 2, "Pulse did not repeat"
        key("2")
        time.sleep(0.15)
        key("Escape")
        process.wait(timeout=5)
        assert process.returncode == 0, "Closing during Reveal failed"
    finally:
        if process.poll() is None:
            process.terminate()
            process.wait(timeout=5)

report = dict(passed=True, initial_enter_completed=len(ends), labels_per_window=[int(row[2]) for row in last],
              chart_opacity_range=[min(chart), max(chart)],
              shimmer_samples=shimmer_samples,
              cases=["all labels including generated chart ticks", "en/ko/ar Reveal restart and completion",
                     "initial ENTER completion", "SCREEN headline overlays", "repeating chart pulse",
                     "visible white shimmer band", "visible chart canvas and bright ticks",
                     "rapid locale replacement", "close during Reveal"],
              limitation="Real host DALi; TV performance and Team callback delivery are not validated.")
(output / "motion-checks.json").write_text(json.dumps(report, indent=2) + "\n")
print(json.dumps(report, indent=2))
