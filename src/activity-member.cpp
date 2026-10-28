// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#include "activity-window.h"
#include "team-helpers.h"
#include <team_ui_application.hpp>
#include <cstdio>
#include <memory>

namespace Buxby
{
class ActivityMember final : public tizen_appfw::TeamUIApplication
{
  void OnPreCreate() override
  {
    ConfigureUi();
    Dali::BoundsInteger bounds(736, 568, 520, 448);
    GetDefaultWindowData().SetPositionSize(bounds);
    GetDefaultWindowData().SetTransparency(true);
  }

  void OnCreate() override
  {
    try
    {
      mScene = std::make_unique<ActivityWindow>(GetDefaultWindow(), ResumeMember);
      GetDefaultWindow().Show();
    }
    catch(const std::exception& error)
    {
      std::fprintf(stderr, "Buxby: %s\n", error.what());
      Exit();
    }
  }

  void OnResume() override
  {
    if(mScene)
    {
      GetDefaultWindow().Show();
      GetDefaultWindow().Activate();
    }
  }

  void OnLanguageChanged(const std::string&) override
  {
    if(mScene)
    {
      mScene->OnLocaleChanged();
    }
  }

  void OnTerminate() override
  {
    mScene.reset();
  }

  std::unique_ptr<ActivityWindow> mScene;
};
}
TEAM_UI_APPLICATION_REGISTER(Buxby::ActivityMember)
