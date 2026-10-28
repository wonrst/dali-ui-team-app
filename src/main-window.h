// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#pragma once
#include "catalog.h"
#include "style.h"
#include "scene-motion.h"
#include "window-navigation.h"
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <array>
#include <functional>
#include <memory>
#include <string>

namespace Buxby
{
class MainWindow : public ConnectionTracker
{
public:
  MainWindow(Window window, const std::string& language, std::function<int(const char*)> setLanguage,
             std::function<void()> close, ActivateMember activate);
  ~MainWindow();
  void OnLocaleChanged(const std::string& language);

private:
  void SelectLanguage(unsigned index);
  void ExpandSuggestion(unsigned index);
  void OnKey(Window, KeyEvent event);

  Catalog                           mCatalog{"dali-buxby-main"};
  Window                            mWindow;
  AbsoluteLayout                    mRoot;
  StackLayout                       mSuggestions;
  Label                             mBrand;
  Label                             mTagline;
  Label                             mEyebrow;
  Label                             mHeadline;
  Label                             mDescription;
  Label                             mSuggestionsTitle;
  Label                             mLanguageTitle;
  Label                             mFooter;
  Label                             mError;
  std::array<InteractiveView, 3>    mLanguageButtons;
  std::array<Label, 3>              mLanguageLabels;
  std::array<InteractiveView, 2>    mCards;
  std::array<Label, 2>              mCardTitles;
  std::array<Label, 2>              mCardDescriptions;
  SceneMotion                       mMotion;
  std::function<int(const char*)>   mSetLanguage;
  std::function<void()>             mClose;
  unsigned                          mExpanded{0};
  std::unique_ptr<WindowNavigation> mNavigation;
};
}
