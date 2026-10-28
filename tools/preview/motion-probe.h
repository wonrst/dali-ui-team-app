// Host-only observations of actual DALi animation state; never installed in the RPM.
#pragma once

#include "scene-motion.h"
#include <dali-ui-components/public-api/chart/chart-view.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/actors/layer.h>
#include <algorithm>
#include <array>
#include <clocale>
#include <cstdio>

namespace
{
void TraceTransitionStart(Dali::Ui::View view, Dali::Ui::LayoutTransitionSlot slot)
{
  std::printf("TRANSITION start %u %d\n", view.GetId(), static_cast<int>(slot));
}

void TraceTransitionEnd(Dali::Ui::View view, Dali::Ui::LayoutTransitionSlot slot)
{
  std::printf("TRANSITION end %u %d\n", view.GetId(), static_cast<int>(slot));
}

class MotionProbe : public Dali::ConnectionTracker
{
public:
  explicit MotionProbe(std::array<Dali::Window, 3> windows)
  : mWindows(windows),
    mTimer(Dali::Timer::New(80))
  {
    for(auto window : mWindows)
    {
      ObserveTransitions(window.GetRootLayer());
    }
    mTimer.TickSignal().Connect(this, &MotionProbe::Sample);
    mTimer.Start();
  }

  ~MotionProbe()
  {
    mTimer.Stop();
  }

private:
  struct Snapshot
  {
    unsigned labels{0};
    unsigned invalid{0};
    unsigned shimmers{0};
    float    minimum{1.f};
    float    maximum{0.f};
    float    chartOpacity{-1.f};
  };

  void ObserveTransitions(Dali::Actor actor)
  {
    if(auto view = Dali::Ui::View::DownCast(actor))
    {
      if(auto transition = view.GetLayoutTransition())
      {
        transition.SetOnStart(Dali::Ui::LayoutLifecycleCallback::New(&TraceTransitionStart));
        transition.SetOnFinished(Dali::Ui::LayoutLifecycleCallback::New(&TraceTransitionEnd));
      }
    }
    for(unsigned i = 0; i < actor.GetChildCount(); ++i)
    {
      ObserveTransitions(actor.GetChildAt(i));
    }
  }

  void Inspect(Dali::Actor actor, Snapshot& state)
  {
    using namespace Dali::Ui;
    if(auto chart = ChartView::DownCast(actor))
    {
      state.chartOpacity = chart.GetCurrentProperty<float>(Dali::Actor::Property::OPACITY);
    }
    if(auto label = Label::DownCast(actor))
    {
      ++state.labels;
      const auto  reveal   = label.GetTextReveal();
      const float progress = label.GetTextRevealProgress();
      state.minimum        = std::min(state.minimum, progress);
      state.maximum        = std::max(state.maximum, progress);
      if(reveal == Text::Reveal::None() || reveal.GetUnit() != Text::Reveal::Unit::PIXEL ||
         reveal.GetFadeDurationRatio() != 0.f || reveal.GetSequence() != Text::Reveal::Sequence::PER_LINE ||
         reveal.GetSequenceStaggerRatio() < 0.25f || reveal.GetSequenceStaggerRatio() > 0.5f)
      {
        ++state.invalid;
      }
      if(label.GetTextGradientOverlay().GetType() != Gradient::Type::NONE)
      {
        ++state.shimmers;
        state.invalid += label.GetTextGradientOverlayMode() != Text::GradientOverlayMode::SCREEN;
      }
      return;
    }
    for(unsigned i = 0; i < actor.GetChildCount(); ++i)
    {
      Inspect(actor.GetChildAt(i), state);
    }
  }

  bool Sample()
  {
    for(unsigned i = 0; i < mWindows.size(); ++i)
    {
      Snapshot state;
      Inspect(mWindows[i].GetRootLayer(), state);
      std::printf("MOTION %s %u %u %u %u %.3f %.3f %.3f\n", std::setlocale(LC_MESSAGES, nullptr), i, state.labels,
                  state.invalid, state.shimmers, state.minimum, state.maximum, state.chartOpacity);
    }
    std::fflush(stdout);
    return true;
  }

  std::array<Dali::Window, 3> mWindows;
  Dali::Timer                 mTimer;
};
}
