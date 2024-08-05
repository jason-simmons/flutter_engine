// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_IMPELLER_RENDERER_BACKEND_GLES_PIPELINE_LIBRARY_GLES_H_
#define FLUTTER_IMPELLER_RENDERER_BACKEND_GLES_PIPELINE_LIBRARY_GLES_H_

#include "impeller/renderer/backend/gles/reactor_gles.h"
#include "impeller/renderer/pipeline_library.h"

namespace impeller {

class ContextGLES;

class PipelineLibraryGLES final : public PipelineLibrary {
 public:
  // |PipelineLibrary|
  ~PipelineLibraryGLES() override;

 private:
  friend ContextGLES;

  ReactorGLES::Ref reactor_;
  PipelineMap pipelines_;

  explicit PipelineLibraryGLES(ReactorGLES::Ref reactor);

  // |PipelineLibrary|
  bool IsValid() const override;

  // |PipelineLibrary|
  PipelineFuture<PipelineDescriptor> GetPipeline(PipelineDescriptor descriptor,
                                                 bool async) override;

  // |PipelineLibrary|
  PipelineFuture<ComputePipelineDescriptor> GetPipeline(
      ComputePipelineDescriptor descriptor,
      bool async) override;

  // |PipelineLibrary|
  void RemovePipelinesWithEntryPoint(
      std::shared_ptr<const ShaderFunction> function) override;

  PipelineLibraryGLES(const PipelineLibraryGLES&) = delete;

  PipelineLibraryGLES& operator=(const PipelineLibraryGLES&) = delete;
};

}  // namespace impeller

#endif  // FLUTTER_IMPELLER_RENDERER_BACKEND_GLES_PIPELINE_LIBRARY_GLES_H_
