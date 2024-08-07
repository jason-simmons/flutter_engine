// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "fml/backtrace.h"
#include "fml/build_config.h"
#include "fml/command_line.h"
#include "fml/logging.h"
#include "gtest/gtest.h"

#include "flow_test_utils.h"

int main(int argc, char** argv) {
  fml::InstallCrashHandler();
  testing::InitGoogleTest(&argc, argv);
  fml::CommandLine cmd = fml::CommandLineFromPlatformOrArgcArgv(argc, argv);

#if defined(OS_FUCHSIA)
  flutter::SetGoldenDir(cmd.GetOptionValueWithDefault(
      "golden-dir", "/pkg/data/flutter/testing/resources"));
#else
  flutter::SetGoldenDir(
      cmd.GetOptionValueWithDefault("golden-dir", "flutter/testing/resources"));
#endif
  flutter::SetFontFile(cmd.GetOptionValueWithDefault(
      "font-file",
      "flutter/third_party/txt/third_party/fonts/Roboto-Regular.ttf"));
  return RUN_ALL_TESTS();
}
