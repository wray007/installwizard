#
{
  'targets': [
    {
      'target_name': 'srdtk',
      'type': 'static_library',
      'include_dirs': [
        '.',
        '..',
      ],
      'sources': [
        'object/auto_array.h',
        'object/smart_ptr.h',
        'object/uuid.h',
        'pattern/observer.h',
        #'pattern/SingleInstance.h',
        #'pattern/win32/AsyncObserverProxyWindowImpl.h',
        #'pattern/win32/MySubclassWnd.h',
        #'pattern/win32/Subclass.cpp',
        #'pattern/win32/Subclass.h',
        'string/string_util.cpp',
        'string/string_util.h',
        'string/string_macros.h',
      ],
      'conditions': [
        ['OS!="win"', {
          'source!': [
            'pattern/SingleInstance.h',
            'pattern/win32/AsyncObserverProxyWindowImpl.h',
            'pattern/win32/MySubclassWnd.h',
            'pattern/win32/Subclass.cpp',
            'pattern/win32/Subclass.h',
          ],
        }],
      ],
    },
  ],
}