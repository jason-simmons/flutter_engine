// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_IMPELLER_SCENE_SCENE_H_
#define FLUTTER_IMPELLER_SCENE_SCENE_H_

#include <memory>

#include "impeller/renderer/render_target.h"
#include "impeller/scene/camera.h"
#include "impeller/scene/node.h"
#include "impeller/scene/scene_context.h"

namespace impeller {
namespace scene {

class Scene {
 public:
  Scene() = delete;

  explicit Scene(std::shared_ptr<SceneContext> scene_context);

  ~Scene();

  Node& GetRoot();

  bool Render(const RenderTarget& render_target,
              const Matrix& camera_transform);

  bool Render(const RenderTarget& render_target, const Camera& camera);

 private:
  std::shared_ptr<SceneContext> scene_context_;
  Node root_;

  Scene(const Scene&) = delete;

  Scene& operator=(const Scene&) = delete;
};

}  // namespace scene
}  // namespace impeller

#endif  // FLUTTER_IMPELLER_SCENE_SCENE_H_
