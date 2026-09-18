/*
	Charmy for Windows32

0.00	スケルトン
0.01	時計表示のサンプルから、時計表示スレッド処理をまる移植
0.02	時計表示位置をウィンドウの左上に設定
0.03	フォントを変更。とりあえず、14ポイントに
0.04	CreateWindow時の指定を変更し、リサイズ出来ない細線のウィンドウ枠に
0.05	ウィンドウサイズを時計表示分のみに変更
0.06	メニューリソースを設定。中身はまだ。
0.07	Exitのみ、メニュー処理を作成
0.08	１行表示部分を分離(色換えに備えて)
0.09	３ヶ月分の表示をしてみた。
0.10	閏年チェックと月初め演算処理を組み込み。
0.11	起動時のウィンドウ位置を右上に位置させた。
0.12	各色の変数を用意、背景や各文字色を反映。
0.13	最初にバックを塗りつぶす処理を実装。
0.14	アイコンを埋め込む。
0.15	カレンダー表示フラグをもうけて、カレンダーを非表示にできるようにした。
0.16	時計非表示フラグをもうけた。時計非表示でもスレッドは稼働する。
0.17	時計の表示/非表示をメニューで選択できるようにした。
0.18	カレンダーの表示/非表示も同上。
0.19	１ヶ月分のみの表示フラグを作成。
0.20	３ヶ月横表示モードフラグを作成、表示位置演算＆ウィンドウリサイズも
	入れた。
0.21	ダブルクリック＆D&Dイベントテスト。lnkファイルを実行するため、
	ShellExecute()で実行。
0.22	ポップアップメニューテスト。クリックでポップアップメニューを開き、
	アイテムをクリックすると実行させた。
0.23	D&D＆ポップアップテスト。ファイルをドロップするとポップアップが開き、
	選択したアプリでファイルを開く。クリックしてもアプリを実行。
0.24	起動時に指定したディレクトリを検索して、リストを得る。D&D及びメニュー
	起動用アプリは、この方法でディレクトリを指定し、ショートカットを登録
	することで起動リストを作る方法の予定。
0.25	得たリストをメニューに登録。
0.26	iniファイルへの書き込み、読み出し処理を実装。中身はまだD&D用アプリ
	のショートカット検索先のみ。
0.27	File->Programでとりあえずポップアップの再建策を行うようにしてみた。
0.28	アイテムメニューへ追加するテスト
0.29	ディレクトリを検索してアイテムメニューへ追加
0.30	アイテムを選択して実行
0.31	WindowsExitExでWindowsのシャットダウンをできるようになった。
	これでほぼ予定機能は入ったなぁ…
0.32	Win2K、XP対応のため、ExitWindowsを呼ぶ前にシャットダウン権限を得る処理
	を追加。
0.33	iniファイルの検索をカレント→環境変数HOME→実行ファイルの順に検索する
	処理をPsycheから移植。
0.34	EosWM32から、メニューチェック処理を移植。時計、カレンダーの表示状態に
	応じてチェックを付けた。
0.35	iniファイルの内容を追加。ダブルクリック用項目 LeftPrg を追加。
	PathFlg項目に１をセットすると、DropPath,BootPathに「実行ファイルからの
	相対パス指定」とするようにした。(USBメモリ等への配置のため)
	また、DropPath,BootPathの存在確認も追加。
	とりあえずメニューにHelp→Aboutを追加。ただし中身はなにもしてない。
	リソースにバージョン情報も付加。ここから表示したいなー
0.36	起動メニュー内容検索処理を共通化。ショートカットのlnkとpifだけでなく、
	BATファイルとEXEファイルも検索対象に追加。
	とりあえず MessageBoxでバージョン表示を追加。
0.37	拡張子検索をテーブル化して、.urlと.websiteを追加。
0.38	VisualStudio2019へ環境を移行。
	GetVersion()がエラーになったので修正。
	他は、TCHAR周りの展開と、makepath/splitpathの変換、メニュー生成をウィンドウ
	作成と同時に行うことでなんとかなった？
	(ExitWindowsEx()はとりあえずWarningで済んでいるので保留)
	環境defineは「_MSC_VER」で行った。
0.39	iniファイル検索を、カレント→CHARM→HOME→マイドキュメント→argv0の順序に
	変更した。
	PathFlg項目は「実行ファイルからの相対パス」ではなく「iniファイルからの
	相対パス」とした。
	アプリ名もリソースのIDS_APP_NAMEから引くようにした。
0.40	色データをiniファイルに記録/読込処理を作成。
0.41	祝日CSV読込処理をなんとかした。
	最大200件とし、去年以降のデータを読み込むことにした。
	色データのiniファイルが項目なしの場合、デフォルト値を継承するようにした。
0.42	スタートアップフォルダ指定がある場合はそこから実行するようにした。
0.43	バージョン番号、他もろもろを、リソースから展開できるようにした。
0.44	「Setup」メニューからiniファイルをnotepadで開くようにして、
	設定ダイアログを省略することにした。
0.45	x64環境でコンパイル。WndProc()の引数宣言の変更が必要だったがそれ以外は
	Warningで済んだ(型変換でビット落ちするかも)のでよしとする。
	コンパイルは「_WIN64」で分けられるらしいので#ifで分離。
	同様にタイトル、およびAboutでの表示内容も変更。
	IDS_APP_TITLEは「Windows」のみにして、タイトル文字列作成関数で、
	「(x64)」「(x86)」をつけることにした。
0.46	正直微妙なのだが、全体的に再チェックして、char→TCHARの環境に
	書き換えた。
0.47	一定時間後にメッセージがポップアップするカウントダウンタイマを
	つけてみた。
0.48	カウントダウンタイマがタイムアウトしても、メッセージボックスが
	下にいたままだったので、ウィンドウを最前面に出すようにした。
0.49	どうもファイル検索で例外エラーになってしまう…
	ファイル検索時の一時メモリがうまくいっていないっぽい。
	やむなくCE版同様に固定バッファを確保した。
0.50	公開にむけて、iniファイルが無い場合にコメント付きのiniファイルを
	生成してみた。
*/
#include	<windows.h>
#include	<tchar.h>
#include	<stdio.h>

