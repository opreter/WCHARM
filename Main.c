/*
	MS-Windows アプリケーションスケルトン
*/
#include	<windows.h>
#include	<tchar.h>

#ifdef _MSC_VER
#elif	!defined(BCC32)
#define	WINCE
#endif

#if defined(WINCE) || defined(_MSC_VER)
#include	<commctrl.h>
#endif

#include	"resource.h"	/* リソース専用ヘッダ */

// グローバル変数 :
HINSTANCE	hInst;		// 現在のインスタンス
HWND		hwndCB;		// コマンド バー ウィンドウ

HMENU		hMnu;		// メニューハンドル

TCHAR	cmdline[256];

extern	TCHAR	szAppName[256];

/*	WM_PAINT(PAINTイベント)	*/
extern	int wproc_paint(HWND hwnd,int wParam,long lParam);
/*	WM_CREATE(CREATEイベント)	*/
extern	int wproc_create(HWND hwnd,int wParam,long lParam);
/*	WM_DESTORY	*/
extern	int wproc_destroy(HWND hwnd,int wParam,long lParam);
/*	WM_COMMAND	*/
extern	int	wproc_command(HWND hwnd,int wParam,long lParam);
/*	マウスダウン	*/
extern	int	wproc_mousedown(HWND hwnd,int wParam,UINT xpos,UINT ypos);
/*	マウスアップ	*/
extern	int	wproc_mouseup(HWND hwnd,int wParam,UINT xpos,UINT ypos);
/*	マウス移動	*/
extern	int	wproc_mousemove(HWND hwnd,int wParam,UINT xpos,UINT ypos);

extern	int	wproc_dropfile(HWND hwnd,HDROP wParam,long lParam);
/*	WM_KEYDOWN	*/
extern	int	wproc_keydown(HWND hwnd,int wParam,long lParam);
/*	WM_CLOSE	*/
extern	int	wproc_close(HWND hwnd,int wParam,long lParam);
/*	マウスダブルクリック	*/
extern	int	wproc_mousedbl(HWND hwnd,int wParam,UINT xpos,UINT ypos);
/*	メニュー初期化後	*/
extern	void	wproc_menuinit(HWND hwnd,HMENU hmenu);
//	タイトルバーに表示するプログラム名。リソースから展開できるようにインスタンスも渡す。
extern	void	GetAppName(LPTSTR pappname, HINSTANCE hInstance, int size);

#ifndef	WINCE
/*	ウィンドウのCREATEイベント時点で、ウィンドウサイズを固定値に	*/
void	crewin_resize(HWND hwnd,int xv,int yv)
{
	RECT	rect1,rect2;
	int	flame,title;

	GetWindowRect(hwnd,&rect1);	/* ウィンドウサイズを得る */
	GetClientRect(hwnd,&rect2);	/* クライアントサイズを得る */

	title=rect1.bottom-rect1.top-rect2.bottom;	/* ﾀｲﾄﾙﾊﾞｰ分のサイズ */
	flame=rect1.right-rect1.left-rect2.right;	/* 枠分のサイズ */

	/* ウィンドウサイズを再設定 */
	SetWindowPos(hwnd,0,rect1.left,rect1.top,xv+flame,yv+title,0);
}
#endif

/*  クリックかダブルクリックかを判断するために使われるタイマーID */
UINT uClickTimerID = 3;
UINT sv_wp;
LONG sv_lp;

