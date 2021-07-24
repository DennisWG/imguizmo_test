# Minimal reproducible example of ImGuizmo not working properly
This repository exists to showcase a bug I've encountered while using [ImGuizmo](https://github.com/CedricGuillemet) ([the issue](https://github.com/CedricGuillemet/ImGuizmo/issues/192)).

You can find the relevant code inside `main.cpp` within the functions `render_viewport` and `render`.
While the former only renders 2 ImGui panels for you to play around with, the latter is responsible for rendering a cube to a framebuffer and then displaying said framebuffer inside a "viewport panel".
It also displays the ImGuizmo's gizmo.

You can freely move and look around in the 3d space using the following keys:
* WASD - move forward/left/backward/right
* Q/E - move up/down
* hold right mouse - rotate the camera

I truly hope this helps in pinpointing this annoying little bug!

## Disclaimer
This example uses third party code which can be found inside the folder "extern". All the respective licenses apply!

My own code is public domain and free to modify and use in whatever way you see fit. No warranty implied; use at your own risk.