#ifdef	WINCE
#include	<commctrl.h>

HWND		hwndCB;		// コマンド バー ウィンドウ
#endif

#pragma	comment(lib,"Version.lib")

#include	"resource.h"	/* リソース専用ヘッダ */

#ifdef _MSC_VER
//	リソース名読み替え
#define	MNU_EXIT	IDM_EXIT
#endif // _MSC_VER

#include	"pbscmd32.h"
#include	"pbsdir32.h"
#include	"calend.h"
#include	"lanch.h"

extern	HINSTANCE	hInst;		// 現在のインスタンス
extern	HMENU		hMnu;		// メニューハンドル
extern	TCHAR	cmdline[256];

#ifdef _MSC_VER
TCHAR	szAppName[256];					// アプリ名(StringTableから展開するのでここは領域確保だけ)
//	バージョン番号はリソースへ組み込んだ
#else
TCHAR	szAppName[256] = _T("Charmy for Win32");	// アプリ名
#define	VERNO	"0.50"
#endif // _MSC_VER

/*	スレッド用	*/
DWORD Thread(LPVOID);

typedef struct{
	HWND	hwnd;
	BOOL	bEnd;
	TCHAR	fontname[80];
	int	fontsize;
	COLORREF bcl;
	COLORREF fcl;
	BOOL	dspf;
} PARAM, *PPARAM;

static HANDLE hThread;
DWORD threadID;
static PARAM data;

#if 1
TCHAR	Dropfiles[5120];	// D&Dされたファイル郡(コマンドライン)
#else
HLOCAL	hDrops;			/* D&Dされたファイル名列確保用メモリハンドル */
LPTSTR	Dropfiles;		/* D&Dされたファイル名列へのポインタ */
#endif
BOOL	Dropuse;		/* D&Dされたファイルの存在。メモリ解放時に
				   FALSEにすること */

TCHAR	Ddr[20],Ddir[1024],Dname[256],Dext[256];

