// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#include "main-window.h"
#include "member-ids.h"
#include "team-helpers.h"
#include <app_core_base.h>
#include <team_ui_application.hpp>
#include <team_app_control.hpp>
#include <team_manager.hpp>
#include <cstdio>
#include <memory>
#include <optional>
#include <set>
#include <string>

namespace Buxby
{
namespace
{
int SetApplicationLanguage(const char* locale)
{
  // TODO: use app_locale_manager_set_language() once it is available in a public app-common header.
  // Temporary TV platform dependency: app-core-cpp / app_core_base.h.
  return app_core_locale_manager_set_language(locale);
}
}

class MainMember final : public tizen_appfw::TeamUIApplication
{
  void OnPreCreate() override
  {
    ConfigureUi();
    Dali::BoundsInteger bounds(1320, 0, 600, 1080);
    GetDefaultWindowData().SetPositionSize(bounds);
    GetDefaultWindowData().SetTransparency(true);
  }

  void OnCreate() override
  {
    try
    {
      const int result = SetApplicationLanguage("en_US.UTF-8");
      if(result != 0)
      {
        std::fprintf(stderr, "Buxby: initial application locale failed (%d)\n", result);
        Exit();
        return;
      }
      auto close = [this]
      {
        Exit();
      };
      mScene =
        std::make_unique<MainWindow>(GetDefaultWindow(), "en_US.UTF-8", SetApplicationLanguage, close, ResumeMember);
      GetDefaultWindow().Show();
      mStartingMembers = {DAY_ID, ACTIVITY_ID};
      mStartupSubscription =
        tizen_appfw::TeamManager::ObserveLifecycle([this](const tizen_appfw::TeamApplicationRunningContext& context,
                                                          tizen_appfw::TeamApplicationLifecycleState        state)
      {
        OnMemberStarted(context, state);
      });
      for(const char* id : {DAY_ID, ACTIVITY_ID})
      {
        if(auto member = tizen_appfw::TeamManager::FindRunningApplication(id))
        {
          // Lifecycle observers report transitions, not a snapshot of already running members.
          if(member->GetState() == tizen_appfw::TeamApplicationLifecycleState::Resumed)
          {
            mStartingMembers.erase(id);
          }
          else
          {
            member->Resume();
          }
        }
        else
        {
          tizen_appfw::TeamAppControl control;
          control.SetOperation("http://tizen.org/appcontrol/operation/view");
          control.SetAppId(id);
          control.SendLaunchRequest();
        }
      }
      CompleteStartup();
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

  // This is the name exposed by the Team C++ API, including version 1.3.6.
  void OnLanguageChanged(const std::string& language) override
  {
    if(mScene)
    {
      mScene->OnLocaleChanged(language);
    }
  }

  void OnTerminate() override
  {
    mStartupSubscription.reset();
    mStartingMembers.clear();
    mScene.reset();
    for(const char* id : {DAY_ID, ACTIVITY_ID})
    {
      try
      {
        if(auto member = tizen_appfw::TeamManager::FindRunningApplication(id))
        {
          member->Terminate();
        }
      }
      catch(const std::exception& error)
      {
        std::fprintf(stderr, "Buxby: terminate %s: %s\n", id, error.what());
      }
    }
  }

  void OnMemberStarted(const tizen_appfw::TeamApplicationRunningContext& context,
                       tizen_appfw::TeamApplicationLifecycleState        state)
  {
    if(state == tizen_appfw::TeamApplicationLifecycleState::Resumed &&
       mStartingMembers.erase(context.GetApplicationId()))
    {
      CompleteStartup();
    }
  }

  void CompleteStartup()
  {
    if(mStartupSubscription && mStartingMembers.empty())
    {
      // Choose the initial active window once, after both children have resumed.
      // Team defers Resume requests made inside a lifecycle observer.
      mStartupSubscription.reset();
      ResumeMember(MAIN_ID);
    }
  }

  std::unique_ptr<MainWindow>                                      mScene;
  std::set<std::string>                                            mStartingMembers;
  std::optional<tizen_appfw::TeamApplicationLifecycleSubscription> mStartupSubscription;
};
}
TEAM_UI_APPLICATION_REGISTER(Buxby::MainMember)
