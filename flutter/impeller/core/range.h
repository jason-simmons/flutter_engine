// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_IMPELLER_CORE_RANGE_H_
#define FLUTTER_IMPELLER_CORE_RANGE_H_

#include <cstddef>

namespace impeller {

struct Range {
  size_t offset = 0;
  size_t length = 0;

  constexpr Range() {}

  constexpr Range(size_t p_offset, size_t p_length)
      : offset(p_offset), length(p_length) {}

  constexpr bool operator==(const Range& o) const {
    return offset == o.offset && length == o.length;
  }
};

}  // namespace impeller

#endif  // FLUTTER_IMPELLER_CORE_RANGE_H_
