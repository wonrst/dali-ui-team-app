// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#include "main-window.h"
#include "member-ids.h"
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/adaptor-framework/key.h>
#include <string_view>
#include <utility>

namespace Buxby
{
namespace
{
constexpr std::array<const char*, 3> LOCALES{"en_US.UTF-8", "ko_KR.UTF-8", "ar_AE.UTF-8"};
constexpr std::array<const char*, 3> LANGUAGE_NAME_IDS{"SID_LANGUAGE_ENGLISH", "SID_LANGUAGE_KOREAN",
                                                       "SID_LANGUAGE_ARABIC"};
constexpr std::array<const char*, 2> TITLE_IDS{"SID_EVENING_TITLE", "SID_REFRESH_TITLE"};
constexpr std::array<const char*, 2> DESCRIPTION_IDS{"SID_EVENING_DESCRIPTION", "SID_REFRESH_DESCRIPTION"};
}

MainWindow::MainWindow(Window window, const std::string& language, std::function<int(const char*)> setLanguage,
                       std::function<void()> close, ActivateMember activate)
: mWindow(window),
  mSetLanguage(std::move(setLanguage)),
  mClose(std::move(close))
{
  mRoot     = MakeRoot(window, 600, 1080, false);
  mBrand    = MakeLabel(mRoot, 36, 30, 528, 42, 25, BOLD);
  mTagline  = MakeLabel(mRoot, 36, 80, 528, 32, 18, LIGHT, MUTED);
  mEyebrow  = MakeLabel(mRoot, 36, 151, 528, 28, 16, MEDIUM, MINT);
  mHeadline = MakeLabel(mRoot, 36, 193, 528, 148, 49, SEMIBOLD);
  ApplyHeadlineStyle(mHeadline);
  mDescription      = MakeLabel(mRoot, 36, 355, 528, 65, 22, LIGHT, MUTED);
  mSuggestionsTitle = MakeLabel(mRoot, 36, 461, 528, 30, 21, SEMIBOLD);

  mSuggestions = StackLayout::New(StackOrientation::VERTICAL);
  mSuggestions.SetSpacing(14.f);
  Place(mSuggestions, 36, 509, 528, 250);
  mRoot.Add(mSuggestions);
  mSuggestions.SetLayoutTransition(MakeItemTransition(true));
  for(unsigned i = 0; i < mCards.size(); ++i)
  {
    mCards[i] = MakeButton(mSuggestions, 0, 0, 528, 82);
    mCards[i].SetName(i == 0 ? "suggestion-0" : "suggestion-1");
    mCards[i].SetClippingMode(ClippingMode::CLIP_TO_BOUNDING_BOX);
    mCards[i].SetRequestedWidth(MATCH_PARENT);
    mCards[i].SetRequestedHeight(i == mExpanded ? 150.f : 82.f);
    auto content = AbsoluteLayout::New();
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(MATCH_PARENT);
    mCards[i].Add(content);
    mCardTitles[i]       = MakeLabel(content, 24, 17, 480, 42, 23, SEMIBOLD);
    mCardDescriptions[i] = MakeLabel(content, 24, 65, 480, 63, 18, REGULAR, MUTED);
    mCardDescriptions[i].SetVisible(i == mExpanded);
    mCards[i].ClickedSignal().Connect(this, [this, i](View, InputEvent)
    {
      ExpandSuggestion(i);
    });
  }

  auto divider = View::New();
  Place(divider, 36, 815, 528, 1);
  divider.SetBackgroundColor(UiColor(0xFFFFFF, 0.15f));
  mRoot.Add(divider);
  mLanguageTitle = MakeLabel(mRoot, 36, 845, 528, 32, 20, MEDIUM);
  for(unsigned i = 0; i < mLanguageButtons.size(); ++i)
  {
    auto button = MakeButton(mRoot, 36.f + 180.f * i, 894, 168, 64);
    button.SetName(LOCALES[i]);
    auto content = AbsoluteLayout::New();
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(MATCH_PARENT);
    button.Add(content);
    mLanguageLabels[i] = MakeLabel(content, 8, 0, 152, 64, 22, SEMIBOLD);
    mLanguageLabels[i].SetHorizontalTextAlignment(Text::Alignment::CENTER);
    button.ClickedSignal().Connect(this, [this, i](View, InputEvent)
    {
      SelectLanguage(i);
    });
    mLanguageButtons[i] = button;
  }
  mFooter = MakeLabel(mRoot, 36, 985, 528, 48, 16, LIGHT, MUTED);
  mError  = MakeLabel(mRoot, 36, 1036, 528, 28, 14, REGULAR, 0xFFD2B7);
  mWindow.KeyEventSignal().Connect(this, &MainWindow::OnKey);
  OnLocaleChanged(language);
  mNavigation = std::make_unique<WindowNavigation>(window, mLanguageButtons[0], MAIN_LINKS, std::move(activate));
}

MainWindow::~MainWindow()
{
  DisconnectAll();
  mNavigation.reset();
  mMotion.Stop();
  mRoot.Unparent();
}

void MainWindow::OnLocaleChanged(const std::string& language)
{
  mBrand.SetText(mCatalog.Get("SID_APP_NAME"));
  mTagline.SetText(mCatalog.Get("SID_TAGLINE"));
  mEyebrow.SetText(mCatalog.Get("SID_EYEBROW"));
  mHeadline.SetText(mCatalog.Get("SID_TITLE"));
  mDescription.SetText(mCatalog.Get("SID_DESCRIPTION"));
  mSuggestionsTitle.SetText(mCatalog.Get("SID_SUGGESTIONS_TITLE"));
  mLanguageTitle.SetText(mCatalog.Get("SID_LANGUAGE_TITLE"));
  mFooter.SetText(mCatalog.Get("SID_FOOTER"));
  mError.SetText("");
  for(unsigned i = 0; i < mCards.size(); ++i)
  {
    mCardTitles[i].SetText(mCatalog.Get(TITLE_IDS[i]));
    mCardDescriptions[i].SetText(mCatalog.Get(DESCRIPTION_IDS[i]));
    mCards[i].SetAccessibilityName(mCatalog.Get(TITLE_IDS[i]));
  }
  // Team may report a locale with or without its encoding suffix.
  const std::string_view currentLocale(language);
  const auto             languageCode = currentLocale.substr(0, currentLocale.find('.'));
  for(unsigned i = 0; i < mLanguageButtons.size(); ++i)
  {
    const std::string_view buttonLocale(LOCALES[i]);
    const bool             selected = languageCode == buttonLocale.substr(0, buttonLocale.find('.'));
    mLanguageLabels[i].SetText(mCatalog.Get(LANGUAGE_NAME_IDS[i]));
    mLanguageLabels[i].SetTextColor(UiColor(selected ? 0x183846 : INK));
    mLanguageButtons[i].SetBackgroundColor(UiColor(selected ? MINT : 0x17243D, selected ? 1.f : 0.62f));
    mLanguageButtons[i].SetAccessibilityName(mCatalog.Get(LANGUAGE_NAME_IDS[i]));
  }
  mMotion.RevealText(mRoot);
}

void MainWindow::SelectLanguage(unsigned index)
{
  if(index < LOCALES.size() && mSetLanguage(LOCALES[index]) != 0)
  {
    mError.SetText(mCatalog.Get("SID_LANGUAGE_CHANGE_ERROR"));
    mMotion.RevealText(mError);
  }
}

void MainWindow::ExpandSuggestion(unsigned index)
{
  mExpanded = index;
  for(unsigned i = 0; i < mCards.size(); ++i)
  {
    mCardDescriptions[i].SetVisible(i == mExpanded);
    mCards[i].SetRequestedHeight(i == mExpanded ? 150.f : 82.f);
  }
  mMotion.RevealText(mCardDescriptions[index]);
}

void MainWindow::OnKey(Window, KeyEvent event)
{
  if(event.GetState() != KeyEvent::DOWN)
  {
    return;
  }
  if(IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK))
  {
    mClose();
  }
  for(unsigned i = 0; i < LOCALES.size(); ++i)
  {
    if(event.GetKeyName().CStr() == std::to_string(i + 1))
    {
      SelectLanguage(i);
    }
  }
}
}
