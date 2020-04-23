include_directories("${PROJECT_SOURCE_DIR}/include" ${OpenCL_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})

QT5_WRAP_CPP(lf_renderer_HEADERS_MOC ViewWidget.h ViewWindow.h)

add_executable(simple_renderer main.cpp ViewWindow.cpp ViewWidget.cpp ProjectionRenderer.cpp SimpleRenderer.cpp Renderer.cpp ${lf_renderer_HEADERS_MOC})
target_compile_features(simple_renderer PRIVATE cxx_range_for)
target_link_libraries(simple_renderer ${OpenCL_LIBRARIES} Qt5::Widgets) #Qt5::OpenGL
