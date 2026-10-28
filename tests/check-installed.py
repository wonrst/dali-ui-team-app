#!/usr/bin/env python3
"""Validate that package metadata resolves to installed ARM/x86 member libraries."""
import gettext
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

root, prefix, package_dir = map(Path, sys.argv[1:])
app_root = root / str(prefix).lstrip("/")
xml = root / str(package_dir).lstrip("/") / "dali-buxby.xml"
apps = ET.parse(xml).getroot().findall("{http://tizen.org/ns/packages}ui-application")
assert len(apps) == 3, xml
for app in apps:
    library = app_root / "bin" / app.attrib["exec"]
    assert library.read_bytes()[:4] == b"\x7fELF", library
for locale in ("en_US", "ko_KR", "ar_AE"):
    for member in ("main", "day", "activity"):
        mo = app_root / "res/locale" / locale / "LC_MESSAGES" / f"dali-buxby-{member}.mo"
        with mo.open("rb") as stream:
            assert gettext.GNUTranslations(stream).info()["language"] == locale
assert len(list((app_root / "bin").iterdir())) == 3
assert not (app_root / "bin/buxby-preview").exists()
print("PASS: three installed ELF members, manifest paths and nine compiled catalogs")