extern	void	Pbs_exitwin(HWND hwnd,int Mode);
//	リソースからプロダクトバージョン文字列を読みだす
extern	BOOL	GetVersionResoce(LPTSTR namestr, LPTSTR verstr, LPTSTR copystr, LPTSTR compstr, HINSTANCE hInstance);

/*	Aboutダイアログ表示	*/
void	About_dig(HWND hwnd)
{
	TCHAR	xpath[256];
	TCHAR	str[1024];
	TCHAR	namestr[256];
	TCHAR	verstr[80];
	TCHAR	copystr[80];
	TCHAR	compstr[80];

	// リソースからバージョンを展開
	GetVersionResoce(namestr, verstr, copystr, compstr, hInst);
	// バージョン表示文字列生成
	_stprintf(str, _T("%s Ver. %s\n\n  %s %s"), namestr, verstr, copystr, compstr);

	MessageBox(
		hwnd,
		str,
		_T("About"),
		MB_ICONINFORMATION
	);
}

/*	pt→pix変換	*/
int	pt2pix_hdc(HDC hdc,int ipt)
{
	int	ppy,pt;
	
	ppy = ipt;
	pt = ppy * GetDeviceCaps(hdc, LOGPIXELSY);
	ppy = (pt / 72);
	
	return ppy;
}

/*	WM_CREATE(CREATEイベント)	*/
int	wproc_create(HWND hwnd,int wParam,long lParam)
{
	HDC	hdc;
	HFONT	hfont,hofont;
	SIZE	sz;
	int	i;
	TCHAR	xpath[256];
	
	/* 実行ファイルのあるパスを得る */
	GetModuleFileName( hInst, xpath, 255 );
	_splitpathT(xpath,Ddr,Ddir,Dname,Dext);

	Calender_ini(xpath);

	data.hwnd = hwnd;
	data.bEnd = FALSE;

	//hDrops=NULL;
	//Dropfiles=NULL;
	Dropuse=FALSE;
	
	/* フォントを一度設定して、フォントの縦横サイズを得る */
	hdc=GetDC(hwnd);
	
	VFontpix=pt2pix_hdc(hdc,VFontsize);	/* pt→pix変換 */
	
	hfont = CreateFont_VB(VFontname,VFontpix,0,0,0,0);
	hofont=SelectObject(hdc,hfont);
	
	GetTextExtentPoint32(hdc,_T("O"),1,&sz);
	FontX=sz.cx;
	FontY=sz.cy;
	
	SelectObject(hdc,hofont);
	DeleteObject(hfont);
	ReleaseDC(hwnd, hdc);
	
	Resize_Calender(hwnd);
	
	_tcscpy(data.fontname,VFontname);
	data.fontsize=VFontpix;
	data.bcl=ColorBack;
	data.fcl=ColorClock;
	data.dspf=Clockf;

	// タイマ変数クリア
	bTimer = FALSE;
	lnTimer = 0;

	/* 時計用スレッド始動 */
	hThread = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)Thread,
		(LPVOID)&data,
		0,
		(LPDWORD)&threadID
	);

	// スタートアップ郡起動
	StartupExec(hwnd);

#ifndef	WINCE
	DragAcceptFiles(hwnd,TRUE);	/* D&Dを許可 */
#endif
	return 0;
}

/*	WM_CLOSE	*/
int	wproc_close(HWND hwnd,int wParam,long lParam)
{
	Lanchfree();
	Dropfree();
	
	data.bEnd = TRUE;
	WaitForSingleObject(hThread, INFINITE);
	//MessageBox(NULL, "スレッド終了", "成功！", MB_OK);
	if(CloseHandle(hThread) == 0){
		MessageBox(NULL, _T("ハンドルクローズに失敗！"), _T("失敗！"), MB_OK);
	}
	DestroyWindow(hwnd);
	return 0;
}

/*	WM_DESTROY	*/
int	wproc_destroy(HWND hwnd,int wParam,long lParam)
{
	return 0;
}

