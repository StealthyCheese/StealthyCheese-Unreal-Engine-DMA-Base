#include <Windows.h>
#include <iostream>
#include <thread>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <dwmapi.h>

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "Resource Files/Librarys/DirectX/d3dx9.lib")

// DirectX
#include "Resource Files/Librarys/DirectX/d3dx9.h"

// Utils
//		//Structs
#include "Utils/Structs.h"

// Ntdll
#include "Resource Files/Ntdll/structs.h"

// DMA Library
#include "Resource Files/Librarys/vmmdll/vmmdll.h"

// Cache
#include "Cache/Cache.h"

// Config
#include "Config/Config.h"

// Memory Utils
#include "Memory/Registry.h"
#include "Memory/InputManager.h"
#include "Memory/Memory.h"

// Overlay
#include "Overlay/Overlay.h"

// EntityLoop
#include "EntityLoop/EntityLoop.h"