/*	ウィンドウプロシージャ	*/
#if defined(_MSC_VER) && _WIN64
LRESULT CALLBACK WndProc(_In_ HWND hwnd, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam)
#else
int WINAPI WndProc(HWND hwnd, UINT message, int wParam, long lParam)
#endif // _MSC_VER
{
	UINT	xpos,ypos;
	
	switch(message){
	case WM_CLOSE:
		wproc_close(hwnd,wParam,lParam);
		break;
#ifndef	WINCE
	case WM_DROPFILES:	/* D&Dイベント。CEはいちおうなし */
		wproc_dropfile(hwnd,(HDROP)wParam,lParam);
		break;
#endif
	case WM_KEYDOWN:
		wproc_keydown(hwnd,wParam,lParam);
		break;
	case WM_MOUSEMOVE:
		xpos=LOWORD(lParam);
		ypos=HIWORD(lParam);
		wproc_mousemove(hwnd,wParam,xpos,ypos);
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		sv_wp=wParam;	/* パラメータを保存 */
		sv_lp=lParam;
		/* クリックとダブルクリックを区別するためにタイマーを設定 */
		SetTimer(hwnd, uClickTimerID, GetDoubleClickTime(), NULL);
		break;
	case WM_TIMER:
		KillTimer(hwnd, uClickTimerID);    /* タイマー破棄 */
		wParam=sv_wp;	/* 保存していたパラメータを復帰 */
		lParam=sv_lp;
		xpos=LOWORD(lParam);
		ypos=HIWORD(lParam);
		wproc_mousedown(hwnd,wParam,xpos,ypos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		xpos=LOWORD(lParam);
		ypos=HIWORD(lParam);
		wproc_mouseup(hwnd,wParam,xpos,ypos);
		break;
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
		KillTimer(hwnd, uClickTimerID);    /* タイマー破棄 */
		xpos=LOWORD(lParam);
		ypos=HIWORD(lParam);
		wproc_mousedbl(hwnd,wParam,xpos,ypos);
		break;
	case WM_CREATE:
		// ウィンドウ生成時の処理
		wproc_create(hwnd,wParam,lParam);
		break;
	case WM_PAINT:
		// ウインドウのクライアントエリアに、選択したファイルの該当
		// する部分を描画する。
		wproc_paint(hwnd,wParam,lParam);
		break;
	case WM_COMMAND:
		wproc_command(hwnd,wParam,lParam);
		break;
	case WM_DESTROY:
		// DestroyWindow 呼び出しによる終了の合図。
		wproc_destroy(hwnd,wParam,lParam);
#ifdef WINCE
		CommandBar_Destroy(hwndCB);
#endif
		PostQuitMessage(0);
		break;
	}
	
	return DefWindowProc(hwnd,message,wParam,lParam);
}

int PASCAL WinMain(HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPTSTR lpszCmdParam,
		int nCmdShow)
{
//	static char szAppName[] =APPNAME;
	HWND  hwnd ;
	MSG   msg ;
	WNDCLASS wndclass ;
	RECT	rc;

	hInst = hInstance;	// グローバル変数にインスタンスを保存
	_tcscpy(cmdline,lpszCmdParam);
	
#if defined(_MSC_VER)
	// アプリ名を展開
	GetAppName(szAppName, hInstance, 256);
#elif	defined(WINCE)
	/* アプリ名をグローバル変数に設定(CEは必ず必要) */
	LoadString(hInstance, IDS_APPNAME,szAppName,256);
#endif

	if (!hPrevInstance){
		/* ウィンドウクラスの宣言 */
		wndclass.style   = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wndclass.lpfnWndProc   = (WNDPROC) WndProc ;
		wndclass.cbClsExtra = 0 ;
		wndclass.cbWndExtra = 0 ;
		wndclass.hInstance  = hInstance ;
		wndclass.hCursor    = LoadCursor (NULL, IDC_ARROW) ;
		wndclass.lpszClassName = szAppName ;
		
		//wndclass.hIcon   = LoadIcon (NULL, IDI_APPLICATION) ;
		//wndclass.hIcon   = NULL;
		wndclass.hIcon   = LoadIcon (hInst, MAKEINTRESOURCE(IDI_ICON1));
		//wndclass.hbrBackground = GetStockObject (COLOR_BACKGROUND);
		wndclass.hbrBackground = GetStockObject (WHITE_BRUSH);
#if defined(_MSC_VER)
		wndclass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
#else
		wndclass.lpszMenuName  = NULL; // ←CE/W32の場合はNULL(?);
#endif
		
		RegisterClass (&wndclass) ;
	}

#ifndef	WINCE
	hwnd = CreateWindow (
		szAppName,
		szAppName,
		//WS_OVERLAPPEDWINDOW,
		(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX),
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
	) ;
	
	GetClientRect(hwnd,&rc);	/* クライアントサイズを得る */

#if defined(_MSC_VER)
	/* メニューハンドルを保存	*/
	hMnu = GetMenu(hwnd);
#else
	/*	メニューリソースを割り当てる	*/
//	hMnu=LoadMenu(hInstance ,MAKEINTRESOURCE(IDR_MENU1));
	hMnu=LoadMenu(hInstance ,_T("IDR_MENU1"));
	SetMenu(hwnd,hMnu);	/* メニューをウィンドウに設定 */

	/* メニュー分が増えたのでこゆことしてるわけ(^^;; */
	crewin_resize(hwnd,rc.right-rc.left,rc.bottom-rc.top);
#endif
#else
	// WinCE用
	hwnd = CreateWindow (
		szAppName,
		szAppName,
		WS_VISIBLE,	// ←ここが違う
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
	) ;
	
	/*	ウィンドウメニューの表示。CEはメニューバーを動的に
		作っていて、メインウィンドウ部に重なってる。PAINT
		イベント時注意！	*/
	hwndCB = CommandBar_Create(hInst, hwnd, 1);
	CommandBar_InsertMenubar(hwndCB, hInst, IDR_MENU1, 0);
	CommandBar_AddAdornments(hwndCB, 0, 0);
	hMnu=CommandBar_GetMenu(hwndCB,0);
#endif
	wproc_menuinit(hwnd,hMnu);	/* メニュー初期化後のメニュー設定 */

	ShowWindow (hwnd, nCmdShow) ;
	UpdateWindow (hwnd) ;
	while (GetMessage (&msg, NULL, 0, 0)){
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return msg.wParam ;
}

#if 0
main()
{
	WinMain (0,0,0,1);
}
#endif