/*	スレッド部	*/
DWORD Thread(LPVOID param)
{
	PPARAM pData;
	HDC hdc;
	RECT rc;

	pData = (PPARAM)param;

	while(!pData->bEnd) {
		if(pData->dspf!=FALSE){
			hdc = GetDC(pData->hwnd);
			Dispclock(pData->hwnd,hdc,pData->fontname,pData->fontsize,pData->bcl,pData->fcl);
			ReleaseDC(pData->hwnd, hdc);
		}
		Sleep(300);
	}
	return 0L;
}

void	boxfillDDB(HDC hdc,int x1,int y1,int x2,int y2,COLORREF col)
{
	HPEN	np,op;
	POINT	pt;
	LOGBRUSH bt;
	HBRUSH	nb,ob;
	
	np=CreatePen(PS_SOLID,0,col);
	op=SelectObject(hdc,np);
	
	bt.lbStyle=BS_SOLID;
	bt.lbColor=col;
	bt.lbHatch=NULL;
	nb=CreateBrushIndirect(&bt);
	
	ob=SelectObject(hdc,nb);
	
	Rectangle(hdc,x1,y1,x2,y2);
	
	SelectObject(hdc,ob);
	DeleteObject(nb);
	
	SelectObject(hdc,op);
	DeleteObject(np);
}

/*	WM_PAINT(PAINTイベント)	*/
int	wproc_paint(HWND hwnd,int wParam,long lParam)
{
	HDC hdc;
	HDC hdc_mem;
	PAINTSTRUCT ps;
	
	RECT	rc;
	
	int	CBy;
	
#ifdef	WINCE
	/*	コマンドバーの高さを得る。コマンドバーはクライアント領域に
		被さっているので、これを基点にしないと書いても表示されない！ */
	CBy=CommandBar_Height(hwndCB);
#else
	CBy=0;		/* CE以外ならこゆことはない */
#endif
	
	hdc = BeginPaint(hwnd, &ps);
	
	/* 背景を塗りつぶす */
	GetClientRect(hwnd,&rc);
	boxfillDDB(hdc,0,0,rc.right-rc.left,rc.bottom-rc.top,ColorBack);
	
	if(Clockf!=FALSE){
		Dispclock(hwnd,hdc,VFontname,VFontpix,ColorBack,ColorClock);
	}
	
	/* カレンダー表示は関数内に表示チェックがある */
	Redrow_Calender(hdc);
	
	EndPaint(hwnd, &ps);
	
	return 0;
}

/*	マウス移動	*/
int	wproc_mousemove(HWND hwnd,int wParam,UINT xpos,UINT ypos)
{
	return 0;
}

/*	マウスアップ	*/
int	wproc_mouseup(HWND hwnd,int wParam,UINT xpos,UINT ypos)
{
	return 0;
}

/*	ウィンドウ内にポップアップメニューを表示する	*/
void	PopupDisp(HWND hwnd,HMENU hmnu,UINT xp,UINT yp)
{
	RECT	rect1,rect2;
	int	flame,title;
	
	GetWindowRect(hwnd,&rect1);	/* ウィンドウサイズを得る */
	GetClientRect(hwnd,&rect2);	/* クライアントサイズを得る */
	title=rect1.bottom-rect1.top-rect2.bottom;	/* ﾀｲﾄﾙﾊﾞｰ分のサイズ */
	flame=rect1.right-rect1.left-rect2.right;	/* 枠分のサイズ */
	
	xp+=flame+rect1.left;
	yp+=title+rect1.top;
	TrackPopupMenu(hmnu,TPM_LEFTALIGN, xp,yp,0,hwnd,NULL);
}

/*	マウスダウン	*/
int	wproc_mousedown(HWND hwnd,int wParam,UINT xpos,UINT ypos)
{
	if(wParam==MK_RBUTTON){
		/* 右ボタン時のみポップアップ表示 */
		PopupDisp(hwnd,hDMenu,xpos,ypos);
	}
	return 0;
}

/*	マウスダブルクリック	*/
int	wproc_mousedbl(HWND hwnd,int wParam,UINT xpos,UINT ypos)
{
	ShellExecute(hwnd,
		NULL,
		ExecPrg,
		NULL,
		NULL,
		SW_SHOWNA
	);
	return 0;
}

