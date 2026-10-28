// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#include "day-window.h"
#include "member-ids.h"
#include <utility>

namespace Buxby
{
DayWindow::DayWindow(Window window, ActivateMember activate)
{
  mRoot = MakeRoot(window, 520, 448, true);
  mRoot.SetName("day");
  mEyebrow  = MakeLabel(mRoot, 28, 24, 464, 30, 17, MEDIUM, MINT);
  mHeadline = MakeLabel(mRoot, 28, 70, 464, 90, 34, SEMIBOLD);
  ApplyHeadlineStyle(mHeadline);
  for(unsigned i = 0; i < mTimes.size(); ++i)
  {
    const float y = 192.f + i * 86.f;
    mTimes[i]     = MakeLabel(mRoot, 28, y, 100, 36, 25, BOLD, MINT);
    auto accent   = View::New();
    accent.SetBackgroundColor(UiColor(MINT, 0.3f));
    accent.SetCornerRadius(2.f);
    Place(accent, 140, y + 3, 3, 51);
    mRoot.Add(accent);
    mTitles[i]       = MakeLabel(mRoot, 162, y, 330, 33, 23, SEMIBOLD);
    mDescriptions[i] = MakeLabel(mRoot, 162, y + 36, 330, 28, 17, LIGHT, MUTED);
  }
  mFooter = MakeLabel(mRoot, 28, 391, 464, 31, 15, REGULAR, MUTED);
  OnLocaleChanged();
  mNavigation = std::make_unique<WindowNavigation>(window, mRoot, DAY_LINKS, std::move(activate));
}

DayWindow::~DayWindow()
{
  mNavigation.reset();
  mMotion.Stop();
  mRoot.Unparent();
}

void DayWindow::OnLocaleChanged()
{
  mEyebrow.SetText(mCatalog.Get("SID_EYEBROW"));
  mHeadline.SetText(mCatalog.Get("SID_TITLE"));
  mRoot.SetAccessibilityName(mCatalog.Get("SID_TITLE"));
  mTimes[0].SetText(mCatalog.Get("SID_WALK_TIME"));
  mTimes[1].SetText(mCatalog.Get("SID_UNWIND_TIME"));
  mTitles[0].SetText(mCatalog.Get("SID_WALK_TITLE"));
  mTitles[1].SetText(mCatalog.Get("SID_UNWIND_TITLE"));
  mDescriptions[0].SetText(mCatalog.Get("SID_WALK_DESCRIPTION"));
  mDescriptions[1].SetText(mCatalog.Get("SID_UNWIND_DESCRIPTION"));
  mFooter.SetText(mCatalog.Get("SID_FOOTER"));
  mMotion.RevealText(mRoot);
}
}
