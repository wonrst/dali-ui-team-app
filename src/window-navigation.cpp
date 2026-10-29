// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#include "window-navigation.h"
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <dali/public-api/adaptor-framework/key.h>
#include <dali/public-api/events/key-event.h>
#include <string>
#include <utility>

namespace TeamSample
{
using namespace Dali;
using namespace Dali::Ui;

WindowNavigation::WindowNavigation(Window window, View initialFocus, WindowLinks links, ActivateMember activate,
                                   std::function<void()> close)
: mLinks(links),
  mActivate(std::move(activate)),
  mClose(std::move(close))
{
  window.SetAcceptFocus(true);
  window.InterceptKeyEventSignal().Connect(this, &WindowNavigation::OnKey);
  FocusManager::Get().RequestFocus(initialFocus);
}

void WindowNavigation::Activate(const char* id)
{
  if(id)
  {
    mActivate(id);
  }
}

bool WindowNavigation::OnKey(Window window, KeyEvent event)
{
  if(event.GetState() != KeyEvent::DOWN)
  {
    return false;
  }
  const std::string key = event.GetKeyName().CStr();
  if(key == "Tab" || key == "ISO_Left_Tab")
  {
    Activate(key == "ISO_Left_Tab" || event.IsShiftModifier() ? mLinks.previous : mLinks.next);
    return true;
  }
  if(IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK))
  {
    if(mLinks.back)
    {
      Activate(mLinks.back);
    }
    else if(mClose)
    {
      mClose();
    }
    return true;
  }

  FocusDirection direction;
  const char*    boundary;
  if(key == "Left")
  {
    direction = FocusDirection::LEFT;
    boundary  = mLinks.left;
  }
  else if(key == "Right")
  {
    direction = FocusDirection::RIGHT;
    boundary  = mLinks.right;
  }
  else if(key == "Up")
  {
    direction = FocusDirection::UP;
    boundary  = mLinks.up;
  }
  else if(key == "Down")
  {
    direction = FocusDirection::DOWN;
    boundary  = mLinks.down;
  }
  else
  {
    return false;
  }
  // Follow physical view positions, including RTL, before crossing a boundary.
  if(!FocusManager::Get().MoveFocus(window, direction))
  {
    Activate(boundary);
  }
  return true;
}
}
