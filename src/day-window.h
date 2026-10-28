// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#pragma once
#include "catalog.h"
#include "style.h"
#include "scene-motion.h"
#include "window-navigation.h"
#include <array>
#include <memory>

namespace Buxby
{
class DayWindow
{
public:
  DayWindow(Window window, ActivateMember activate);
  ~DayWindow();
  void OnLocaleChanged();

private:
  Catalog                           mCatalog{"dali-buxby-day"};
  AbsoluteLayout                    mRoot;
  Label                             mEyebrow;
  Label                             mHeadline;
  Label                             mFooter;
  std::array<Label, 2>              mTimes;
  std::array<Label, 2>              mTitles;
  std::array<Label, 2>              mDescriptions;
  SceneMotion                       mMotion;
  std::unique_ptr<WindowNavigation> mNavigation;
};
}
