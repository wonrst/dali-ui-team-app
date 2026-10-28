// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#pragma once

#include "style.h"
#include <functional>

namespace Buxby
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

// DALi owns per-window focus and restoration. This class adds navigation between windows.
class WindowNavigation : public ConnectionTracker
{
public:
  WindowNavigation(Window window, View initialFocus, WindowLinks links, ActivateMember activate);
  ~WindowNavigation();

private:
  bool OnKey(Window, KeyEvent event);
  void Activate(const char* id);

  Window         mWindow;
  WindowLinks    mLinks;
  ActivateMember mActivate;
};
}
