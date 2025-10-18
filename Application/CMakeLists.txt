set(TREE_VIZ_SOURCES
	Source/App.cpp
	Source/App.h
	Source/Frame.cpp
	Source/Frame.h
	Source/Canvas.cpp
	Source/Canvas.h
)

add_executable(TreeVisualizer WIN32
	${TREE_VIZ_SOURCES}
	${CMAKE_CURRENT_SOURCE_DIR}/wx_dpi_aware_pmv2.manifest
)

target_compile_definitions(TreeVisualizer PUBLIC
	__WXMSW__
	_UNICODE
)

target_compile_definitions(TreeVisualizer PRIVATE
	WIN32_LEAN_AND_MEAN
	NOMINMAX
	_USE_MATH_DEFINES
)

target_link_libraries(TreeVisualizer
	HappyMath
	wx::core
	wx::base
	wx::gl
)