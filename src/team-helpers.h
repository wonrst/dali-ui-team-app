// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#pragma once

#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <team_manager.hpp>
#include <cstdio>
#include <exception>

namespace Buxby
{
inline void ConfigureUi()
{
  // Team owns the event loop. Apply once per process, before any member window/UI is created.
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
    else
    {
      std::fprintf(stderr, "Buxby: member %s is not running\n", id);
    }
  }
  catch(const std::exception& error)
  {
    std::fprintf(stderr, "Buxby: resume %s: %s\n", id, error.what());
  }
  catch(...)
  {
    std::fprintf(stderr, "Buxby: resume %s failed\n", id);
  }
}
}
