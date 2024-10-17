#pragma once

#include "../ImGui/imgui.h"
#include "../ImGui/imgui_internal.h"
#include "../ImGui/imgui_impl_dx9.h"
#include "../ImGui/imgui_impl_win32.h"

namespace Renderer {
	void DrawCircle(int x, int y, int radius, const float color[4], int segments);
	void DrawBox(int X, int Y, int W = 50.0f, int H = 50.0f, const float* color = nullptr, int thickness = 2.5);
}