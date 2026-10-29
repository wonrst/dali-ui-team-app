#!/usr/bin/env python3
"""Check that every displayed SID has a translation and target APIs stay public."""
import gettext
import re
import sys
from pathlib import Path

source, build = map(Path, sys.argv[1:])
code = "\n".join(p.read_text() for p in sorted((source / "src").glob("*")))
used = set(re.findall(r'"(SID_[A-Z0-9_]+)"', code))
assert used
for locale in ("en_US", "ko_KR", "ar_AE"):
    mo = build / "locale" / locale / "LC_MESSAGES/dali-ui-team-app.mo"
    with mo.open("rb") as stream:
        catalog = gettext.GNUTranslations(stream)
    assert catalog.info()["language"] == locale
    assert set(catalog._catalog) - {""} == used, f"Catalog/code mismatch: {locale}"
    assert all(catalog.gettext(sid).strip() and catalog.gettext(sid) != sid for sid in used)
for pattern in (r'#include\s*[<"][^>"\n]*(?:devel-api|integration-api|internal|private)',
                r'\b(?:UiLocalizationManager|FontStyleWeight|SendMessage|TeamViewApplication|TeamServiceApplication|setlocale|setenv)\s*[(:]'):
    assert not re.search(pattern, code), f"Unexpected target dependency: {pattern}"
print("PASS: three complete SID catalogs and target API boundaries")
