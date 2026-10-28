// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#pragma once
#include "catalog.h"
#include "style.h"
#include "scene-motion.h"
#include "window-navigation.h"
#include <dali-ui-components/public-api/chart/chart-view.h>
#include <array>
#include <memory>

namespace Buxby
{
class ActivityWindow : public ConnectionTracker
{
public:
  ActivityWindow(Window window, ActivateMember activate);
  ~ActivityWindow() override;
  void OnLocaleChanged();

private:
  void OnChartChildAdded(Actor parent, Actor child);

  Catalog                           mCatalog{"dali-buxby-activity"};
  AbsoluteLayout                    mRoot;
  AbsoluteLayout                    mPlot;
  ChartView                         mChart;
  Label                             mEyebrow;
  Label                             mTotal;
  Label                             mDescription;
  Label                             mTrend;
  Label                             mFooter;
  std::array<Label, 7>              mDays;
  SceneMotion                       mMotion;
  std::unique_ptr<WindowNavigation> mNavigation;
};
}