LRESULT CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
	int	wmId;
	TCHAR	edit_str[100];
	int	tcount;
	TCHAR	outstr[200];
	SYSTEMTIME st;

	switch (message)
	{
	case WM_INITDIALOG:
		if (bTimer == TRUE) {
			MessageBox(hDlg, _T("タイマ稼働中です。"), _T("警告"), MB_OK + MB_ICONWARNING);
		}
		return TRUE;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		switch (wmId) {
		case IDOK:
			// テキストボックスから得る
			GetDlgItemText(hDlg,
				IDC_EDIT1,
				(LPTSTR)edit_str,
				sizeof(edit_str));
			// 文字を数値化
			tcount=(int)_tcstol(edit_str, 0, 10);
			if (tcount < 1) {
				MessageBox(hDlg, _T("値が不明です"), _T("Timer"), MB_OK + MB_ICONERROR);
			} else {
				// 値が指定されていればセット
				_stprintf(outstr, _T("%d分のタイマを設定しました"), tcount);
				MessageBox(hDlg, outstr, _T("Timer"), MB_OK + MB_ICONINFORMATION);
				lnTimer = (long)(tcount) * 60;

				// 現在時刻を得る
				GetLocalTime(&st);
				// 現在秒を保存
				bkTimCnt = st.wSecond;

				bTimer = TRUE;

				EndDialog(hDlg, IDOK);
				return TRUE;
			}
			break;
		case IDCANCEL:
			EndDialog(hDlg, IDOK);
			return TRUE;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, IDOK);
		return TRUE;
	}
	return FALSE;
}

LRESULT CALLBACK DialogArmProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int	wmId;

	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		switch (wmId) {
		case IDOK:
			EndDialog(hDlg, IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, IDOK);
			return TRUE;
		}
		break;
	default:
		break;
	}
	return FALSE;
}

/*	WM_COMMAND	*/
int	wproc_command(HWND hwnd,int wParam,long lParam)
{
	int wmId, wmEvent,mnu;
	int	idx=0;
	TCHAR	*drops;
	LONG	lmode;
	
	wmId    = LOWORD(wParam); 
	wmEvent = HIWORD(wParam); 
	// メニュー選択の解析 :
	switch (wmId)
	{
	case MNU_EXIT:
		wproc_close(hwnd,0,0);
		break;
	case MNU_CLOCK:
		if(Clockf==FALSE){
			Clockf=TRUE;
		} else if(Calendf!=FALSE){
			Clockf=FALSE;
		}
		data.dspf=Clockf;
		Resize_Calender(hwnd);		/* ウィンドウをリサイズ */
		break;
	case MNU_CALENDER:
		if(Calendf==FALSE){
			Calendf=TRUE;
		} else if(Clockf!=FALSE){
			Calendf=FALSE;
		}
		Resize_Calender(hwnd);		/* ウィンドウをリサイズ */
		break;
#if 0
	// このメニューは廃止
//	case MNU_PRG:				/* 登録ソフト再検索 */
//		Dropfree();			/* 一度解放して */
//		Dropinit();			/* 再設定	*/
//		break;
#endif
	case MNU_RESTART:
		Pbs_exitwin(hwnd,1);
		break;
	case MNU_REBOOT:
		Pbs_exitwin(hwnd,2);
		break;
	case MNU_WINEXIT:
		Pbs_exitwin(hwnd,0);
		break;
	case MNU_REPOS:
		/* ウィンドウリサイズを呼んで位置を再設定	*/
		Resize_Calender(hwnd);
		break;
	case IDM_ABOUT:
		About_dig(hwnd);
		break;
	case MNU_SETUP:		// Setup
		// notepadでiniファイルを開く。notepadがないってことは普通ないよね…？
		Pbs_cmdexec(hwnd, _T("notepad"), Inifile);
		break;
	case ID_OPTION_TIMER:	// タイマー設定
		DialogBox(hInst, (LPTSTR)IDD_DIALOG1, hwnd, (DLGPROC)DialogProc);
		break;
	case ID_OPTION_ONE:
		DialogBox(hInst, (LPTSTR)IDD_WC_ALARM, hwnd, (DLGPROC)DialogArmProc);
		break;
	default:
		if(MNU_POP<=wmId && wmId<MNU_ITEM){
			mnu=wmId-MNU_POP;
			if(Dropuse==TRUE){
				/* D&Dで起動された場合 */
#if 1
				DropExec(hwnd, mnu, Dropfiles);
#else
			//	drops=(TCHAR *)LocalLock(hDrops);
			//	
			//	DropExec(hwnd,mnu,drops);
			//	
			//	/* メモリ解放 */
			//	LocalUnlock(hDrops);
			//	LocalFree(hDrops);
#endif
				Dropuse=FALSE;
			} else {
				/* クリックで起動された場合 */
				DropExec(hwnd,mnu,NULL);
			}
		} else if(MNU_ITEM<wmId){
			mnu=wmId-MNU_ITEM-1;
			LanchExec(hwnd,mnu);
		}
		break;
	}
	return 0;
}

