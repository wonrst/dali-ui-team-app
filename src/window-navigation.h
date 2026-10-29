// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#pragma once

#include <dali-ui-foundation/public-api/views/view.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <functional>

namespace TeamSample
{
struct WindowLinks
{
  const char* next;
  const char* previous;
  const char* left;
  const char* right;
  const char* up;
  const char* down;
  const char* back;
};

using ActivateMember = std::function<void(const char*)>;

// DALi restores focus within a window; this class connects the windows.
class WindowNavigation : public Dali::ConnectionTracker
{
public:
  WindowNavigation(Dali::Window window, Dali::Ui::View initialFocus, WindowLinks links, ActivateMember activate,
                   std::function<void()> close = {});

private:
  bool OnKey(Dali::Window window, Dali::KeyEvent event);
  void Activate(const char* id);

  WindowLinks           mLinks;
  ActivateMember        mActivate;
  std::function<void()> mClose;
};
}
