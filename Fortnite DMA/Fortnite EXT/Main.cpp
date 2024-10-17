#include "General.h"

int main() {
	Overlay& overlay = Overlay::getInstance();
    Cache& cache = Cache::getInstance();
    EntityLoop& Entity = EntityLoop::getInstance();
    Config& conf = Config::getInstance();

    std::thread([&overlay]() {
        if (!overlay.OverlaySetup()) {
            ExitProcess(0x1337);
        }
        overlay.OverlayLoop();
    }).detach();

    AllocConsole();
    FILE* fDummy;
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    printf("Welcome, Inizializing...\n");

    while (!mem.Init(true, true)) {
        printf("Failed to initilize DMA Retrying!\n");
        Sleep(1500);
    }
    printf("DMA initilized\n");

    while (!cache.ProcessInfo.TargetPID) {
        printf("Waiting For Game!\n");
        cache.ProcessInfo.TargetPID = mem.GetPidFromName(cache.ProcessInfo.ProcessName.c_str());
        Sleep(1500);
    }
    printf("Found Game PID: %X\n", cache.ProcessInfo.TargetPID);

    printf("{*] Fixing CR3\n");
    while (!mem.FixCr3()) {
        Sleep(1000);
    }

    Entity.MainLoop(mem, Entity, cache, conf, overlay);

	return 0;
}