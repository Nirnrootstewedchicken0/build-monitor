#include <windows.h>

#define STOP_EVENT_NAME "AutoMdbWriter_StopEvent_v1"

static HANDLE g_event = NULL;
static HHOOK  g_hook  = NULL;

static void bind_to_parent_job(void) {
    HANDLE job = CreateJobObjectA(NULL, NULL);
    if (!job) return;
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION info;
    ZeroMemory(&info, sizeof(info));
    info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    SetInformationJobObject(job, JobObjectExtendedLimitInformation, &info, sizeof(info));
    AssignProcessToJobObject(job, GetCurrentProcess());
}

static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_MBUTTONDOWN) {
        if (g_event) SetEvent(g_event);
    }
    return CallNextHookEx(g_hook, nCode, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    (void)hInst; (void)hPrev; (void)lpCmd; (void)nShow;
    bind_to_parent_job();
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