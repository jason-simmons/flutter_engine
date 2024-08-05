// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_IMPELLER_SCENE_ANIMATION_ANIMATION_PLAYER_H_
#define FLUTTER_IMPELLER_SCENE_ANIMATION_ANIMATION_PLAYER_H_

#include <map>
#include <memory>
#include <optional>

#include "impeller/base/timing.h"
#include "impeller/scene/animation/animation_clip.h"

namespace impeller {
namespace scene {

class Node;

class AnimationPlayer final {
 public:
  AnimationPlayer();
  ~AnimationPlayer();

  AnimationPlayer(AnimationPlayer&&);
  AnimationPlayer& operator=(AnimationPlayer&&);

  AnimationClip* AddAnimation(const std::shared_ptr<Animation>& animation,
                              Node* bind_target);

  AnimationClip* GetClip(const std::string& name) const;

  /// @brief  Advanced all clips and updates animated properties in the scene.
  void Update();

 private:
  std::unordered_map<Node*, AnimationTransforms> target_transforms_;

  std::map<std::string, AnimationClip> clips_;

  std::optional<TimePoint> previous_time_;

  AnimationPlayer(const AnimationPlayer&) = delete;

  AnimationPlayer& operator=(const AnimationPlayer&) = delete;
};

}  // namespace scene
}  // namespace impeller

#endif  // FLUTTER_IMPELLER_SCENE_ANIMATION_ANIMATION_PLAYER_H_
