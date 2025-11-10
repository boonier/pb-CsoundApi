# CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(BidulePlugin)
message("CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")

# Set the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED OFF)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_INSTALL_PREFIX "/Users/boonier/Music/Plogue/Bidule/plugins")

# Define the plugin bundle name
set(PLUGIN_NAME "CsoundApi")

# Define the Bidule SDK directory
set(BIDULE_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../common")

# Add the Bidule SDK source file to the plugin
add_library(${PLUGIN_NAME} MODULE
  CsoundApi.cpp
  ${BIDULE_SDK_DIR}/BiduleSDK.cpp
)

target_sources(${PLUGIN_NAME} PRIVATE CsoundApiMain.cpp)

# Include the Bidule SDK headers
include_directories("${BIDULE_SDK_DIR}")
# Set the include directories on the target
target_include_directories(CsoundApi PRIVATE
    "/Applications/Csound/CsoundLib64.framework/Headers"
    "/Users/boonier/GIT/nativefiledialog-extended/src/include"
)
# message("HERE1")
get_target_property(INCLUDE_DIRS CsoundApi INCLUDE_DIRECTORIES)
message("Include directories: ${INCLUDE_DIRS}")



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
# target_link_libraries(${PLUGIN_NAME} PUBLIC "-framework CsoundLib64 -F/Library/Frameworks")
target_link_libraries(${PLUGIN_NAME} PUBLIC "-framework CsoundLib64" "-F/Applications/Csound")
# find_library(Csound NAMES CsoundLib64 PATHS "/Applications/Csound" REQUIRED NO_DEFAULT_PATH)
target_link_libraries(${PLUGIN_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libnfd.a)
target_link_libraries(${PLUGIN_NAME} PUBLIC "-framework AppKit -framework UniformTypeIdentifiers -framework Cocoa -framework CoreFoundation")

# Set the plugin bundle output directory
message("Set the plugin bundle output directory...")
set_target_properties(${PLUGIN_NAME} PROPERTIES
  BUNDLE ON
  BUNDLE_EXTENSION "bundle"
  BUNDLE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
  # RESOURCE "Resources/en.lproj/InfoPlist.strings"
)

message("About to install plugin bundle...")
# Remove existing executable
install(CODE "
  if(EXISTS \"${CMAKE_INSTALL_PREFIX}/CsoundApi.bundle\")
    file(REMOVE_RECURSE \"${CMAKE_INSTALL_PREFIX}/CsoundApi.bundle\")
  endif()
")
install(TARGETS ${PLUGIN_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/")

# Add custom target to launch Bidule with test layout
add_custom_target(launch_test_layout
    COMMAND open -a "Bidule 0.9790 P64" "/Users/boonier/Desktop/BIDULE DEV TESTS/CsoundCsdTestCs7.bidule"
    COMMENT "Launching CsoundCsdTestCs7.bidule in Bidule"
)