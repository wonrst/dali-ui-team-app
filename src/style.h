// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#pragma once

#include <dali-ui-foundation/public-api/layouts/absolute-layout.h>
#include <dali-ui-foundation/public-api/views/interactive-view.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/animation/animation.h>

namespace Buxby
{
using namespace Dali;
using namespace Dali::Ui;

constexpr unsigned    INK      = 0xF5F5FF;
constexpr unsigned    MUTED    = 0xC5CBE5;
constexpr unsigned    MINT     = 0xBEF5E5;
constexpr const char* LIGHT    = "SamsungOneUI_300";
constexpr const char* REGULAR  = "SamsungOneUI_400";
constexpr const char* MEDIUM   = "SamsungOneUI_500";
constexpr const char* SEMIBOLD = "SamsungOneUI_600";
constexpr const char* BOLD     = "SamsungOneUI_700";

void  Place(View view, float x, float y, float width, float height);
Label MakeLabel(View parent, float x, float y, float width, float height, float size, const char* family = REGULAR,
                unsigned color = INK);
AbsoluteLayout  MakeRoot(Window window, float width, float height, bool satellite);
InteractiveView MakeButton(View parent, float x, float y, float width, float height);
void            ApplyHeadlineStyle(Label label);
}
