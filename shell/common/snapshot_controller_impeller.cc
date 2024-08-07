// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "shell/common/snapshot_controller_impeller.h"

#include <algorithm>

#include "flow/surface.h"
#include "fml/trace_event.h"
#include "impeller/display_list/dl_dispatcher.h"
#include "impeller/display_list/dl_image_impeller.h"
#include "impeller/geometry/size.h"
#include "shell/common/snapshot_controller.h"
#include "impeller/renderer/render_target.h"

namespace flutter {

namespace {
sk_sp<DlImage> DoMakeRasterSnapshot(
    const sk_sp<DisplayList>& display_list,
    SkISize size,
    const std::shared_ptr<impeller::AiksContext>& context) {
  TRACE_EVENT0("flutter", __FUNCTION__);
  if (!context) {
    return nullptr;
  }
  // Determine render target size.
  auto max_size = context->GetContext()
                      ->GetResourceAllocator()
                      ->GetMaxTextureSizeSupported();
  double scale_factor_x =
      static_cast<double>(max_size.width) / static_cast<double>(size.width());
  double scale_factor_y =
      static_cast<double>(max_size.height) / static_cast<double>(size.height());
  double scale_factor = std::min({1.0, scale_factor_x, scale_factor_y});

  auto render_target_size = impeller::ISize(size.width(), size.height());

  // Scale down the render target size to the max supported by the
  // GPU if necessary. Exceeding the max would otherwise cause a
  // null result.
  if (scale_factor < 1.0) {
    render_target_size.width *= scale_factor;
    render_target_size.height *= scale_factor;
  }

#if EXPERIMENTAL_CANVAS
  // Do not use the render target cache as the lifecycle of this texture
  // will outlive a particular frame.
  impeller::ISize impeller_size = impeller::ISize(size.width(), size.height());
  impeller::RenderTargetAllocator render_target_allocator =
      impeller::RenderTargetAllocator(
          context->GetContext()->GetResourceAllocator());
  impeller::RenderTarget target;
  if (context->GetContext()->GetCapabilities()->SupportsOffscreenMSAA()) {
    target = render_target_allocator.CreateOffscreenMSAA(
        *context->GetContext(),  // context
        impeller_size,           // size
        /*mip_count=*/1,
        "Picture Snapshot MSAA",  // label
        impeller::RenderTarget::
            kDefaultColorAttachmentConfigMSAA  // color_attachment_config
    );
  } else {
    target = render_target_allocator.CreateOffscreen(
        *context->GetContext(),  // context
        impeller_size,           // size
        /*mip_count=*/1,
        "Picture Snapshot",  // label
        impeller::RenderTarget::
            kDefaultColorAttachmentConfig  // color_attachment_config
    );
  }

  impeller::TextFrameDispatcher collector(context->GetContentContext(),
                                          impeller::Matrix());
  display_list->Dispatch(collector, SkIRect::MakeSize(size));
  impeller::ExperimentalDlDispatcher impeller_dispatcher(
      context->GetContentContext(), target,
      display_list->root_has_backdrop_filter(),
      display_list->max_root_blend_mode(),
      impeller::IRect::MakeSize(impeller_size));
  display_list->Dispatch(impeller_dispatcher, SkIRect::MakeSize(size));
  impeller_dispatcher.FinishRecording();

  context->GetContentContext().GetLazyGlyphAtlas()->ResetTextFrames();

  return impeller::DlImageImpeller::Make(target.GetRenderTargetTexture(),
                                         DlImage::OwningContext::kRaster);
#else
  impeller::DlDispatcher dispatcher;
  display_list->Dispatch(dispatcher);
  impeller::Picture picture = dispatcher.EndRecordingAsPicture();

  std::shared_ptr<impeller::Image> image =
      picture.ToImage(*context, render_target_size);
  if (image) {
    return impeller::DlImageImpeller::Make(image->GetTexture(),
                                           DlImage::OwningContext::kRaster);
  }
#endif  // EXPERIMENTAL_CANVAS

  return nullptr;
}

sk_sp<DlImage> DoMakeRasterSnapshot(
    sk_sp<DisplayList> display_list,
    SkISize picture_size,
    const std::shared_ptr<const fml::SyncSwitch>& sync_switch,
    const std::shared_ptr<impeller::AiksContext>& context) {
  sk_sp<DlImage> result;
  sync_switch->Execute(fml::SyncSwitch::Handlers()
                           .SetIfTrue([&] {
                             // Do nothing.
                           })
                           .SetIfFalse([&] {
                             result = DoMakeRasterSnapshot(
                                 display_list, picture_size, context);
                           }));

  return result;
}
}  // namespace

void SnapshotControllerImpeller::MakeRasterSnapshot(
    sk_sp<DisplayList> display_list,
    SkISize picture_size,
    std::function<void(const sk_sp<DlImage>&)> callback) {
  sk_sp<DlImage> result;
  std::shared_ptr<const fml::SyncSwitch> sync_switch =
      GetDelegate().GetIsGpuDisabledSyncSwitch();
  sync_switch->Execute(
      fml::SyncSwitch::Handlers()
          .SetIfTrue([&] {
            std::shared_ptr<impeller::AiksContext> context =
                GetDelegate().GetAiksContext();
            if (context) {
              context->GetContext()->StoreTaskForGPU(
                  [context, sync_switch, display_list = std::move(display_list),
                   picture_size, callback = std::move(callback)] {
                    callback(DoMakeRasterSnapshot(display_list, picture_size,
                                                  sync_switch, context));
                  });
            } else {
              callback(nullptr);
            }
          })
          .SetIfFalse([&] {
            callback(DoMakeRasterSnapshot(display_list, picture_size,
                                          GetDelegate().GetAiksContext()));
          }));
}

sk_sp<DlImage> SnapshotControllerImpeller::MakeRasterSnapshotSync(
    sk_sp<DisplayList> display_list,
    SkISize picture_size) {
  return DoMakeRasterSnapshot(display_list, picture_size,
                              GetDelegate().GetIsGpuDisabledSyncSwitch(),
                              GetDelegate().GetAiksContext());
}

void SnapshotControllerImpeller::CacheRuntimeStage(
    const std::shared_ptr<impeller::RuntimeStage>& runtime_stage) {
  impeller::RuntimeEffectContents runtime_effect;
  runtime_effect.SetRuntimeStage(runtime_stage);
  auto context = GetDelegate().GetAiksContext();
  if (!context) {
    return;
  }
  runtime_effect.BootstrapShader(context->GetContentContext());
}

sk_sp<SkImage> SnapshotControllerImpeller::ConvertToRasterImage(
    sk_sp<SkImage> image) {
  FML_UNREACHABLE();
}

}  // namespace flutter
