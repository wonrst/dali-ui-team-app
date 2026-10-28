// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#include "style.h"
#include "scene-motion.h"
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout-params.h>
#include <dali-ui-foundation/public-api/views/effects/overlay-effect.h>

namespace Buxby
{
namespace
{
void PaintPanelFocus(View panel, bool focused)
{
  panel.SetBorderlineWidth(focused ? 3.f : 1.f);
  panel.SetBorderlineColor(UiColor(0xD7E3FF, focused ? 0.95f : 0.25f));
}

void PaintButtonFocus(View button, bool focused)
{
  button.SetBorderlineWidth(focused ? 3.f : 0.f);
  button.SetBorderlineColor(UiColor(0xFFFFFF, 0.95f));
  button.SetBorderlineOffset(-1.f);
}
}

void Place(View view, float x, float y, float width, float height)
{
  view.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(x, y, width, height)));
}

Label MakeLabel(View parent, float x, float y, float width, float height, float size, const char* family,
                unsigned color)
{
  auto label = Label::New();
  label.SetFontFamily(family);
  label.SetFontSize(size);
  label.SetTextColor(UiColor(color));
  label.SetMultiLine(true);
  label.SetLineHeightMode(Text::LineHeightMode::RELATIVE);
  label.SetLineHeight(1.12f);
  label.SetLayoutDirectionMode(Text::LayoutDirectionMode::INHERIT);
  label.SetHorizontalTextAlignment(Text::Alignment::START);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  Place(label, x, y, width, height);
  parent.Add(label);
  return label;
}

AbsoluteLayout MakeRoot(Window window, float width, float height, bool satellite)
{
  window.SetBackgroundColor(Color::TRANSPARENT);
  auto root = AbsoluteLayout::New();
  root.SetUiScalePolicy(UiScalePolicy::DISABLED);
  root.SetRequestedWidth(width);
  root.SetRequestedHeight(height);
  root.SetLayoutTransition(MakeItemTransition());
  Gradient::Linear gradient(Vector2(-0.5f, -0.5f), Vector2(0.5f, 0.5f));
  if(satellite)
  {
    root.SetFocusable(true);
    root.FocusChangedSignal().Connect(&PaintPanelFocus);
    root.SetCornerRadius(28.f);
    root.SetBorderlineWidth(1.f);
    root.SetBorderlineOffset(-1.f);
    root.SetBorderlineColor(UiColor(0xD7E3FF, 0.25f));
    gradient.SetStopNodes({{0.f, UiColor(0x274D67, 0.93f)}, {1.f, UiColor(0x29283F, 0.86f)}});
  }
  else
  {
    gradient.SetStopNodes({{0.f, UiColor(0x67429A, 0.90f)},
                           {0.40f, UiColor(0x285D91, 0.88f)},
                           {0.73f, UiColor(0x20566B, 0.88f)},
                           {1.f, UiColor(0x533A70, 0.90f)}});
  }
  root.SetBackgroundGradient(gradient);
  window.Add(root);
  return root;
}

InteractiveView MakeButton(View parent, float x, float y, float width, float height)
{
  auto button = InteractiveView::New();
  button.SetFocusable(true);
  button.FocusChangedSignal().Connect(&PaintButtonFocus);
  button.SetCornerRadius(20.f);
  button.SetBackgroundColor(UiColor(0x17243D, 0.62f));
  button.SetStateEffect(
    OverlayEffect::Plain().Configure().SetOverlayColor(UiColor(0xE9EEFF, 0.20f)).SetRecoilScaleFactor(0.97f).Build());
  Place(button, x, y, width, height);
  parent.Add(button);
  return button;
}

void ApplyHeadlineStyle(Label label)
{
  Gradient::Linear gradient(Vector2(-0.5f, 0.f), Vector2(0.5f, 0.f));
  gradient.SetStopNodes({{0.f, UiColor(0xC8AEFF)}, {0.52f, UiColor(0x8ACFFF)}, {1.f, UiColor(0x83E9D3)}});
  label.SetTextGradientBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
  label.SetTextGradient(gradient);

  ConfigureShimmer(label);
}
}
