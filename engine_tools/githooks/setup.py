#!/usr/bin/env python3
# Copyright 2013 The Flutter Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

'''
Sets up githooks.
'''

import os
import subprocess
import sys

SRC_ROOT = os.path.dirname(
    os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


def IsWindows():
  os_id = sys.platform
  return os_id.startswith('win32') or os_id.startswith('cygwin')


def Main(argv):
  git = 'git'
  githooks = os.path.join(SRC_ROOT, 'engine_tools', 'githooks')
  if IsWindows():
    git = 'git.bat'
  result = subprocess.run([
      git,
      'config',
      'core.hooksPath',
      githooks,
  ], cwd=SRC_ROOT)
  return result.returncode


if __name__ == '__main__':
  sys.exit(Main(sys.argv))
