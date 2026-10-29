// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#pragma once

#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <team_manager.hpp>
#include <cstdio>
#include <exception>

namespace TeamSample
{
inline void ConfigureUi()
{
  // Team owns the event loop. Apply once, before a member creates its window.
  if(!Dali::Ui::UiConfig::HasCurrent())
  {
    Dali::Ui::UiConfig::New().Apply();
  }
}

inline void ResumeMember(const char* id) noexcept
{
  try
  {
    if(auto member = tizen_appfw::TeamManager::FindRunningApplication(id))
    {
      member->Resume();
    }
  }
  catch(const std::exception& error)
  {
    std::fprintf(stderr, "Team sample: resume %s: %s\n", id, error.what());
  }
  catch(...)
  {
    std::fprintf(stderr, "Team sample: resume %s failed\n", id);
  }
}
}
