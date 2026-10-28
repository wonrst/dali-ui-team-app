// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#include "scene-motion.h"
#include <dali-ui-foundation/public-api/animation/label-animation-bridge.autogen.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/layouts/layout-bounds-effects.h>
#include <dali-ui-foundation/public-api/text/style/reveal.h>
#include <dali/public-api/animation/key-frames.h>
#include <algorithm>

namespace Buxby
{
namespace
{
constexpr float REVEAL_SECONDS  = 1.35f;
constexpr float SHIMMER_SECONDS = 2.2f;
constexpr float SHIMMER_DELAY   = REVEAL_SECONDS;

void StopAnimation(Animation& animation)
{
  if(animation)
  {
    animation.Stop();
    animation.Reset();
  }
}
}

LayoutTransition MakeItemTransition(bool animateChanges)
{
  auto transition = LayoutTransition::New();
  transition.SetEnterOnInitialMount(true);
  transition.SetReflowScope(LayoutReflowScope::SUBTREE);
  transition.SetEnterBoundsEffect(
    LayoutBoundsEffects::SlideFrom(LayoutBoundsEdge::BOTTOM, LayoutBoundsLength::Pixel(14.f),
                                   {Duration(0.65f), AlphaFunction::EASE_OUT_SQUARE, Duration(0.08f)}));
  if(animateChanges)
  {
    transition.SetChangeTiming({Duration(0.42f), AlphaFunction::EASE_OUT_SQUARE, Duration()});
  }
  else
  {
    transition.ClearChangeTiming();
  }
  return transition;
}

void ConfigureShimmer(Label label)
{
  Gradient::Linear overlay(Vector2(-0.5f, 0.f), Vector2(0.5f, 0.f));
  overlay.SetSpreadMethod(Gradient::SpreadMethod::PAD);
  overlay.SetStartOffset(-1.f);
  overlay.SetStopNodes({{0.f, UiColor(0xFFFFFF, 0.f)},
                        {0.10f, UiColor(0xFFFFFF, 0.f)},
                        {0.28f, UiColor(0xFFFFFF, 0.8f)},
                        {0.36f, UiColor(0xFFFFFF)},
                        {0.68f, UiColor(0xFFFFFF)},
                        {0.90f, UiColor(0xFFFFFF, 0.f)},
                        {1.f, UiColor(0xFFFFFF, 0.f)}});
  label.SetTextGradientOverlayBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
  label.SetTextGradientOverlayMode(Text::GradientOverlayMode::SCREEN);
  label.SetTextGradientOverlay(overlay);
}

SceneMotion::~SceneMotion()
{
  Stop();
}

void SceneMotion::RevealText(Actor subtree)
{
  if(auto label = Label::DownCast(subtree))
  {
    Replay(label);
    return;
  }
  for(unsigned i = 0; i < subtree.GetChildCount(); ++i)
  {
    RevealText(subtree.GetChildAt(i));
  }
}

void SceneMotion::Replay(Label label)
{
  auto track = std::find_if(mTextTracks.begin(), mTextTracks.end(), [label](const TextTrack& item)
  {
    return item.label == label;
  });
  if(track == mTextTracks.end())
  {
    mTextTracks.push_back({label, {}});
    track = mTextTracks.end() - 1;
  }
  StopAnimation(track->animation);

  Text::Reveal reveal;
  reveal.SetUnit(Text::Reveal::Unit::PIXEL);
  reveal.SetFadeDurationRatio(0.f);
  // A single line is one sequence; translated/wrapped text automatically gains overlapping lines.
  reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
  reveal.SetSequenceStaggerRatio(0.35f);
  label.SetTextReveal(reveal);
  label.SetTextRevealProgress(0.f);

  auto& animation = track->animation;
  animation       = Animation::New(REVEAL_SECONDS);
  animation.SetEndAction(Animation::BAKE_FINAL);
  label.Animate(animation).TextRevealProgress(1.f, Duration(REVEAL_SECONDS), AlphaFunction::EASE_OUT_SQUARE);
  if(label.GetTextGradientOverlay().GetType() != Gradient::Type::NONE)
  {
    ConfigureShimmer(label);
    label.Animate(animation).TextGradientOverlayStartOffset(1.f, Duration(SHIMMER_SECONDS),
                                                            AlphaFunction::EASE_IN_OUT_SINE, Duration(SHIMMER_DELAY));
  }
  animation.Play();
}

void SceneMotion::StartChartPulse(View chart)
{
  DisconnectAll();
  StopAnimation(mPulse);
  // Animate the chart as an item; its own renderer owns the canvas/tick geometry below it.
  auto contents = LayoutTransition::New();
  contents.ClearChangeTiming();
  chart.SetLayoutTransition(contents);
  // ChartView creates/replaces its tick Labels after layout. Follow the public actor lifecycle.
  chart.ChildAddedSignal().Connect(this, &SceneMotion::OnChartChildAdded);
  chart.ChildRemovedSignal().Connect(this, &SceneMotion::OnChartChildRemoved);
  auto brightness = KeyFrames::New();
  brightness.Add(0.f, 1.f);
  brightness.Add(0.25f, 0.90f);
  brightness.Add(0.5f, 0.78f);
  brightness.Add(0.75f, 0.90f);
  brightness.Add(1.f, 1.f);
  mPulse = Animation::New(3.2f);
  mPulse.SetLoopCount(Animation::INFINITE_LOOP);
  mPulse.SetEndAction(Animation::BAKE_FINAL);
  // Pulse the rendered chart without changing its data, bounds or text layout.
  mPulse.AnimateBetween(Property(chart, Actor::Property::OPACITY), brightness, Animation::CUBIC);
  mPulse.Play();
}

void SceneMotion::OnChartChildAdded(Actor, Actor child)
{
  if(auto label = Label::DownCast(child))
  {
    Replay(label);
  }
}

void SceneMotion::OnChartChildRemoved(Actor, Actor child)
{
  auto track = std::find_if(mTextTracks.begin(), mTextTracks.end(), [child](const TextTrack& item)
  {
    return item.label == child;
  });
  if(track != mTextTracks.end())
  {
    StopAnimation(track->animation);
    mTextTracks.erase(track);
  }
}

void SceneMotion::Stop()
{
  DisconnectAll();
  for(auto& track : mTextTracks)
  {
    StopAnimation(track.animation);
  }
  mTextTracks.clear();
  StopAnimation(mPulse);
}
}
