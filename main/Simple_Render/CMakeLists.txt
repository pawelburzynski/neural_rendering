include_directories("${PROJECT_SOURCE_DIR}/include" ${OpenCL_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})

QT5_WRAP_CPP(lf_renderer_HEADERS_MOC LFViewWidget.h LFViewWindow.h)

add_executable(simple_render main.cpp LFViewWindow.cpp LFViewWidget.cpp LightFieldRenderer.cpp ${lf_renderer_HEADERS_MOC})
target_compile_features(simple_render PRIVATE cxx_range_for)
target_link_libraries(simple_render ${OpenCL_LIBRARIES} Qt5::Widgets) #Qt5::OpenGL
