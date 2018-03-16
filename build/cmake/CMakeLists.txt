# global settngs
cmake_minimum_required (VERSION 3.5.1)

project (OnSag)
set (CMAKE_CXX_STANDARD 11)
set (APPCOMMON_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external/AppCommon)
set (ONSAG_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set (MODELS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external/Models)
set (WXWIDGETS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external/wxWidgets)

# defines include directories
include_directories (${ONSAG_SOURCE_DIR}/include)
include_directories (${ONSAG_SOURCE_DIR}/res)
include_directories (${APPCOMMON_SOURCE_DIR}/include)
include_directories (${APPCOMMON_SOURCE_DIR}/res)
include_directories (${MODELS_SOURCE_DIR}/include)
include_directories (${WXWIDGETS_SOURCE_DIR}/include)

# defines common source files
set (APPCOMMON_SRC_FILES
  ${APPCOMMON_SOURCE_DIR}/src/graphics/data_set_2d.cc
  ${APPCOMMON_SOURCE_DIR}/src/graphics/line_data_set_2d.cc
  ${APPCOMMON_SOURCE_DIR}/src/graphics/line_renderer_2d.cc
  ${APPCOMMON_SOURCE_DIR}/src/graphics/plot_2d.cc
  ${APPCOMMON_SOURCE_DIR}/src/graphics/plot_axis.cc
  ${APPCOMMON_SOURCE_DIR}/src/graphics/plot_pane_2d.cc
  ${APPCOMMON_SOURCE_DIR}/src/graphics/renderer_2d.cc
  ${APPCOMMON_SOURCE_DIR}/src/graphics/text_data_set_2d.cc
  ${APPCOMMON_SOURCE_DIR}/src/graphics/text_renderer_2d.cc
  ${APPCOMMON_SOURCE_DIR}/src/widgets/error_message_dialog.cc
  ${APPCOMMON_SOURCE_DIR}/src/widgets/log_pane.cc
  ${APPCOMMON_SOURCE_DIR}/src/widgets/report_table.cc
  ${APPCOMMON_SOURCE_DIR}/src/widgets/status_bar_log.cc
  ${APPCOMMON_SOURCE_DIR}/src/widgets/timer.cc
  ${APPCOMMON_SOURCE_DIR}/src/xml/color_xml_handler.cc
  ${APPCOMMON_SOURCE_DIR}/src/xml/point_xml_handler.cc
  ${APPCOMMON_SOURCE_DIR}/src/xml/xml_handler.cc
)

# defines OnSag source files
set (ONSAG_SRC_FILES
  ${ONSAG_SOURCE_DIR}/res/resources.cc
  ${ONSAG_SOURCE_DIR}/src/analysis_controller.cc
  ${ONSAG_SOURCE_DIR}/src/edit_pane.cc
  ${ONSAG_SOURCE_DIR}/src/file_handler.cc
  ${ONSAG_SOURCE_DIR}/src/on_sag_app.cc
  ${ONSAG_SOURCE_DIR}/src/on_sag_config_xml_handler.cc
  ${ONSAG_SOURCE_DIR}/src/on_sag_doc.cc
  ${ONSAG_SOURCE_DIR}/src/on_sag_doc_commands.cc
  ${ONSAG_SOURCE_DIR}/src/on_sag_doc_xml_handler.cc
  ${ONSAG_SOURCE_DIR}/src/on_sag_frame.cc
  ${ONSAG_SOURCE_DIR}/src/on_sag_printout.cc
  ${ONSAG_SOURCE_DIR}/src/on_sag_view.cc
  ${ONSAG_SOURCE_DIR}/src/preferences_dialog.cc
  ${ONSAG_SOURCE_DIR}/src/profile_plot_options_dialog.cc
  ${ONSAG_SOURCE_DIR}/src/profile_plot_pane.cc
  ${ONSAG_SOURCE_DIR}/src/results_pane.cc
  ${ONSAG_SOURCE_DIR}/src/sag_cable.cc
  ${ONSAG_SOURCE_DIR}/src/sag_cable_unit_converter.cc
  ${ONSAG_SOURCE_DIR}/src/sag_cable_xml_handler.cc
  ${ONSAG_SOURCE_DIR}/src/sag_method.cc
  ${ONSAG_SOURCE_DIR}/src/sag_method_unit_converter.cc
  ${ONSAG_SOURCE_DIR}/src/sag_method_xml_handler.cc
  ${ONSAG_SOURCE_DIR}/src/sag_cable_unit_converter.cc
  ${ONSAG_SOURCE_DIR}/src/sag_span.cc
  ${ONSAG_SOURCE_DIR}/src/sag_span_editor_dialog.cc
  ${ONSAG_SOURCE_DIR}/src/sag_span_unit_converter.cc
  ${ONSAG_SOURCE_DIR}/src/sag_span_xml_handler.cc
  ${ONSAG_SOURCE_DIR}/src/sag_structure.cc
  ${ONSAG_SOURCE_DIR}/src/sag_structure_unit_converter.cc
  ${ONSAG_SOURCE_DIR}/src/sag_structure_xml_handler.cc
  ${ONSAG_SOURCE_DIR}/src/span_sagger.cc
)

# defines OnSag + AppCommon resource files
set (ONSAG_RESOURCE_FILES
  ${ONSAG_SOURCE_DIR}/external/AppCommon/res/xrc/error_message_dialog.xrc
  ${ONSAG_SOURCE_DIR}/res/xrc/edit_pane.xrc
  ${ONSAG_SOURCE_DIR}/res/xrc/on_sag_menubar.xrc
  ${ONSAG_SOURCE_DIR}/res/xrc/preferences_dialog.xrc
  ${ONSAG_SOURCE_DIR}/res/xrc/profile_plot_options_dialog.xrc
  ${ONSAG_SOURCE_DIR}/res/xrc/sag_span_editor_dialog.xrc
)

# adds Models libraries
add_subdirectory (${MODELS_SOURCE_DIR}/build/cmake ${CMAKE_CURRENT_BINARY_DIR}/Models)

# adds wxWidgets libraries
# using wx-config in git subdirectory
# - find_package() command won't locate wxWidgets as a subdirectory on unix
# - add_subdirectory() command has a build error
# - ExternalProject looks complicated

# gets wx-config directory
if (CMAKE_BUILD_TYPE MATCHES Debug)
  set (WXCONFIG_DIR ${WXWIDGETS_SOURCE_DIR}/build-debug)
elseif (CMAKE_BUILD_TYPE MATCHES Release)
  set (WXCONFIG_DIR ${WXWIDGETS_SOURCE_DIR}/build-release)
endif ()

# gets wxWidgets compiler flags
execute_process (COMMAND ${WXCONFIG_DIR}/wx-config --cxxflags
                 OUTPUT_VARIABLE WXWIDGETS_COMPILER_FLAGS)
separate_arguments(WXWIDGETS_COMPILER_FLAGS UNIX_COMMAND "${WXWIDGETS_COMPILER_FLAGS}")

# gets wxWidgets linker flags
execute_process (COMMAND ${WXCONFIG_DIR}/wx-config --libs all
                 OUTPUT_VARIABLE WXWIDGETS_LINKER_FLAGS)
separate_arguments(WXWIDGETS_LINKER_FLAGS UNIX_COMMAND "${WXWIDGETS_LINKER_FLAGS}")

# compiles resource files
execute_process (COMMAND ${WXCONFIG_DIR}/utils/wxrc/wxrc -v -c
                 -o ${ONSAG_SOURCE_DIR}/res/resources.cc
                 ${ONSAG_RESOURCE_FILES})

# defines executable
add_executable (OnSag ${APPCOMMON_SRC_FILES} ${ONSAG_SRC_FILES})
target_compile_options (OnSag PUBLIC ${WXWIDGETS_COMPILER_FLAGS})
target_link_libraries (OnSag LINK_PUBLIC
  otlsmodels_sagging
  otlsmodels_transmissionline
  otlsmodels_base
  ${WXWIDGETS_LINKER_FLAGS})

# prints out all variables for debugging
get_cmake_property(_variableNames VARIABLES)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()
