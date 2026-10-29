// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#include "app-config.h"
#include "team-helpers.h"
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <team_ui_application.hpp>
#include <libintl.h>
#include <memory>
#include <stdexcept>

namespace TeamSample
{
using namespace Dali;
using namespace Dali::Ui;

class FirstMember final : public tizen_appfw::TeamUIApplication, public ConnectionTracker
{
  void OnPreCreate() override
  {
    ConfigureUi();
    Dali::BoundsInteger bounds(736, 64, 520, 448);
    GetDefaultWindowData().SetPositionSize(bounds);
    GetDefaultWindowData().SetTransparency(true);
  }

  void OnCreate() override
  {
    try
    {
      CreateUi();
      OnLanguageChanged({});
      mNavigation = std::make_unique<WindowNavigation>(GetDefaultWindow(), mRoot, FIRST_LINKS, ResumeMember);
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

  void OnLanguageChanged(const std::string&) override
  {
    if(mLabel)
    {
      const char* text = dgettext(TEXT_DOMAIN, "SID_FIRST_LABEL");
      mLabel.SetText(text);
      mRoot.SetAccessibilityName(text);
    }
  }

  void OnTerminate() override
  {
    mNavigation.reset();
    DisconnectAll();
    if(mRoot)
    {
      mRoot.Unparent();
    }
    mLabel.Reset();
    mRoot.Reset();
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
    mRoot.SetPadding(28.f);
    mRoot.SetBackgroundColor(UiColor(0x314C62, 0.84f));
    mRoot.SetCornerRadius(28.f);
    mRoot.SetFocusable(true);
    mRoot.SetBorderlineColor(UiColor(0xFFFFFF));
    mRoot.SetBorderlineOffset(-1.f);
    mRoot.FocusChangedSignal().Connect(this, [](View view, bool focused)
    {
      view.SetBorderlineWidth(focused ? 3.f : 0.f);
    });
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
  }

  StackLayout                       mRoot;
  Label                             mLabel;
  std::unique_ptr<WindowNavigation> mNavigation;
};
}
TEAM_UI_APPLICATION_REGISTER(TeamSample::FirstMember)
