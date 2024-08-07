// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "shell/platform/embedder/tests/embedder_test_context_software.h"

#include <utility>

#include "fml/make_copyable.h"
#include "fml/paths.h"
#include "runtime/dart_vm.h"
#include "shell/platform/embedder/tests/embedder_assertions.h"
#include "shell/platform/embedder/tests/embedder_test_compositor_software.h"
#include "testing/testing.h"
#include "third_party/dart/runtime/bin/elf_loader.h"
#include "third_party/skia/include/core/SkSurface.h"

namespace flutter {
namespace testing {

EmbedderTestContextSoftware::EmbedderTestContextSoftware(
    std::string assets_path)
    : EmbedderTestContext(std::move(assets_path)) {}

EmbedderTestContextSoftware::~EmbedderTestContextSoftware() = default;

bool EmbedderTestContextSoftware::Present(const sk_sp<SkImage>& image) {
  software_surface_present_count_++;

  FireRootSurfacePresentCallbackIfPresent([image] { return image; });

  return true;
}

size_t EmbedderTestContextSoftware::GetSurfacePresentCount() const {
  return software_surface_present_count_;
}

EmbedderTestContextType EmbedderTestContextSoftware::GetContextType() const {
  return EmbedderTestContextType::kSoftwareContext;
}

void EmbedderTestContextSoftware::SetupSurface(SkISize surface_size) {
  surface_size_ = surface_size;
}

void EmbedderTestContextSoftware::SetupCompositor() {
  FML_CHECK(!compositor_) << "Already set up a compositor in this context.";
  compositor_ = std::make_unique<EmbedderTestCompositorSoftware>(surface_size_);
}

}  // namespace testing
}  // namespace flutter