/*	WM_KEYDOWN	*/
int	wproc_keydown(HWND hwnd,int wParam,long lParam)
{
	return 0;
}

#if 1
/*	Drag & Dropイベント	*/
int	wproc_dropfile(HWND hwnd, HDROP hDrop, long lParam)
{
	POINT	pt;
	int	dmax, i;
	TCHAR	tfile[1024];

	// フラグセット
	Dropuse = TRUE;

	// D&Dされた位置を取得
	DragQueryPoint(hDrop, &pt);

	dmax = DragQueryFile(hDrop, -1, NULL, 0);	/* D&Dされたファイル数 */

	// D&Dされたファイルを得る(0番目)
	DragQueryFile(hDrop, 0, tfile, 1024);
	// 「"」でファイル名を囲む
	_tcscpy(Dropfiles, _T("\""));
	_tcscat(Dropfiles, tfile);
	_tcscat(Dropfiles, _T("\""));

	for (i = 1; i < dmax; i++) {
		// D&Dされたファイルを得る(n番目)
		DragQueryFile(hDrop, i, tfile, 1024);
		// 「"」で囲ったファイル名をスペースでつなげる
		_tcscat(Dropfiles, _T(" \""));
		_tcscat(Dropfiles, tfile);
		_tcscat(Dropfiles, _T("\""));
	}

	PopupDisp(hwnd, hDMenu, pt.x, pt.y);	/* ポップアップ表示 */

	DragFinish(hDrop);
	return 0;
}
#else
/*	Drag & Dropイベント	*/(こちらは廃止)
int	wproc_dropfile(HWND hwnd,HDROP hDrop,long lParam)
{
	int	dmax,i;
	long	sz;
	HLOCAL	hloc;
	LPTSTR	drops;
	TCHAR	tfile[1024];
	POINT	pt;
	
	DragQueryPoint(hDrop,&pt);
	dmax=DragQueryFile(hDrop,-1,NULL,0);	/* D&Dされたファイル数 */
	
	sz=0;	/* ファイル名のサイズを得る */
	for(i=0;i<dmax;i++){
		sz+=DragQueryFile(hDrop,i,NULL,0);
		sz+=(sizeof(TCHAR)*3);
	}
	
	hDrops=LocalAlloc(LHND,sz);	/* LocalAllocしてメモリ確保 */
	Dropuse=TRUE;
	drops=(LPTSTR)LocalLock(hDrops);

	/* D&Dされたファイル名を得る(0番目) */
	DragQueryFile(hDrop,0,tfile,sz);
	_tcscpy(drops,_T("\""));
	_tcscat(drops,tfile);
	
	for(i=1;i<dmax;i++){
		_tcscat(drops,_T("\" \""));	/* 空白で繋げる */
		
		/* D&Dされたファイル名を得る */
		DragQueryFile(hDrop,i,tfile,1024);
		_tcscat(drops,tfile);
	}
	_tcscat(drops,_T("\""));
	
	LocalUnlock(hDrops);			/* メモリ解除 */
	
	PopupDisp(hwnd,hDMenu,pt.x,pt.y);	/* ポップアップ表示 */
	
	DragFinish(hDrop);
	return 0;
}
#endif

