#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "comsuppw.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <comutil.h>

TCHAR szClassName[] = TEXT("Window");

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hButton;
	static HWND hEdit1;
	static HWND hEdit2;
	switch (msg)
	{
	case WM_CREATE:
		hEdit1 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("google.co.jp"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton = CreateWindow(TEXT("BUTTON"), TEXT("取得"), WS_VISIBLE | WS_CHILD | WS_TABSTOP, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit2 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_TABSTOP | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		break;
	case WM_SIZE:
		MoveWindow(hEdit1, 10, 10, 256, 32, TRUE);
		MoveWindow(hButton, 276, 10, 128, 32, TRUE);
		MoveWindow(hEdit2, 10, 50, LOWORD(lParam) - 20, HIWORD(lParam) - 60, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			SetWindowText(hEdit2, 0);
			DWORD dwSize = GetWindowTextLengthA(hEdit1);
			LPSTR lpszHost = (LPSTR)GlobalAlloc(GMEM_ZEROINIT, dwSize + 1);
			if (lpszHost)
			{
				GetWindowTextA(hEdit1, lpszHost, dwSize + 1);
				{
					WSADATA wsaData;
					if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0)
					{
						addrinfo hints = { 0 };
						hints.ai_family = AF_UNSPEC;
						hints.ai_socktype = SOCK_STREAM;
						hints.ai_protocol = IPPROTO_TCP;
						addrinfo *result = NULL;
						if (getaddrinfo(lpszHost, NULL, &hints, &result) == 0)
						{
							for (addrinfo *ptr = result; ptr != NULL; ptr = ptr->ai_next)
							{
								switch (ptr->ai_family)
								{
								case AF_INET:
								{
									sockaddr_in *sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
									BSTR bstrText = _com_util::ConvertStringToBSTR(inet_ntoa(sockaddr_ipv4->sin_addr));
									SendMessage(hEdit2, EM_REPLACESEL, 0, (LPARAM)bstrText);
									SendMessage(hEdit2, EM_REPLACESEL, 0, (LPARAM)TEXT("\r\n"));
									SysFreeString(bstrText);
								}
								break;
								}
							}
							freeaddrinfo(result);
						}
						WSACleanup();
					}
				}
				GlobalFree(lpszHost);
			}
			SetFocus(hEdit1);
			SendMessage(hEdit1, EM_SETSEL, 0, -1);
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefDlgProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		DLGWINDOWEXTRA,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("ホスト名からIPアドレスを取得する"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		if (!IsDialogMessage(hWnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	WSACleanup();
	return (int)msg.wParam;
}
