# Copyright 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'targets': [
    {
      'target_name': 'zip',
      'type': 'static_library',
      'dependencies': [
        '../zlib.gyp:minizip',
      ],
      'include_dirs': [
        '../../..',
        '<(DEPTH)/third_party/boost',
      ],
      'sources': [
        'zip.cc',
        'zip.h',
        'zip_internal.cc',
        'zip_internal.h',
        'zip_reader.cc',
        'zip_reader.h',
      ],
      'defines': [
        'NOMINMAX',
      ],
      'dependencies': [
        '<(DEPTH)/third_party/boost/boost.gyp:boost_file_system',
        '<(DEPTH)/third_party/boost/boost.gyp:boost_system',
      ],
      'conditions': [
        ['OS=="win"', {
          'defines': [
            'OS_WIN=1'
          ],
        }],
      ],
    },
  ],
}
