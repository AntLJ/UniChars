// UniChars.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "UniChars.h"
#include <commdlg.h>

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_UNICHARS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_UNICHARS));

    MSG msg;

    // メイン メッセージ ループ:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_UNICHARS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_UNICHARS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

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

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static CHOOSEFONT cf;
	static int iPage;
	static LOGFONT lf;
	HDC hdc;
	int cxChar, cyChar, x, y, i, cxLabels;
	PAINTSTRUCT ps;
	SIZE size;
	TCHAR szBuffer[8];
	TEXTMETRIC tm;
	WCHAR ch;
	switch (message)
	{
	case WM_CREATE:
		hdc = GetDC(hwnd);
		lf.lfHeight = -GetDeviceCaps(hdc, LOGPIXELSY) / 6; // 12 points
		lstrcpy(lf.lfFaceName, TEXT("Lucida Sans Unicode"));
		ReleaseDC(hwnd, hdc);
		cf.lStructSize = sizeof(CHOOSEFONT);
		cf.hwndOwner = hwnd;
		cf.lpLogFont = &lf;
		cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
		SetScrollRange(hwnd, SB_VERT, 0, 255, FALSE);
		SetScrollPos(hwnd, SB_VERT, iPage, TRUE);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_FONT:
			if (ChooseFont(&cf))
				InvalidateRect(hwnd, NULL, TRUE);
			return 0;
		}
		return 0;
	case WM_VSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_LINEUP: iPage -= 1; break;
		case SB_LINEDOWN: iPage += 1; break;
		case SB_PAGEUP: iPage -= 16; break;
		case SB_PAGEDOWN: iPage += 16; break;
		case SB_THUMBPOSITION:iPage = HIWORD(wParam); break;
		default:
			return 0;
		}
		iPage = max(0, min(iPage, 255));
		SetScrollPos(hwnd, SB_VERT, iPage, TRUE);
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		SelectObject(hdc, CreateFontIndirect(&lf));
		GetTextMetrics(hdc, &tm);
		cxChar = tm.tmMaxCharWidth;
		cyChar = tm.tmHeight + tm.tmExternalLeading;
		cxLabels = 0;
		for (i = 0; i < 16; i++)
		{
			wsprintf(szBuffer, TEXT(" 000%1X: "), i);
			GetTextExtentPoint(hdc, szBuffer, 7, &size);
			cxLabels = max(cxLabels, size.cx);
		}
		for (y = 0; y < 16; y++)
		{
			wsprintf(szBuffer, TEXT(" %03X_: "), 16 * iPage + y);
			TextOut(hdc, 0, y * cyChar, szBuffer, 7);
			for (x = 0; x < 16; x++)
			{
				ch = (WCHAR)(256 * iPage + 16 * y + x);
				TextOutW(hdc, x * cxChar + cxLabels, y * cyChar, &ch, 1);
			}
		}
		DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

// バージョン情報ボックスのメッセージ ハンドラーです。
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
