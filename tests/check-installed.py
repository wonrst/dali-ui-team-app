#!/usr/bin/env python3
"""Validate the installed member libraries, manifest and compiled translations."""
import gettext
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

root, prefix, package_dir = map(Path, sys.argv[1:])
app = root / str(prefix).lstrip("/")
xml = root / str(package_dir).lstrip("/") / "dali-ui-team-app.xml"
manifest = ET.parse(xml).getroot()
ns = "{http://tizen.org/ns/packages}"
assert manifest.attrib["package"] == manifest.attrib["team-leaderid"] == "dali-ui-team-app"
members = manifest.findall(ns + "ui-application")
assert len(members) == 3 and not manifest.findall(ns + "service-application")
for member, suffix, library in zip(members, ("", ".first", ".second"), ("main", "first", "second")):
    assert member.attrib["appid"] == "dali-ui-team-app" + suffix
    assert member.attrib["exec"] == f"libdali-ui-team-{library}.so"
    assert member.attrib["type"] == "capp"
    assert (app / "bin" / member.attrib["exec"]).read_bytes()[:4] == b"\x7fELF"
assert len(list((app / "bin").iterdir())) == 3
catalogs = list((app / "res/locale").rglob("*.mo"))
assert len(catalogs) == 3
for mo in catalogs:
    with mo.open("rb") as stream:
        catalog = gettext.GNUTranslations(stream)
    assert catalog.info()["language"] == mo.parent.parent.name
print("PASS: three installed ELF members, manifest IDs and catalogs")
