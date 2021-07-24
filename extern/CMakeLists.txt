add_subdirectory("glfw-3.3")
add_subdirectory("Glad")
add_subdirectory("glm-0.9.9.5")

add_library(imgui STATIC
	"imgui/imgui.cpp"
	"imgui/imgui_draw.cpp"
	"imgui/imgui_widgets.cpp"
	"imgui/imgui_demo.cpp"
	"imgui/imgui_tables.cpp"
	"imgui/backends/imgui_impl_glfw.cpp"
	"imgui/backends/imgui_impl_opengl3.cpp"
 )

set_target_properties(imgui PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(imgui
	PUBLIC
		"imgui"
)

target_compile_definitions(imgui
	PUBLIC
		"IMGUI_IMPL_OPENGL_LOADER_GLAD"
)

target_link_libraries(imgui
	PUBLIC
		glad
		glfw
)

add_library(imguizmo STATIC
	"ImGuizmo/ImGuizmo.h"
	"ImGuizmo/ImGuizmo.cpp"
)

set_target_properties(imguizmo PROPERTIES LINKER_LANGUAGE CXX)


target_link_libraries(imguizmo
	PRIVATE
		imgui
)

target_include_directories(imguizmo
	PUBLIC
		"ImGuizmo"
)