// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_IMPELLER_AIKS_EXPERIMENTAL_CANVAS_H_
#define FLUTTER_IMPELLER_AIKS_EXPERIMENTAL_CANVAS_H_

#include <memory>
#include <optional>
#include <vector>

#include "impeller/aiks/canvas.h"
#include "impeller/aiks/image_filter.h"
#include "impeller/aiks/paint.h"
#include "impeller/entity/entity.h"
#include "impeller/entity/entity_pass.h"
#include "impeller/entity/entity_pass_clip_stack.h"

namespace impeller {

struct LazyRenderingConfig {
  std::unique_ptr<EntityPassTarget> entity_pass_target;
  std::unique_ptr<InlinePassContext> inline_pass_context;

  LazyRenderingConfig(ContentContext& renderer,
                      std::unique_ptr<EntityPassTarget> p_entity_pass_target)
      : entity_pass_target(std::move(p_entity_pass_target)) {
    inline_pass_context =
        std::make_unique<InlinePassContext>(renderer, *entity_pass_target, 0);
  }
};

/// This Canvas attempts to translate from display lists to draw calls directly.
///
/// It's not fully implemented yet but if successful it will be replacing the
/// aiks Canvas functionality.
///
/// See also:
///   - go/impeller-canvas-efficiency
class ExperimentalCanvas : public Canvas {
 public:
  ExperimentalCanvas(ContentContext& renderer,
                     RenderTarget& render_target,
                     bool requires_readback);

  ExperimentalCanvas(ContentContext& renderer,
                     RenderTarget& render_target,
                     bool requires_readback,
                     Rect cull_rect);

  ExperimentalCanvas(ContentContext& renderer,
                     RenderTarget& render_target,
                     bool requires_readback,
                     IRect cull_rect);

  ~ExperimentalCanvas() override = default;

  void Save(uint32_t total_content_depth) override;

  void SaveLayer(const Paint& paint,
                 std::optional<Rect> bounds,
                 const std::shared_ptr<ImageFilter>& backdrop_filter,
                 ContentBoundsPromise bounds_promise,
                 uint32_t total_content_depth,
                 bool can_distribute_opacity) override;

  bool Restore() override;

  void EndReplay();

  void DrawTextFrame(const std::shared_ptr<TextFrame>& text_frame,
                     Point position,
                     const Paint& paint) override;

  struct SaveLayerState {
    Paint paint;
    Rect coverage;
  };

 private:
  // clip depth of the previous save or 0.
  size_t GetClipHeightFloor() const {
    if (transform_stack_.size() > 1) {
      return transform_stack_[transform_stack_.size() - 2].clip_height;
    }
    return 0;
  }

  ContentContext& renderer_;
  RenderTarget& render_target_;
  const bool requires_readback_;
  EntityPassClipStack clip_coverage_stack_;
  std::vector<LazyRenderingConfig> render_passes_;
  std::vector<SaveLayerState> save_layer_state_;

  void SetupRenderPass();

  void AddRenderEntityToCurrentPass(Entity entity, bool reuse_depth) override;
  void AddClipEntityToCurrentPass(Entity entity) override;
  bool BlitToOnscreen();

  Point GetGlobalPassPosition() {
    if (save_layer_state_.empty()) {
      return Point(0, 0);
    }
    return save_layer_state_.back().coverage.GetOrigin();
  }

  ExperimentalCanvas(const ExperimentalCanvas&) = delete;

  ExperimentalCanvas& operator=(const ExperimentalCanvas&) = delete;
};

}  // namespace impeller

#endif  // FLUTTER_IMPELLER_AIKS_EXPERIMENTAL_CANVAS_H_
