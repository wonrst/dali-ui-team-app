// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#pragma once

#include "style.h"
#include <dali-ui-foundation/public-api/layouts/layout-transition.h>
#include <vector>

namespace Buxby
{
LayoutTransition MakeItemTransition(bool animateChanges = false);
void             ConfigureShimmer(Label label);

// Each scene owns its animations; Team members only handle their normal lifecycle.
class SceneMotion : public ConnectionTracker
{
public:
  SceneMotion() = default;
  ~SceneMotion() override;
  SceneMotion(const SceneMotion&)            = delete;
  SceneMotion& operator=(const SceneMotion&) = delete;

  void RevealText(Actor subtree);
  void StartChartPulse(View chart);
  void Stop();

private:
  struct TextTrack
  {
    Label     label;
    Animation animation;
  };

  void Replay(Label label);
  void OnChartChildAdded(Actor parent, Actor child);
  void OnChartChildRemoved(Actor parent, Actor child);

  std::vector<TextTrack> mTextTracks;
  Animation              mPulse;
};
}
