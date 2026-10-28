// Host-only runner. Team owns Application on TV; this file is never in the RPM.
#include "main-window.h"
#include "day-window.h"
#include "activity-window.h"
#include "member-ids.h"
#include "motion-probe.h"
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <dali/devel-api/adaptor-framework/application.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/actors/layer.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <clocale>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <stdexcept>

namespace
{
void DumpLabels(Dali::Actor actor)
{
  if(auto label = Dali::Ui::Label::DownCast(actor))
  {
    std::printf("label: %s\n", label.GetText().CStr());
    std::printf("label-style: %s direction=%d fill=%d\n", label.GetFontFamily().CStr(),
                static_cast<int>(label.GetLayoutDirectionMode()), static_cast<int>(label.GetTextGradient().GetType()));
    const auto size = label.GetCurrentProperty<Dali::Vector3>(Dali::Actor::Property::SIZE);
    std::printf("label-size: %.0f x %.0f\n", size.x, size.y);
  }
  if(auto button = Dali::Ui::InteractiveView::DownCast(actor))
  {
    auto size = button.GetCurrentProperty<Dali::Vector3>(Dali::Actor::Property::SIZE);
    std::printf("button: %.0f x %.0f\n", size.x, size.y);
  }
  for(unsigned i = 0; i < actor.GetChildCount(); ++i)
  {
    DumpLabels(actor.GetChildAt(i));
  }
}

void KeepRequestedPosition(Dali::Window window)
{
  auto display = XOpenDisplay(nullptr);
  if(!display)
  {
    throw std::runtime_error("Cannot open preview X display");
  }
  XSizeHints hints{};
  long       supplied = 0;
  XGetWMNormalHints(display, window.GetNativeId(), &hints, &supplied);
  auto bounds = window.GetPositionSize();
  hints.flags |= USPosition | USSize;
  hints.x      = bounds.x;
  hints.y      = bounds.y;
  hints.width  = bounds.width;
  hints.height = bounds.height;
  XSetWMNormalHints(display, window.GetNativeId(), &hints);
  XSync(display, False);
  XCloseDisplay(display);
}

class Preview : public Dali::ConnectionTracker
{
public:
  Preview(Dali::Application app)
  : mApp(app)
  {
    mApp.InitSignal().Connect(this, &Preview::Create);
  }

private:
  void Create(Dali::Application)
  {
    // Host-only platform event injection: let the actual adaptor update every window's direction.
    Dali::Adaptor::Get().NotifyLanguageChanged(std::setlocale(LC_MESSAGES, nullptr));
    auto main        = Dali::Window::New({1320, 0, 600, 1080}, "Samsong Buxby", true);
    auto day         = Dali::Window::New({736, 64, 520, 448}, "Buxby Day", true);
    auto activity    = Dali::Window::New({736, 568, 520, 448}, "Buxby Activity", true);
    mWindows         = {main, day, activity};
    auto setLanguage = [this](const char* locale)
    {
      if(std::getenv("BUXBY_PREVIEW_SET_FAILURE"))
      {
        return -1;
      }
      if(!std::setlocale(LC_MESSAGES, locale))
      {
        return -1;
      }
      // Only host infrastructure supplies the platform language event. Scenes never set their root direction.
      Dali::Adaptor::Get().NotifyLanguageChanged(locale);
      mMain->OnLocaleChanged(locale);
      mDay->OnLocaleChanged();
      mActivity->OnLocaleChanged();
      DumpScenes();
      return 0;
    };
    auto close = [this]
    {
      DumpScenes();
      mApp.Quit();
    };
    auto activate = [this](const char* id)
    {
      const unsigned index = std::strcmp(id, Buxby::MAIN_ID) == 0 ? 0 : std::strcmp(id, Buxby::DAY_ID) == 0 ? 1 : 2;
      mWindows[index].Activate();
    };
    mMain =
      std::make_unique<Buxby::MainWindow>(main, std::setlocale(LC_MESSAGES, nullptr), setLanguage, close, activate);
    mDay      = std::make_unique<Buxby::DayWindow>(day, activate);
    mActivity = std::make_unique<Buxby::ActivityWindow>(activity, activate);
    if(std::getenv("BUXBY_PREVIEW_MOTION"))
    {
      mMotion = std::make_unique<MotionProbe>(mWindows);
    }
    Dali::Ui::FocusManager::Get().FocusChangedSignal().Connect(this, &Preview::TraceFocus);
    for(auto window : {main, day, activity})
    {
      KeepRequestedPosition(window);
      window.Show();
    }
    mApp.GetWindow().Hide();
    main.Activate();
    DumpScenes();
    if(const char* seconds = std::getenv("BUXBY_PREVIEW_SECONDS"))
    {
      mTimer = Dali::Timer::New(std::max(1, std::atoi(seconds)) * 1000);
      mTimer.TickSignal().Connect(this, &Preview::Finish);
      mTimer.Start();
    }
  }

  void TraceFocus(Dali::Ui::View, Dali::Ui::View current)
  {
    if(std::getenv("BUXBY_PREVIEW_TRACE"))
    {
      std::printf("FOCUS %s\n",
                  current ? current.GetProperty<Dali::String>(Dali::Actor::Property::NAME).CStr() : "none");
      std::fflush(stdout);
    }
  }

  bool Finish()
  {
    DumpScenes();
    mApp.Quit();
    return false;
  }

  void DumpScenes()
  {
    if(std::getenv("BUXBY_PREVIEW_TRACE"))
    {
      std::printf("PREVIEW LOCALE %s\n", std::setlocale(LC_MESSAGES, nullptr));
      for(unsigned i = 0; i < mWindows.size(); ++i)
      {
        std::printf("WINDOW %u\n", i);
        const auto direction = mWindows[i].GetRootLayer().GetProperty<int>(Dali::Actor::Property::LAYOUT_DIRECTION);
        std::printf("window-direction: %s\n", direction == Dali::LayoutDirection::RIGHT_TO_LEFT ? "rtl" : "ltr");
        DumpLabels(mWindows[i].GetRootLayer());
      }
      std::puts("END PREVIEW");
      std::fflush(stdout);
    }
  }

  Dali::Application                      mApp;
  Dali::Timer                            mTimer;
  std::array<Dali::Window, 3>            mWindows;
  std::unique_ptr<Buxby::MainWindow>     mMain;
  std::unique_ptr<Buxby::DayWindow>      mDay;
  std::unique_ptr<Buxby::ActivityWindow> mActivity;
  std::unique_ptr<MotionProbe>           mMotion;
};
}

int main(int argc, char** argv)
{
  const char* locale = std::getenv("BUXBY_PREVIEW_LOCALE");
  if(!std::setlocale(LC_ALL, locale ? locale : "en_US.UTF-8"))
  {
    std::fputs("Requested host locale is not installed\n", stderr);
    return 1;
  }
  Dali::WindowData   data;
  Dali::PositionSize bounds(0, 0, 1, 1);
  data.SetPositionSize(bounds);
  auto app = Dali::Application::New(&argc, &argv, "", false, data);
  Dali::Ui::UiConfig::New().Apply();
  Preview preview(app);
  app.MainLoop();
}
