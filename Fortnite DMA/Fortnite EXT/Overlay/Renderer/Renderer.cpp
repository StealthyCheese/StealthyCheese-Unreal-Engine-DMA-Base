#include "Renderer.h"

void Renderer::DrawCircle(int x, int y, int radius, const float color[4], int segments) {
    ImDrawList* Drawlist = ImGui::GetForegroundDrawList();
    Drawlist->AddCircle(ImVec2(x, y),radius,ImGui::ColorConvertFloat4ToU32(ImVec4(color[0], color[1], color[2], color[3])),segments);
}

void Renderer::DrawBox(int X, int Y, int W, int H, const float color[4], int thickness) {
    static float defaultColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    if (!color) { color = defaultColor; }
    ImDrawList* Drawlist = ImGui::GetForegroundDrawList();

    ImU32 outlineColor = ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1));
    Drawlist->AddLine(ImVec2(X, Y), ImVec2(X, Y + H), outlineColor, thickness);
    Drawlist->AddLine(ImVec2(X, Y), ImVec2(X + W, Y), outlineColor, thickness);
    Drawlist->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + H), outlineColor, thickness);
    Drawlist->AddLine(ImVec2(X + W, Y + H), ImVec2(X, Y + H), outlineColor, thickness);
    Drawlist->AddLine(ImVec2(X, Y + H), ImVec2(X, Y), outlineColor, thickness);
    Drawlist->AddLine(ImVec2(X, Y), ImVec2(X + W, Y), outlineColor, thickness);
    Drawlist->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + H), outlineColor, thickness);
    Drawlist->AddLine(ImVec2(X + W, Y + H), ImVec2(X, Y + H), outlineColor, thickness);

    ImU32 boxColor = ImGui::ColorConvertFloat4ToU32(ImVec4(color[0], color[1], color[2], color[3]));
    Drawlist->AddLine(ImVec2(X, Y), ImVec2(X, Y + H), boxColor, thickness);
    Drawlist->AddLine(ImVec2(X, Y), ImVec2(X + W, Y), boxColor, thickness);
    Drawlist->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + H), boxColor, thickness);
    Drawlist->AddLine(ImVec2(X + W, Y + H), ImVec2(X, Y + H), boxColor, thickness);
}