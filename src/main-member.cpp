// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#include "app-config.h"
#include "team-helpers.h"
#include <app_core_base.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/views/effects/overlay-effect.h>
#include <dali-ui-foundation/public-api/views/interactive-view.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <team_app_control.hpp>
#include <team_ui_application.hpp>
#include <libintl.h>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <string_view>

namespace TeamSample
{
using namespace Dali;
using namespace Dali::Ui;

namespace
{
int SetApplicationLanguage(const char* locale)
{
  // TODO: use app_locale_manager_set_language() when its public header is available.
  return app_core_locale_manager_set_language(locale);
}
}

class MainMember final : public tizen_appfw::TeamUIApplication, public ConnectionTracker
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
      if(SetApplicationLanguage(LOCALES[0]) != 0)
      {
        std::fprintf(stderr, "Team sample: could not set the initial language\n");
        Exit();
        return;
      }
      CreateUi();
      OnLanguageChanged(LOCALES[0]);
      mNavigation =
        std::make_unique<WindowNavigation>(GetDefaultWindow(), mLanguageButtons[0], MAIN_LINKS, ResumeMember, [this]
      {
        Exit();
      });
      StartSubWindows();
    }
    catch(const std::exception& error)
    {
      std::fprintf(stderr, "Team sample: %s\n", error.what());
      Exit();
    }
  }

  void OnResume() override
  {
    if(mRoot)
    {
      GetDefaultWindow().Show();
      GetDefaultWindow().Activate();
    }
  }

  void OnLanguageChanged(const std::string& language) override
  {
    if(mLabel)
    {
      mLabel.SetText(dgettext(TEXT_DOMAIN, "SID_MAIN_LABEL"));
      UpdateLanguageButtons(language);
    }
  }

  void OnTerminate() override
  {
    mStartupSubscription.reset();
    mNavigation.reset();
    DisconnectAll();
    if(mRoot)
    {
      mRoot.Unparent();
    }
    mLanguageButtons = {};
    mLanguageLabels  = {};
    mLabel.Reset();
    mRoot.Reset();
    for(const char* id : {FIRST_ID, SECOND_ID})
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
        std::fprintf(stderr, "Team sample: terminate %s: %s\n", id, error.what());
      }
    }
  }

  void CreateUi()
  {
    if(!bindtextdomain(TEXT_DOMAIN, LOCALE_DIR) || !bind_textdomain_codeset(TEXT_DOMAIN, "UTF-8"))
    {
      throw std::runtime_error("Could not bind the translation catalog");
    }
    auto window = GetDefaultWindow();
    window.SetBackgroundColor(Color::TRANSPARENT);

    mRoot = StackLayout::New(StackOrientation::VERTICAL);
    mRoot.SetUiScalePolicy(UiScalePolicy::DISABLED);
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    mRoot.SetPadding(36.f);
    mRoot.SetSpacing(24.f);
    mRoot.SetBackgroundColor(UiColor(0x243953, 0.90f));
    window.Add(mRoot);

    mLabel = Label::New();
    mLabel.SetRequestedWidth(MATCH_PARENT);
    mLabel.SetFontFamily("SamsungOneUI_400");
    mLabel.SetFontSize(30.f);
    mLabel.SetTextColor(UiColor(0xF3F6FF));
    mLabel.SetMultiLine(true);
    mLabel.SetLayoutDirectionMode(Text::LayoutDirectionMode::INHERIT);
    mLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mRoot.Add(mLabel);

    auto languages = StackLayout::New(StackOrientation::HORIZONTAL);
    languages.SetRequestedWidth(MATCH_PARENT);
    languages.SetSpacing(12.f);
    mRoot.Add(languages);

    for(unsigned i = 0; i < mLanguageButtons.size(); ++i)
    {
      auto button = InteractiveView::New();
      button.SetRequestedHeight(64.f);
      button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.f));
      button.SetCornerRadius(16.f);
      button.SetBorderlineColor(UiColor(0xFFFFFF));
      button.SetBorderlineOffset(-1.f);
      button.FocusChangedSignal().Connect(this, [](View view, bool focused)
      {
        view.SetBorderlineWidth(focused ? 3.f : 0.f);
      });
      button.SetStateEffect(OverlayEffect::Plain().Configure().SetOverlayColor(UiColor(0xFFFFFF, 0.16f)).Build());
      button.ClickedSignal().Connect(this, [this, i](View, InputEvent)
      {
        if(SetApplicationLanguage(LOCALES[i]) != 0)
        {
          mLabel.SetText(dgettext(TEXT_DOMAIN, "SID_LANGUAGE_ERROR"));
        }
      });
      languages.Add(button);

      auto label = Label::New();
      label.SetRequestedWidth(MATCH_PARENT);
      label.SetRequestedHeight(MATCH_PARENT);
      label.SetFontFamily("SamsungOneUI_400");
      label.SetFontSize(22.f);
      label.SetLayoutDirectionMode(Text::LayoutDirectionMode::INHERIT);
      label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
      label.SetVerticalTextAlignment(Text::Alignment::CENTER);
      button.Add(label);
      mLanguageButtons[i] = button;
      mLanguageLabels[i]  = label;
    }
  }

  void UpdateLanguageButtons(const std::string& language)
  {
    const std::string_view locale(language);
    const auto             languageCode = locale.substr(0, locale.find('.'));
    for(unsigned i = 0; i < mLanguageButtons.size(); ++i)
    {
      const std::string_view buttonLocale(LOCALES[i]);
      const bool             selected = languageCode == buttonLocale.substr(0, buttonLocale.find('.'));
      const char*            text     = dgettext(TEXT_DOMAIN, LANGUAGE_IDS[i]);
      mLanguageLabels[i].SetText(text);
      mLanguageLabels[i].SetTextColor(UiColor(selected ? 0x183846 : 0xF3F6FF));
      mLanguageButtons[i].SetBackgroundColor(UiColor(selected ? 0xBEF5E5 : 0x17243D));
      mLanguageButtons[i].SetAccessibilityName(text);
    }
  }

  void StartSubWindows()
  {
    mStartingMembers = {FIRST_ID, SECOND_ID};
    mStartupSubscription =
      tizen_appfw::TeamManager::ObserveLifecycle([this](const tizen_appfw::TeamApplicationRunningContext& context,
                                                        tizen_appfw::TeamApplicationLifecycleState        state)
    {
      if(state == tizen_appfw::TeamApplicationLifecycleState::Resumed)
      {
        mStartingMembers.erase(context.GetApplicationId());
        CompleteStartup();
      }
    });
    for(const char* id : {FIRST_ID, SECOND_ID})
    {
      if(auto member = tizen_appfw::TeamManager::FindRunningApplication(id))
      {
        // An already resumed member will not emit another state transition.
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

  void CompleteStartup()
  {
    if(mStartupSubscription && mStartingMembers.empty())
    {
      // Select Main once after both children resume; Team defers this request from observers.
      mStartupSubscription.reset();
      ResumeMember(MAIN_ID);
    }
  }

  StackLayout                                                      mRoot;
  Label                                                            mLabel;
  std::array<InteractiveView, 3>                                   mLanguageButtons;
  std::array<Label, 3>                                             mLanguageLabels;
  std::unique_ptr<WindowNavigation>                                mNavigation;
  std::set<std::string>                                            mStartingMembers;
  std::optional<tizen_appfw::TeamApplicationLifecycleSubscription> mStartupSubscription;
};
}
TEAM_UI_APPLICATION_REGISTER(TeamSample::MainMember)
