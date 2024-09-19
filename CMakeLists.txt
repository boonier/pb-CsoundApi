# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(BidulePlugin)

message("CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")

# Set the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED OFF)

# Define the plugin bundle name
set(PLUGIN_NAME "CsoundTest")

# Define the Bidule SDK directory
set(BIDULE_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../common")

# Set the include directories
include_directories("/Library/Frameworks/CsoundLib64.framework/Versions/6.0/Headers")
include_directories("/Users/boonier/GIT/nativefiledialog-extended/src/include")
message("Include directories: ${INCLUDE_DIRECTORIES}")


# Include the Bidule SDK headers
include_directories("${BIDULE_SDK_DIR}")

# Add the Bidule SDK source file to the plugin
add_library(${PLUGIN_NAME} MODULE
  CsoundTest.cpp
  ${BIDULE_SDK_DIR}/BiduleSDK.cpp
)

# Copy the InfoPlist.strings file to the plugin bundle -- is require for Bidule?
add_custom_command(TARGET ${PLUGIN_NAME} PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory
  ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_NAME}.bundle/Contents/Resources/en.lproj/
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_CURRENT_SOURCE_DIR}/en.lproj/InfoPlist.strings
  ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_NAME}.bundle/Contents/Resources/en.lproj/
  COMMENT "Copying InfoPlist.strings to bundle"
)

# Add the Csound library to the plugin
target_link_libraries(${PLUGIN_NAME} PRIVATE "-framework CsoundLib64 -F/Library/Frameworks")
target_link_libraries(${PLUGIN_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libnfd.a)
target_link_libraries(${PLUGIN_NAME} PRIVATE "-framework AppKit -framework UniformTypeIdentifiers -framework Cocoa -framework CoreFoundation")


# target_include_directories(${PLUGIN_NAME} PUBLIC
#   "${BIDULE_SDK_DIR}"
#   "/Library/Frameworks/CsoundLib64.framework/Versions/6.0/Headers"
#   "/Users/boonier/GIT/nativefiledialog-extended/src/include"
# )


# Set the plugin bundle output directory
set_target_properties(${PLUGIN_NAME} PROPERTIES
  BUNDLE ON
  BUNDLE_EXTENSION "bundle"
  BUNDLE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
  RESOURCE "Resources/en.lproj/InfoPlist.strings"
  CODE_SIGN_IDENTITY "iPhone Developer"
)