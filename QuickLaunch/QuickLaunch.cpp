// QuickLaunch.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "QuickLaunch.h"
#include "windowsx.h"

#define MAX_LOADSTRING 100
#define	WM_USER_SHELLICON WM_USER + 1
#define WM_TASKBAR 1234

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

HHOOK hook;
NOTIFYICONDATA nidApp;
HMENU hPopMenu;
HWND hDial;
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK	ShortcutKey(int message, WPARAM wParam, LPARAM lParam);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_QUICKLAUNCH, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
	
    // Perform application initialization:
    //if (!InitInstance (hInstance, nCmdShow))
    //{
    //    return FALSE;
    //}

	HWND hDialog = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);
	hDial = hDialog;
	HICON hMainIcon;
	hMainIcon = LoadIcon(hInstance, (LPCTSTR)MAKEINTRESOURCE(IDI_SMALL));

	nidApp.cbSize = sizeof(NOTIFYICONDATA); // sizeof the struct in bytes 
	nidApp.hWnd = (HWND)hDialog;              //handle of the window which will process this app. messages 
	nidApp.uID = IDI_SMALL;           //ID of the icon that willl appear in the system tray 
	nidApp.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; //ORing of all the flags 
	nidApp.hIcon = hMainIcon; // handle of the Icon to be displayed, obtained from LoadIcon 
	nidApp.uCallbackMessage = WM_USER_SHELLICON;
	LoadString(hInstance, IDS_APPTOOLTIP, nidApp.szTip, MAX_LOADSTRING);
	Shell_NotifyIcon(NIM_ADD, &nidApp);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_QUICKLAUNCH));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!IsDialogMessage(hDialog, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
	wcex.hIcon			= nullptr;	//LoadIcon(hInstance, MAKEINTRESOURCE(IDI_QUICKLAUNCH));
	wcex.hCursor		= 0;		// LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground	= 0;		//(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = nullptr;		// MAKEINTRESOURCEW(IDC_QUICKLAUNCH);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable
   

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_USER_SHELLICON:
	{
		POINT lpClickPoint;
		switch (LOWORD(lParam))
		{
		case WM_RBUTTONDOWN: {
			UINT uFlag = MF_BYPOSITION | MF_STRING;
			GetCursorPos(&lpClickPoint);
			hPopMenu = CreatePopupMenu();
			if (state)
				InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDS_SHOW, _T("Hide"));
			else
				InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDS_SHOW, _T("Show"));

			InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDS_SCAN, _T("Scan to build database"));
			InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDS_STATIC, _T("View statitistics"));
			InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDM_EXIT, _T("Exit"));

			SetForegroundWindow(hWnd);
			TrackPopupMenu(hPopMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, lpClickPoint.x, lpClickPoint.y, 0, hWnd, NULL);

			return TRUE;
		}
		}
		break;
	}
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case IDS_SHOW:
				if (state)
					ShowWindow(hWnd, SW_HIDE);
				else
					ShowWindow(hWnd, SW_RESTORE);
				state = !state;
				break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
				Shell_NotifyIcon(NIM_DELETE, &nidApp);
				//saveDatabase();
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	//HWND hThis = GetDlgItem(hDlg, IDC_COMBO1);
	WCHAR buffer[MAX_LOADSTRING];
	bool check;
	HWND hEdit = GetDlgItem(hDlg, IDC_COMBO1);
	int notifycode = HIWORD(wParam);

	switch (message)
	{
	case WM_CREATE:
		//hook = SetWindowsHookEx(WH_KEYBOARD_LL, ShortcutKey, hInst, 0);
		break;
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	case WM_USER_SHELLICON:
		POINT lpClickPoint;
		switch (lParam) 
		{
		
		case WM_LBUTTONDBLCLK:
			ShowWindow(hDlg, SW_RESTORE);
			break;
		case WM_RBUTTONDOWN:
			UINT uFlag = MF_BYPOSITION | MF_STRING;
			GetCursorPos(&lpClickPoint);
			hPopMenu = CreatePopupMenu();
			if (state)
				InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDS_SHOW, _T("Hide"));
			else
				InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDS_SHOW, _T("Show"));

			InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDS_SCAN, _T("Scan to build database"));
			InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDS_STATIC, _T("View statitistics"));
			InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDM_EXIT, _T("Exit"));

			SetForegroundWindow(hDlg);
			TrackPopupMenu(hPopMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, lpClickPoint.x, lpClickPoint.y, 0, hDlg, NULL);
			break;
		}

		break;
	case WM_COMMAND:
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDC_COMBO1:
			if (notifycode == CBN_EDITUPDATE)
			{
				GetWindowText(hEdit, buffer, MAX_FILE_NAME);
				for (int i = 0; i < 3; i++)
					ComboBox_DeleteString(hEdit, 0);

				int count = 0;
				int i = 0;
				int buffLen = wcslen(buffer);
				while (i < ListApp.size()) {
					if (count == 5)		// Giới hạn hiển thị tối đa 5 item tìm được trong 1 dropbox
						break;
					if (stringCompareBeginLetters(buffer, buffLen, ListApp[i].szName)) {
						ComboBox_AddString(hEdit, ListApp[i].szName);
						count++;
					}
					i++;
				}

				ComboBox_ShowDropdown(hEdit, TRUE);

			}
			break;
		case IDOK:
			//SendMessage(hThis, CB_ADDSTRING, NULL, (LPARAM)L"test 1");
			

			GetWindowText(hEdit, buffer, MAX_LOADSTRING);
			check = false;
			for (int i = 0; i < ListApp.size(); i++) {
				if (StrCmp(ListApp[i].szName, buffer) == 0) {
					check = true;
					ShellExecute(NULL, L"open", ListApp[i].szPath, NULL, NULL, SW_SHOWDEFAULT);
					break;
				}
			}

			if (!check) {
				MessageBox(0, L"Path not found !!!", 0, 0);
			}


		
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			break;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			break;
		case IDS_SHOW:
			if (state)
				ShowWindow(hDlg, SW_HIDE);
			else
				ShowWindow(hDlg, SW_RESTORE);
			state = !state;
			break;
		case IDS_SCAN:
			LoadData(PRO_FILE);
			MessageBox(0, L"Load data done !!!", L"LOAD data done !!!", 0);
			sort(ListApp.begin(), ListApp.end(), compareFirstLetter);
			saveData();
			MessageBox(0, L"Save data done !!!", L"SAVE data done !!!", 0);
			break;
		case IDS_STATIC:
			MessageBox(0, L"Nothing!", L"Nothing!", 0);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hDlg, About);
			break;
		case IDM_EXIT:
			Shell_NotifyIcon(NIM_DELETE, &nidApp);
			PostQuitMessage(0);

			break;
		default:
			return DefWindowProc(hDlg, message, wParam, lParam);
		}
		break;


	}
	return (INT_PTR)FALSE;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

//LRESULT CALLBACK ShortcutKey(int message, WPARAM wParam, LPARAM lParam)
//{
//	if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_SPACE))
//	{
//		if (IsWindowVisible(hDlg) == false)
//		{
//			ShowWindow(hDlg, 1);
//			SetForegroundWindow(hDlg);
//			SetFocus(hDlg);
//			BringWindowToTop(hDlg);
//		}
//	}
//	return CallNextHookEx(hook, message, wParam, lParam);
//}