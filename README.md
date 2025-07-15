# pb-CsoundApi

This is an sdk plugin for Plogue Bidule.

## Requirements

Csound needs to be installed on your system, as the library contains headers for the api, which is required for building.

The plugin uses CMake to build, so less of the Xcode non-fun to worry about.

`cmake --build . --target install` will install the plugin to `CMAKE_INSTALL_PREFIX`