/*	メニューハンドルとIDを指定してチェックのON/OFFを行う	*/
/*	WinWMの場合は左右別のメニューハンドルの指定が必要	*/
void	pbs_MenuCheckd(HMENU hmnu, UINT item, UINT mode)
{
	if(mode!=0){
		CheckMenuItem(hmnu,item,MF_BYCOMMAND|MF_CHECKED);
	} else {
		CheckMenuItem(hmnu,item,MF_BYCOMMAND|MF_UNCHECKED);
	}
}

/*	カレンダー、時計の表示状態に応じてメニューのチェックを設定	*/
void	CalendCheckmenu(HMENU hmnu)
{
	pbs_MenuCheckd(hmnu, MNU_CLOCK, 0);
	pbs_MenuCheckd(hmnu, MNU_CALENDER, 0);

	if(Clockf!=FALSE){
		pbs_MenuCheckd(hmnu, MNU_CLOCK, 1);
	}
	if(Calendf!=FALSE){
		pbs_MenuCheckd(hmnu, MNU_CALENDER, 1);
	}
}

/*	メニュー初期化後	*/
void	wproc_menuinit(HWND hwnd,HMENU hmenu)
{
	CalendCheckmenu(hmenu);
	Lanchinit(hwnd,hmenu);
}

//	タイトルバーに表示するプログラム名。リソースから展開できるようにインスタンスも渡す。
void	GetAppName(LPTSTR pappname, HINSTANCE hInstance, int size)
{
	LoadString(hInstance, IDS_APP_TITLE, pappname, size);
#if _WIN64
	// x64でのコンパイルの場合
	_tcscat(pappname, _T("(x64)"));
#else
	// x86でのコンパイルの場合
	_tcscat(pappname, _T("(x86)"));
#endif // _WIN64
}

//	リソースからプロダクトバージョン文字列を読みだす
BOOL	GetVersionResoce(LPTSTR namestr, LPTSTR verstr, LPTSTR copystr, LPTSTR compstr, HINSTANCE hInstance)
{
	TCHAR	path[256];
	DWORD dwZero = 0;
	DWORD dwVerInfoSize;
	HLOCAL	hblock;
	unsigned char* pBlock;

	//バージョンを取得する為のバッファ
	void* pvVersion;
	UINT VersionLen;

	// 出力初期化
	_tcscpy(namestr, _T(""));
	_tcscpy(verstr, _T(""));
	_tcscpy(copystr, _T(""));
	_tcscpy(compstr, _T(""));

	/* 実行ファイルのあるパスを得る */
	GetModuleFileName(hInstance, path, 255);

	dwVerInfoSize = GetFileVersionInfoSize(path, &dwZero);
	if (dwVerInfoSize == 0) {
		// 取得不能
		return FALSE;
	}

	hblock = LocalAlloc(LHND, sizeof(unsigned char) * dwVerInfoSize);
	if (hblock == NULL) {		/* 確保できなかった */
		return FALSE;
	}
	pBlock = (unsigned char*)LocalLock(hblock);
	if (pBlock == NULL) {
		// ロックできなかった
		return FALSE;
	}

	// データ取得
	GetFileVersionInfoW(path, dwZero, dwVerInfoSize, pBlock);

	// プログラム名：szAppNameを使う。
	_tcscpy(namestr, szAppName);

	//プロダクトバージョン
	if (VerQueryValue(pBlock, TEXT("\\StringFileInfo\\041104b0\\ProductVersion"), &pvVersion, &VersionLen)) {
		_tcscpy(verstr, (LPTSTR)pvVersion);
	}

	// コピーライト
	if (VerQueryValue(pBlock, TEXT("\\StringFileInfo\\041104b0\\LegalCopyright"), &pvVersion, &VersionLen)) {
		_tcscpy(copystr, (LPTSTR)pvVersion);
	}

	// 会社名
	if (VerQueryValue(pBlock, TEXT("\\StringFileInfo\\041104b0\\CompanyName"), &pvVersion, &VersionLen)) {
		_tcscpy(compstr, (LPTSTR)pvVersion);
	}

	// メモリ開放
	LocalUnlock(hblock);
	LocalFree(hblock);

	return TRUE;
}