#pragma once
#include <d3d9.h>
#include <d3dx9.h>

class Overlay {
private:
	static std::unique_ptr<Overlay> instance;
	Overlay() {}
public:

	struct MenuSettings {
		int PageTab = 0;
	};
	MenuSettings Settings;

	struct OverlayInfo {
		WNDCLASSEXW wc;
		HWND Window = 0;
		int screenWidth;
		int screenHeight;
		// Data
		LPDIRECT3D9              g_pD3D = nullptr;
		LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
		bool                     g_DeviceLost = false;
		UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
		D3DPRESENT_PARAMETERS    g_d3dpp = {};
	};
	OverlayInfo OverlayInf;

	void OverlayLoop();
	bool OverlaySetup();

    // Helper functions
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
    void ResetDevice();

	Overlay(const Overlay&) = delete;
	Overlay& operator=(const Overlay&) = delete;

	static Overlay& getInstance() {
		if (!instance) {
			instance.reset(new Overlay());
		}
		return *instance;
	}
};