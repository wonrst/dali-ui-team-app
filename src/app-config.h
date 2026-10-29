// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#pragma once

#include "window-navigation.h"
#include <array>

namespace TeamSample
{
constexpr const char* MAIN_ID     = "dali-ui-team-app";
constexpr const char* FIRST_ID    = "dali-ui-team-app.first";
constexpr const char* SECOND_ID   = "dali-ui-team-app.second";
constexpr const char* TEXT_DOMAIN = "dali-ui-team-app";

constexpr std::array<const char*, 3> LOCALES{"en_US.UTF-8", "ko_KR.UTF-8", "ar_AE.UTF-8"};
constexpr std::array<const char*, 3> LANGUAGE_IDS{"SID_ENGLISH", "SID_KOREAN", "SID_ARABIC"};

// Tab, Shift+Tab, Left, Right, Up, Down, Back. Null keeps focus in this window.
constexpr WindowLinks MAIN_LINKS{FIRST_ID, SECOND_ID, FIRST_ID, nullptr, FIRST_ID, SECOND_ID, nullptr};
constexpr WindowLinks FIRST_LINKS{SECOND_ID, MAIN_ID, nullptr, MAIN_ID, nullptr, SECOND_ID, MAIN_ID};
constexpr WindowLinks SECOND_LINKS{MAIN_ID, FIRST_ID, nullptr, MAIN_ID, FIRST_ID, nullptr, MAIN_ID};
}
