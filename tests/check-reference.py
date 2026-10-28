#!/usr/bin/env python3
"""Check translated catalogs, the three-member package and target API boundaries."""
import gettext
import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

source, build = map(Path, sys.argv[1:])
members = ("main", "day", "activity")
locales = ("en_US", "ko_KR", "ar_AE")
for member in members:
    original = None
    for locale in locales:
        mo = build / "locale" / locale / "LC_MESSAGES" / f"dali-buxby-{member}.mo"
        with mo.open("rb") as stream:
            catalog = gettext.GNUTranslations(stream)
        keys = set(catalog._catalog) - {""}
        assert keys, f"Empty catalog: {mo}"
        if original is None:
            original = keys
        assert keys == original, f"Incomplete catalog: {mo}"
        assert all(catalog.gettext(key).strip() for key in keys), mo
        assert catalog.info()["language"] == locale, mo
        assert all(re.fullmatch(r"SID_[A-Z0-9_]+", key) for key in keys), mo
        assert all(catalog.gettext(key) != key for key in keys), f"Untranslated SID: {mo}"
    # Includes indirect lookups such as recommendation and weekday ID arrays.
    code = (source / "src" / f"{member}-window.cpp").read_text()
    used = set(re.findall(r'"(SID_[A-Z0-9_]+)"', code))
    assert used == original, f"Catalog/code mismatch for {member}: {used ^ original}"

ns = {"t": "http://tizen.org/ns/packages"}
manifest = ET.parse(source / "packaging/dali-buxby.xml").getroot()
apps = manifest.findall("t:ui-application", ns)
assert len(apps) == 3 and not manifest.findall("t:service-application", ns)
assert manifest.attrib["team-leaderid"] == manifest.attrib["package"] == "dali-buxby"
for app, member in zip(apps, members):
    expected_id = "dali-buxby" + ("" if member == "main" else "." + member)
    assert app.attrib["appid"] == expected_id
    assert app.attrib["exec"] == f"libdali-buxby-{member}.so"
    assert app.attrib["type"] == "capp"

for path in (source / "src").glob("*.*"):
    code = path.read_text()
    assert "LayoutDirectionMode::CONTENTS" not in code, f"Labels must inherit direction: {path}"
    for pattern in (r'#include\s*[<"][^>"\n]*(?:devel-api|integration-api|internal|private)',
                    r'\b(?:UiLocalizationManager|SetTranslatableText|FontStyleWeight|SetFontWeight|SendMessage|TeamViewApplication|TeamServiceApplication|setlocale|setenv)\s*[(:]'):
        assert not re.search(pattern, code), f"Target API boundary: {path}: {pattern}"
print("PASS: nine SID catalogs, three UI member registrations and target API boundaries")
