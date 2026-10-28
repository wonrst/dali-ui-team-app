// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#pragma once

#include "window-navigation.h"

namespace Buxby
{
constexpr const char* MAIN_ID     = "dali-buxby";
constexpr const char* DAY_ID      = "dali-buxby.day";
constexpr const char* ACTIVITY_ID = "dali-buxby.activity";

// Tab, Shift+Tab, Left, Right, Up, Down, Back. A null link stays in the current window.
constexpr WindowLinks MAIN_LINKS{DAY_ID, ACTIVITY_ID, DAY_ID, nullptr, DAY_ID, ACTIVITY_ID, nullptr};
constexpr WindowLinks DAY_LINKS{ACTIVITY_ID, MAIN_ID, nullptr, MAIN_ID, nullptr, ACTIVITY_ID, MAIN_ID};
constexpr WindowLinks ACTIVITY_LINKS{MAIN_ID, DAY_ID, nullptr, MAIN_ID, DAY_ID, nullptr, MAIN_ID};
}
