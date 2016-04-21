
::tools\gyp\gyp.bat --no-circular-check --depth . -Ibuild/common.gypi build/All.gyp
tools\gyp\gyp.bat --no-circular-check --depth . -Dcomponent=shared_library -Ibuild/common.gypi build/All.gyp