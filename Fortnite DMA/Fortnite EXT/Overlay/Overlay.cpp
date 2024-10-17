#include "../General.h"
#include "Overlay.h"

#include "Renderer/Renderer.h"

std::unique_ptr<Overlay> Overlay::instance = nullptr;
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void RenderEntitys(Config& conf, EntityLoop& Entity) {
    for (auto it = Entity.PlayerInf.Boxes.begin(); it != Entity.PlayerInf.Boxes.end(); ) {

        if (conf.Setting.ShowBox) {
            Renderer::DrawBox(it->position.x, it->position.y, it->width, it->height, conf.Setting.BoxColor);
        }

        if (conf.Setting.ShowSkeleton) {

        }

        ++it;
    }
}

void RenderUtils(Config& conf, Overlay& overlay, ImGuiIO& io) {
    if (conf.Setting.Aimbot && conf.Setting.ShowFOV) {
        Renderer::DrawCircle(overlay.OverlayInf.screenWidth / 2, overlay.OverlayInf.screenHeight / 2, conf.Setting.FOVSize, conf.Setting.FOVColor, 300);
    }
    if (conf.Setting.ShowStats) {
        char buffer[256];
        sprintf_s(buffer, sizeof(buffer), "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::GetForegroundDrawList()->AddText(ImVec2(10, 10), ImColor(255, 255, 255, 255), buffer);
    }

}

void RenderMenu(Config& conf, Overlay& overlay) {
    if (!conf.Setting.ShowMenu) { return; }
    ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_Always);
    ImGui::Begin("GeneralMenu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    {
        ImGui::Text("CheatName");
        if (ImGui::Button("Aimbot", ImVec2(75, 35))) { overlay.Settings.PageTab = 0; } ImGui::SameLine();
        if (ImGui::Button("Visuals", ImVec2(75, 35))) { overlay.Settings.PageTab = 1; } ImGui::SameLine();
        if (ImGui::Button("Settings", ImVec2(75, 35))) { overlay.Settings.PageTab = 2; }
        if (overlay.Settings.PageTab == 0) {
            ImGui::Checkbox("Enable Aim", &conf.Setting.Aimbot);
            ImGui::Checkbox("Show FOV", &conf.Setting.ShowFOV);
            ImGui::SliderFloat("Aim FOV", &conf.Setting.FOVSize, 25.0f, 180.0f);
        }
        if (overlay.Settings.PageTab == 1) {
            ImGui::Checkbox("Box Esp", &conf.Setting.ShowBox);
            ImGui::Checkbox("Skeleton Esp", &conf.Setting.ShowSkeleton);
        }
        if (overlay.Settings.PageTab == 2) {
            ImGui::ColorEdit3("Box Color", (float*)&conf.Setting.BoxColor);
            ImGui::ColorEdit3("Skeleton Color", (float*)&conf.Setting.SkeletonColor);
            ImGui::ColorEdit3("FOV Color", (float*)&conf.Setting.FOVColor);
        }
    }
    ImGui::End();
}

void Overlay::OverlayLoop() {
    EntityLoop& Entity = EntityLoop::getInstance();
    Config& conf = Config::getInstance();
    Overlay& overlay = Overlay::getInstance();
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.00f);
    static RECT old_rc;
    // Main loop
    bool done = false;
    while (!done) {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        HWND hwnd_active = GetForegroundWindow();
        if (hwnd_active == OverlayInf.Window) {
            HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
            SetWindowPos(OverlayInf.Window, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }

        RECT rc;
        POINT xy;

        ZeroMemory(&rc, sizeof(RECT));
        ZeroMemory(&xy, sizeof(POINT));
        GetClientRect(OverlayInf.Window, &rc);
        ClientToScreen(OverlayInf.Window, &xy);
        rc.left = xy.x;
        rc.top = xy.y;

        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = NULL;
        io.DeltaTime = 1.0f / 60.0f;

        POINT p;
        GetCursorPos(&p);
        io.MousePos.x = p.x - xy.x;
        io.MousePos.y = p.y - xy.y;

        if (GetAsyncKeyState(VK_HOME) & 1) {
            conf.Setting.ShowMenu = !conf.Setting.ShowMenu;
        }

        if (GetAsyncKeyState(VK_LBUTTON)) {
            io.MouseDown[0] = true;
            io.MouseClicked[0] = true;
            io.MouseClickedPos[0].x = io.MousePos.x;
            io.MouseClickedPos[0].x = io.MousePos.y;
        }
        else { io.MouseDown[0] = false; }

        if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
        {
            old_rc = rc;

            OverlayInf.screenWidth = rc.right;
            OverlayInf.screenHeight = rc.bottom;

            OverlayInf.g_d3dpp.BackBufferWidth = OverlayInf.screenWidth;
            OverlayInf.g_d3dpp.BackBufferHeight = OverlayInf.screenHeight;
            SetWindowPos(OverlayInf.Window, (HWND)0, xy.x, xy.y, OverlayInf.screenWidth, OverlayInf.screenHeight, SWP_NOREDRAW);
            OverlayInf.g_pd3dDevice->Reset(&OverlayInf.g_d3dpp);
        }

        // Handle lost D3D9 device
        if (OverlayInf.g_DeviceLost) {
            HRESULT hr = OverlayInf.g_pd3dDevice->TestCooperativeLevel();
            if (hr == D3DERR_DEVICELOST) {
                ::Sleep(10);
                continue;
            }
            if (hr == D3DERR_DEVICENOTRESET) {
                ResetDevice();
            }
            OverlayInf.g_DeviceLost = false;
        }

        // Handle window resize
        if (OverlayInf.g_ResizeWidth != 0 && OverlayInf.g_ResizeHeight != 0) {
            OverlayInf.g_d3dpp.BackBufferWidth = OverlayInf.g_ResizeWidth;
            OverlayInf.g_d3dpp.BackBufferHeight = OverlayInf.g_ResizeHeight;
            OverlayInf.g_ResizeWidth = OverlayInf.g_ResizeHeight = 0;
            ResetDevice();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        RenderMenu(conf, overlay);
        RenderUtils(conf, overlay, io);
        RenderEntitys(conf, Entity);

        // Rendering
        ImGui::EndFrame();
        OverlayInf.g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        OverlayInf.g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        OverlayInf.g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        OverlayInf.g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f),
            (int)(clear_color.y * clear_color.w * 255.0f),
            (int)(clear_color.z * clear_color.w * 255.0f),
            (int)(clear_color.w * 255.0f));
        OverlayInf.g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (OverlayInf.g_pd3dDevice->BeginScene() >= 0) {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            OverlayInf.g_pd3dDevice->EndScene();
        }
        HRESULT result = OverlayInf.g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
        if (result == D3DERR_DEVICELOST) { OverlayInf.g_DeviceLost = true; }
        Sleep(3);
    }

    // Cleanup
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    ::DestroyWindow(OverlayInf.Window);
    ::UnregisterClassW(OverlayInf.wc.lpszClassName, OverlayInf.wc.hInstance);
}

