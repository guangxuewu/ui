// 17 july 2014

#include "winapi_windows.h"
#include "_cgo_export.h"

#define windowclass L"gouiwindow"

static LRESULT CALLBACK windowWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	void *data;
	RECT r;
	LRESULT lResult;

	data = getWindowData(hwnd, uMsg, wParam, lParam, &lResult, storeWindowHWND);
	if (data == NULL)
		return lResult;
	if (sharedWndProc(hwnd, uMsg, wParam, lParam, &lResult))
		return lResult;
	switch (uMsg) {
	case WM_SIZE:
		if (GetClientRect(hwnd, &r) == 0)
			xpanic("error getting client rect for Window in WM_SIZE", GetLastError());
		windowResize(data, &r);
		return 0;
	case WM_CLOSE:
		windowClosing(data);
		return 0;
	default:
		return DefWindowProcW(hwnd, uMsg, wParam, lParam);
	}
	xmissedmsg("Window", "windowWinProc()", uMsg);
	return 0;		// unreached
}

DWORD makeWindowWindowClass(char **errmsg)
{
	WNDCLASSW wc;

	ZeroMemory(&wc, sizeof (WNDCLASSW));
	wc.lpfnWndProc = windowWndProc;
	wc.hInstance = hInstance;
	wc.hIcon = hDefaultIcon;
	wc.hCursor = hArrowCursor;
	wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
	wc.lpszClassName = windowclass;
	if (RegisterClassW(&wc) == 0) {
		*errmsg = "error registering Window window class";
		return GetLastError();
	}
	return 0;
}

HWND newWindow(LPWSTR title, int width, int height, void *data)
{
	HWND hwnd;

	hwnd = CreateWindowExW(
		0,
		windowclass, title,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		NULL, NULL, hInstance, data);
	if (hwnd == NULL)
		xpanic("Window creation failed", GetLastError());
	return hwnd;
}

void windowClose(HWND hwnd)
{
	if (DestroyWindow(hwnd) == 0)
		xpanic("error destroying window", GetLastError());
}
