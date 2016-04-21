#
{
  'targets': [
    {
      'target_name': 'InstallWizard',
      'product_name': 'nsInstallWizard',
      'type': 'shared_library',
      'defines': [
        'NSISCALL=__stdcall',
        '_USRDLL',
        'DUILIB_STATIC',
      ],
      'include_dirs': [
        '..',
      ],
      'sources': [
        'main.cc',
        'install_wizard_api.cc',
        #'install_wizard_api.h',
        'install_wizard.cc',
        'install_wizard.h',
      ],
      'dependencies': [
        '<(DEPTH)/srdtk/srdtk.gyp:srdtk',
        '<(DEPTH)/third_party/DuiLib/duilib.gyp:duilib',
        '<(DEPTH)/third_party/nsis/pluginapi.gyp:pluginapi',
        '<(DEPTH)/third_party/zlib/zlib.gyp:*',
        '<(DEPTH)/third_party/zlib/google/zip.gyp:zip',
      ],
      'msvs_settings': {
        'VCLinkerTool': {
          'AdditionalLibraryDirectories': [
            '<(DEPTH)/third_party/nsis',
          ],
          #'IgnoreDefaultLibraryNames': [
          #  'libcmtd.lib', 
          #  'libcmt.lib', 
          #  'msvcrt.lib', 
          #  'msvcrtd.lib'
          #],
        },
      },
      'link_settings': {
        'libraries': [
          '-lcomctl32.lib',
          #'-lpluginapi.lib',
        ],
      },
      'msvs_configuration_attributes': {
        'CharacterSet': '1',
      },
    },
  ],
}