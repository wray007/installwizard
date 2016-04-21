#
{
  'target_defaults': {
    'include_dirs': [
      '.',
    ],
    'defines': [ 
      'BOOST_ALL_NO_LIB=1', 
    ],
    'direct_dependent_settings': {
      'defines': [
        'BOOST_ALL_NO_LIB=1', 
      ],
    },
  },
  'targets': [
    {
      'target_name': 'boost_system',
      'type': 'static_library',
      'sources': [
        'libs/system/error_code.cpp',
        'libs/system/local_free_on_destruction.hpp',
      ],
      'defines': [
        'BOOST_SYSTEM_STATIC_LINK=1',
      ],
      'direct_dependent_settings': {
        'defines': [
          'BOOST_SYSTEM_STATIC_LINK=1',
        ],
      },
    }, #target boost_system
    {
      'target_name': 'boost_file_system',
      'type': 'static_library',
      'sources': [
        'libs/filesystem/codecvt_error_category.cpp',
        'libs/filesystem/operations.cpp',
        'libs/filesystem/path.cpp',
        'libs/filesystem/path_traits.cpp',
        'libs/filesystem/portability.cpp',
        'libs/filesystem/unique_path.cpp',
        'libs/filesystem/utf8_codecvt_facet.cpp',
        'libs/filesystem/windows_file_codecvt.cpp',
        'libs/filesystem/windows_file_codecvt.hpp',
      ],
      'defines': [
        'BOOST_FILESYSTEM_STATIC_LINK=1',
      ],
      'direct_dependent_settings': {
        'defines': [
          'BOOST_FILESYSTEM_STATIC_LINK=1',
        ],
      },
      'dependencies': [
        'boost_system',
      ],
    }, #target boost_file_system
  ],
}