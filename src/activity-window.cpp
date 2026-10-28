// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#include "activity-window.h"
#include "member-ids.h"
#include <dali-ui-components/public-api/chart/chart-axis.h>
#include <dali-ui-components/public-api/chart/line-series.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <utility>

namespace Buxby
{
ActivityWindow::ActivityWindow(Window window, ActivateMember activate)
{
  mRoot = MakeRoot(window, 520, 448, true);
  mRoot.SetName("activity");
  Gradient::Linear background(Vector2(-0.5f, -0.5f), Vector2(0.5f, 0.5f));
  background.SetStopNodes({{0.f, UiColor(0x594077, 0.94f)}, {1.f, UiColor(0x29283F, 0.88f)}});
  mRoot.SetBackgroundGradient(background);
  mEyebrow = MakeLabel(mRoot, 28, 24, 464, 30, 17, MEDIUM, 0xD5C9FF);
  mTotal   = MakeLabel(mRoot, 28, 72, 338, 63, 43, BOLD);
  ApplyHeadlineStyle(mTotal);
  mTrend = MakeLabel(mRoot, 378, 85, 114, 40, 21, SEMIBOLD, MINT);
  mTrend.SetHorizontalTextAlignment(Text::Alignment::END);
  mDescription = MakeLabel(mRoot, 28, 133, 464, 30, 18, LIGHT, MUTED);

  mPlot = AbsoluteLayout::New();
  // A time series keeps chronological left-to-right order in every language.
  mPlot.SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);
  Place(mPlot, 28, 180, 464, 211);
  mRoot.Add(mPlot);
  mChart = ChartView::New(ChartView::Type::LINE, Vector2(464, 180));
  mChart.ChildAddedSignal().Connect(this, &ActivityWindow::OnChartChildAdded);
  for(unsigned i = 0; i < mChart.GetChildCount(); ++i)
  {
    OnChartChildAdded(mChart, mChart.GetChildAt(i));
  }
  mChart.SetProperty(ChartView::Property::BACKGROUND_COLOR, Vector4::ZERO);
  mChart.SetProperty(ChartView::Property::SHOW_LEGEND, false);
  mChart.SetProperty(ChartView::Property::SHOW_TOOLTIP, false);
  mChart.SetProperty(ChartView::Property::HOVER_ENABLED, false);
  mChart.SetProperty(ChartView::Property::TOUCH_ENABLED, false);
  mChart.SetProperty(ChartView::Property::AXIS_LABEL_SIZE, 14.f);
  mChart.SetCornerRadius(16.f);
  Place(mChart, 0, 0, 464, 180);
  mChart.SetAnimationDuration(1100.f);
  auto x = ChartAxis::New();
  x.SetMinimumLimit(0);
  x.SetMaximumLimit(6);
  x.SetLabels({"", "", "", "", "", "", ""});
  x.SetShowGridLinesEnabled(false);
  x.SetAxisLineColor(Vector4(1.f, 1.f, 1.f, 0.2f));
  auto y = ChartAxis::New();
  y.SetMinimumLimit(0);
  y.SetMaximumLimit(70);
  y.SetMinimumStep(35);
  y.SetGridColor(UiColor(0xFFFFFF, 0.15f));
  y.SetAxisLineColor(Vector4(1.f, 1.f, 1.f, 0.2f));
  mChart.SetXAxis(x);
  mChart.SetYAxis(y);
  auto series = LineSeries::New();
  series.SetValues({24.f, 32.f, 28.f, 46.f, 35.f, 48.f, 62.f}); // 275 minutes in this sample week.
  series.SetColor(UiColor(MINT));
  series.SetLineWidth(3.5f);
  series.SetSmoothness(0.3f);
  series.SetMarkersVisible(true);
  series.SetMarkerRadius(3.5f);
  series.SetFillEnabled(true);
  series.SetFillColor(UiColor(MINT, 0.15f));
  mChart.AddSeries(series);
  mPlot.Add(mChart);
  mMotion.StartChartPulse(mChart);
  // Explicit public Labels keep the requested font families and PO ownership.
  for(unsigned i = 0; i < mDays.size(); ++i)
  {
    mDays[i] = MakeLabel(mPlot, 6.f + i * 69.f, 182, 60, 28, 14, REGULAR, MUTED);
    mDays[i].SetHorizontalTextAlignment(Text::Alignment::CENTER);
  }
  mFooter = MakeLabel(mRoot, 28, 403, 464, 25, 14, LIGHT, MUTED);
  OnLocaleChanged();
  mNavigation = std::make_unique<WindowNavigation>(window, mRoot, ACTIVITY_LINKS, std::move(activate));
}

ActivityWindow::~ActivityWindow()
{
  DisconnectAll();
  mNavigation.reset();
  mMotion.Stop();
  mRoot.Unparent();
}

void ActivityWindow::OnChartChildAdded(Actor, Actor child)
{
  if(auto label = Label::DownCast(child))
  {
    // Axis labels are created by ChartView after layout, outside MakeLabel().
    label.SetFontFamily(REGULAR);
    label.SetLayoutDirectionMode(Text::LayoutDirectionMode::INHERIT);
    // ChartView owns the tick positions; reserve readable bounds for this chart's 0..70 scale.
    label.SetProperty(Actor::Property::SIZE, Vector2(32.f, 24.f));
    // This SDK has no axis label color setter. A public text fill survives ChartView's color updates.
    Gradient::Linear fill(Vector2(-0.5f, 0.f), Vector2(0.5f, 0.f));
    fill.SetStopNodes({{0.f, UiColor(MUTED)}, {1.f, UiColor(MUTED)}});
    label.SetTextGradient(fill);
  }
}

void ActivityWindow::OnLocaleChanged()
{
  mEyebrow.SetText(mCatalog.Get("SID_EYEBROW"));
  mRoot.SetAccessibilityName(mCatalog.Get("SID_EYEBROW"));
  mTotal.SetText(mCatalog.Get("SID_TOTAL_TIME"));
  mDescription.SetText(mCatalog.Get("SID_DESCRIPTION"));
  mTrend.SetText(mCatalog.Get("SID_TREND"));
  mFooter.SetText(mCatalog.Get("SID_FOOTER"));
  const std::array<const char*, 7> dayIds{"SID_MONDAY", "SID_TUESDAY",  "SID_WEDNESDAY", "SID_THURSDAY",
                                          "SID_FRIDAY", "SID_SATURDAY", "SID_SUNDAY"};
  for(unsigned i = 0; i < mDays.size(); ++i)
  {
    mDays[i].SetText(mCatalog.Get(dayIds[i]));
  }
  mMotion.RevealText(mRoot);
}
}