bool Overlay::OverlaySetup() {
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    OverlayInf.screenWidth = desktop.right;
    OverlayInf.screenHeight = desktop.bottom;

    OverlayInf.wc = { sizeof(OverlayInf.wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&OverlayInf.wc);

    //OverlayInf.Window = ::CreateWindowW(OverlayInf.wc.lpszClassName, L"Dear ImGui DirectX9 Example", WS_POPUP, 0, 0, screenWidth, screenHeight, nullptr, nullptr, OverlayInf.wc.hInstance, nullptr);
    OverlayInf.Window = FindWindowA("WindowsForms10.Window.8.app.0.141b42a_r3_ad1", "AlphaForm");
    if (!OverlayInf.Window) { MessageBoxA(NULL, "Failed Finding Overlay!", "ERROR", MB_OK); }
    MoveWindow(OverlayInf.Window, 0, 0, OverlayInf.screenWidth, OverlayInf.screenHeight, TRUE);

    // Set layered window attributes for transparency
    SetWindowLong(OverlayInf.Window, GWL_EXSTYLE, GetWindowLong(OverlayInf.Window, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(OverlayInf.Window, RGB(0, 0, 0), 0, LWA_COLORKEY);

    // Initialize Direct3D
    if (!CreateDeviceD3D(OverlayInf.Window)) {
        CleanupDeviceD3D();
        ::UnregisterClassW(OverlayInf.wc.lpszClassName, OverlayInf.wc.hInstance);
        return false;
    }

    // Show the window
    ::ShowWindow(OverlayInf.Window, SW_SHOWDEFAULT);
    ::UpdateWindow(OverlayInf.Window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(OverlayInf.Window);
    ImGui_ImplDX9_Init(OverlayInf.g_pd3dDevice);
    return true;
}

bool Overlay::CreateDeviceD3D(HWND hWnd) {
    if ((OverlayInf.g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
        return false;

    // Create the D3DDevice
    ZeroMemory(&OverlayInf.g_d3dpp, sizeof(OverlayInf.g_d3dpp));
    OverlayInf.g_d3dpp.Windowed = TRUE; // Keep TRUE for windowed mode
    OverlayInf.g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    OverlayInf.g_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8; // Use a format that supports alpha
    OverlayInf.g_d3dpp.EnableAutoDepthStencil = TRUE;
    OverlayInf.g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    OverlayInf.g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // Present with vsync

    if (OverlayInf.g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &OverlayInf.g_d3dpp, &OverlayInf.g_pd3dDevice) < 0)
        return false;

    return true;
}

void Overlay::CleanupDeviceD3D() {
    if (OverlayInf.g_pd3dDevice) { OverlayInf.g_pd3dDevice->Release(); OverlayInf.g_pd3dDevice = nullptr; }
    if (OverlayInf.g_pD3D) { OverlayInf.g_pD3D->Release(); OverlayInf.g_pD3D = nullptr; }    
    if (OverlayInf.Window) { DestroyWindow(OverlayInf.Window); OverlayInf.Window = nullptr; }
}

void Overlay::ResetDevice() {
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = OverlayInf.g_pd3dDevice->Reset(&OverlayInf.g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Overlay& overlay = Overlay::getInstance();
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        overlay.OverlayInf.g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        overlay.OverlayInf.g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
