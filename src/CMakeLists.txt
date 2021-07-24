add_executable(imguizmo_test
	"main.cpp"
 "app.hpp" "app.cpp" "camera.hpp" "camera.cpp")

target_compile_features(imguizmo_test PUBLIC cxx_std_20)
set_target_properties(imguizmo_test PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(imguizmo_test
	PUBLIC
		glad
		glm
		glfw
		imgui
		imguizmo
)