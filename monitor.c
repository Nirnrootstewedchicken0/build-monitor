#include <windows.h>

#define STOP_EVENT_NAME "AutoMdbWriter_StopEvent_v1"

static HANDLE g_event = NULL;
static HHOOK  g_hook  = NULL;

static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_MBUTTONDOWN) {
        if (g_event) SetEvent(g_event);
        MessageBoxA(NULL, "C 抓到了滚轮！", "monitor", MB_OK);   // ← 新加
    }
    return CallNextHookEx(g_hook, nCode, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    (void)hInst; (void)hPrev; (void)lpCmd; (void)nShow;
    g_event = CreateEventA(NULL, TRUE, FALSE, STOP_EVENT_NAME);
    if (!g_event) return 1;
    g_hook = SetWindowsHookExA(WH_MOUSE_LL, MouseProc, NULL, 0);
    if (!g_hook) { CloseHandle(g_event); return 1; }
    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    UnhookWindowsHookEx(g_hook);
    CloseHandle(g_event);
    return 0;
}