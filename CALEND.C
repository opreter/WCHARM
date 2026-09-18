#include	<windows.h>
#include	<tchar.h>
#include	<stdio.h>
#include	<stdlib.h>

//#if defined(_MSC_VER)
#include	<shlobj.h>
//#endif // _MSC_VER

#include	"pbscmd32.h"
#include	"pbsdir32.h"
#include	"calend.h"
#include	"lanch.h"

#define	WCHARM	_T("Charmy")

TCHAR	VFontname[128];	/* フォント名 */
UINT	VFontsize;	/* フォントサイズ(ポイント単位) */
UINT	VFontpix;	/* フォントサイズ(ピクセル単位) */

UINT	FontX,FontY;	/* 縦横フォントサイズ */

COLORREF	ColorBack;	/* バックカラー */
COLORREF	ColorClock;	/* 時計 */
COLORREF	ColorNorm;	/* 平日 */
COLORREF	ColorSun;	/* 日曜日 */
COLORREF	ColorHol;	/* 祝日 */
COLORREF	ColorSat;	/* 土曜日(週休２日制用) */
COLORREF	ColorYear;	/* 年、月 */

BOOL	Calendf;		/* カレンダー表示フラグ */
BOOL	Clockf;			/* 時計表示フラグ */
BOOL	Treef;			/* ３ヶ月カレンダーフラグ */
BOOL	Tatef;			/* 縦横表示フラグ(Trueで縦) */

TCHAR	windir[1024];		/* iniファイルパス */
TCHAR	Inifile[1024];		/* iniファイル名 */
TCHAR	ExecPrg[1024];

TCHAR	holyfile[1024];
UINT	hordaymax;		/* 休日バッファ最大数	*/
UINT	horday_y[200];		/* 休日バッファ　年	*/
UINT	horday_m[200];		/* 　　〃　　　　月	*/
UINT	horday_d[200];		/* 　　〃　　　　日	*/

TCHAR	StartupDir[1024];		// スタートアップフォルダ

BOOL	bTimer;			// カウントダウンタイマ起動中
unsigned long lnTimer;		// カウントダウンタイマ。秒単位
int	bkTimCnt;		// 前回の秒

void	ReadProfile(void);
void	WriteProfile(void);
int	isHoryday(int yy, int mon, int dd);
long	Horyday_load(LPTSTR fname);

/*	一月の最大日数	*/
static	int	monmax[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};

/*	閏年チェック	*/
int	chk_leap_year(int yy)
{
	int	fg;

	if((yy % 4) == 0){
		fg = -1;
		if((yy % 100) == 100){
			fg = 0;
			if((yy % 400) == 0){
				fg = -1;
			}
		}
	} else {
		fg = 0;
	}
	return fg;
}

/*	月の初めが何曜日かを得る	*/
int	monstart(int yy,int mon)
{
	int	dweek,y,m;
	
	dweek = 2;
	for(y=1980;y<yy;y++){
		if(chk_leap_year(y)!=0){
			dweek += 366;
		} else {
			dweek += 365;
		}
		dweek %= 7;
	}

	for(m=1;m<mon;m++){
		dweek += monmax[m];
	}
	dweek %= 7;

	return dweek;
}

int	access32(LPTSTR file)
{
	HANDLE	hdir;
	WIN32_FIND_DATA findstr;
	
	hdir=FindFirstFile(file,&findstr);
	if(hdir == INVALID_HANDLE_VALUE)return 1;
	
	FindClose(hdir);
	
	return 0;
}

int	access_pathT(LPTSTR path, LPTSTR file)
{
	if (path[_tcslen(path) - 1] != _T('\\')) {
		/* 末尾が\じゃないなら付け加える */
		_tcscat(path, _T("\\"));
	}
	_tcscat(path, file);		/* フルパス生成	*/

	/* ファイル存在確認	*/
	return access32(path);
}

/*	ファイルを探す(カレント→環境変数→HOME→マイドキュメント→argv[0])	*/
int	get_inipathT(LPTSTR fname, LPTSTR path, LPTSTR envstr, LPTSTR argv0)
{
	TCHAR	curdir[1024];
	DWORD	retd;

	/* カレントパスで確認	*/
	GetCurrentDirectory(1024, curdir);
	_tcscpy(path, curdir);
	if (access_pathT(path, fname) == 0)return 0;

	/* 環境変数を得る	*/
	retd = GetEnvironmentVariable(envstr, curdir, 1024);
	if (retd != 0) {
		_tcscpy(path, curdir);
		if (access_pathT(path, fname) == 0)return 0;
	}

	/* 環境変数「HOME」を探す	*/
	retd = GetEnvironmentVariable(_T("HOME"), curdir, 1024);
	if (retd != 0) {
		_tcscpy(path, curdir);
		if (access_pathT(path, fname) == 0)return 0;
	}

	/* マイドキュメントのパス	*/
	if (SHGetSpecialFolderPath(NULL, curdir, CSIDL_PERSONAL, 0) != FALSE) {
		_tcscpy(path, curdir);
		if (access_pathT(path, fname) == 0)return 0;
	}

	_tcscpy(path, argv0);			/* argv[0]のパスから探す */
	if (access_pathT(path, fname) == 0)return 0;

	/* 結局見つからなかった	*/
	return 1;
}

/*	カレンダーデータ初期化	*/
void	Calender_ini(LPTSTR path)
{
	TCHAR	wpath[1024];
	TCHAR	argv0[1024];
	char	c1;

	Calendf=TRUE;				/* カレンダー表示 */
	Clockf=TRUE;				/* 時計表示 */
	Treef=TRUE;				/* ３ヶ月カレンダー */
	Tatef=TRUE;				/* 縦表示 */
	
	// ColorBack = RGB(0, 0, 0);		/* バック */
	ColorBack = RGB(0xff, 0xff, 0xff);	/* バック */
	// ColorClock = RGB(0, 0xFF, 0xFF);	/* 時計 */
	ColorClock = RGB(0, 0, 0);		/* 時計 */
	// ColorNorm = RGB(0, 0xFF, 0xFF);		/* 平日 */
	ColorNorm = RGB(0, 0, 0);		/* 平日 */
	ColorSun = RGB(0xFF, 0, 0);		/* 日曜日 */
	ColorHol = RGB(0xFF, 0, 0);		/* 祝日 */
	ColorSat = RGB(0xFF, 0, 0);		/* 土曜日(週休２日制用) */
	// ColorYear = RGB(0, 0xFF, 0xFF);		/* 年、月 */
	ColorYear = RGB(0, 0, 0);		/* 年、月 */

	// フォント名設定
	_tcscpy(VFontname, _T("ＭＳ ゴシック"));
	VFontsize = 14;

	/* 実行ファイルのあるパス */
	_splitpathT(path, Ddr, Ddir, Dname, Dext);
	_makepathT(argv0, Ddr, Ddir, _T(""), _T(""));
	_tcscpy(windir, argv0);
	
	/* ダブルクリックで起動するアプリ */
	_tcscpy(ExecPrg,_T("calc.exe"));
	
	/* D&D用ショートカットのあるディレクトリ */
	_makepathT(DropDir, Ddr, Ddir, _T("drop"), _T(""));

	/* メニュー実行用ショートカットのあるディレクトリ */
	_makepathT(BootDir, Ddr, Ddir, _T("boot"), _T(""));

	// 祝日ファイルはデフォルトでは未設定
	_tcscpy(holyfile, _T(""));

	// スタートアップはデフォルトでは未設定
	_tcscpy(StartupDir, _T(""));

//	/* iniファイル名 */
//	_makepath(Inifile,_T(""),windir,_T("wcharm"),_T("ini"));
//	c1=access32(Inifile);

#if defined(_MSC_VER)
	/* iniファイル検索	*/
	c1 = get_inipathT(_T("wcharm.ini"), wpath, _T("CHARM"), argv0);
	_tcscpy(Inifile, wpath);
#else
	/* iniファイル検索	*/
	c1=get_inipath("wcharm.ini",wpath,"CHARM",argv0);
	strcpy(Inifile, wpath);
#endif

	if(c1!=0){
		/* 見つからない：iniファイル名をargv[0]から再生成	*/
		_makepathT(Inifile,_T(""), argv0,_T("wcharm"),_T("ini"));
		WriteProfile();		/* iniファイル書き出し */
	} else {
		/* 見つかった	*/

		/* 最終的にiniファイルのあるパスが基準パス(argv0ではない)	*/
		_splitpathT(Inifile, Ddr, Ddir, Dname, Dext);
		_makepathT(windir, Ddr, Ddir, _T(""), _T(""));
		ReadProfile();		/* iniファイル読み込み */
	}

	/* 祝日CSVを読み込む	*/
	Horyday_load(holyfile);
	
	/* D&D起動用メニュー初期化 */
	Dropinit();
}

/*	１日分のカレンダーを描く	*/
void	dispcalstr(HDC hdc,LPTSTR dstr,COLORREF col,int cx,int cy)
{
	SetTextColor(hdc,col);
	TextOut(hdc,cx,cy,dstr,_tcslen(dstr));
}

/*	１ヶ月分のカレンダーを表示	*/
void	Display_Calender(HDC hdc,int yy,int mo,int dd,int cx,int cy)
{
	int	x,y,stweek,dcnt,zerop,dy;
	COLORREF	dcol;
	TCHAR	dstr[40];
	TCHAR	cflg;
	int	dx;
	int	dateh[6][7];
	int	wareki;

	if(chk_leap_year(yy)!=0){
		monmax[2]=29;
	} else {
		monmax[2]=28;
	}

	for(y=0;y<6;y++){
		for(x=0;x<7;x++){
			dateh[y][x]=0;
		}
	}

	y=0;
	x=monstart(yy,mo);
	for(dcnt=1;dcnt<=monmax[mo];dcnt++){
		dateh[y][x]=dcnt;
		x=x+1;
		if(x>6){
			x=0;
			y=y+1;
		}
	}

	if(yy>2019 || (yy==2019 && mo > 4)){
		/* 2019年5月以降は令和	*/
		_stprintf(dstr,_T("%4d(R%d) -%d-"),yy,(yy-2018),mo);
	} else {
		/* 2019年5月以前は平成	*/
		_stprintf(dstr,_T("%4d(H%d) -%d-"),yy,(yy-1988),mo);
	}
	dispcalstr(hdc,dstr,ColorYear,cx,cy);
	cy+=FontY;
	
	for(y=0;y<6;y++){
		dx=0;
		for(x=0;x<7;x++){
			if(x==0){
				dcol=ColorSun;
			} else if(x==6){
				dcol=ColorSat;
			} else if(isHoryday(yy,mo,dateh[y][x])){
				dcol=ColorHol;
			} else {
				dcol=ColorNorm;
			}
			dstr[0]=0;
			if(dd==dateh[y][x] && dd!=0){
				cflg=_T('>');
			} else {
				cflg=_T(' ');
			}
			if(dateh[y][x]!=0){
				_stprintf(dstr,_T("%c%2d"),cflg,dateh[y][x]);
			} else {
				_stprintf(dstr,_T("   "));
			}
			dispcalstr(hdc,dstr,dcol,cx+dx,cy);
			//TextOut(hdc,cx+dx,cy,cline,strlen(cline));
			dx+=(FontX*3);
		}
		cy+=FontY;
	}
}

/*	時計表示(スレッド内でも呼ばれるのでグローバル変数は駄目だぞ！)	*/
void	Dispclock(HWND hwnd,HDC hdc,LPTSTR fontname,int fontsize,COLORREF bcl,COLORREF fcl)
{
	TCHAR*	str_org = _T("%4d/%02d/%02d %02d:%02d:%02d");
	TCHAR	str[256];
	SYSTEMTIME st;

	HFONT	hfont,hofont;

	GetLocalTime(&st);
	_stprintf(str, str_org, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	/* フォント設定 */
	hfont = CreateFont_VB(fontname,fontsize,0,0,0,0);
	hofont=SelectObject(hdc,hfont);

	SetBkColor(hdc,bcl);
	SetTextColor(hdc,fcl);
	TextOut(hdc, 0, 0, str, _tcslen(str));

#if 0
	アラーム表示
	if(st.wHour==1 && st.wMinute==35){
		MessageBox(hwnd,_T("アラーム時刻です！"), _T("アラーム"), (MB_OK + MB_ICONEXCLAMATION));
	}
#endif
	if (bTimer == TRUE) {
		// ダウンタイマ稼働中
		if (bkTimCnt != st.wSecond) {
			// 秒が変化した

			// 現在秒を保存
			bkTimCnt = st.wSecond;

			// カウントダウン
			lnTimer--;
			if (lnTimer < 1) {
				// カウントアウト 
				bTimer = FALSE;

				// 最前面表示の設定
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW));

				MessageBox(hwnd, _T("アラーム時刻です！"), _T("アラーム"), (MB_OK + MB_ICONEXCLAMATION));

				// 最前面表示の解除
				SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW));
			}
		}
	}

	/* フォント戻し */
	SelectObject(hdc,hofont);
	DeleteObject(hfont);
}

/*	カレンダー表示	*/
void	Redrow_Calender(HDC hdc)
{
	int	xsize,ysize;
	HFONT	hfont,hofont;
	int	yy,mon,dd;
	int	clktop;
	int	ty,tm;

	int	dx,dy;
	SYSTEMTIME st;

	if(Calendf == FALSE)return;	/* カレンダー表示しないなら抜ける */

	GetLocalTime(&st);

	SetBkColor(hdc,ColorBack);

	yy=st.wYear;
	mon=st.wMonth;
	dd=st.wDay;

	/* フォント設定 */
	hfont = CreateFont_VB(VFontname,VFontpix,0,0,0,0);
	hofont=SelectObject(hdc,hfont);

	dx=dy=0;

	if(Clockf!=FALSE){
		clktop=2;	/* 時計表示する */
	} else {
		clktop=0;	/* 時計表示しない */
	}

	dy=clktop*FontY;

	if(Treef!=FALSE){
		/* 先月 */
		ty=yy;
		tm=mon-1;
		
		if(tm < 1){
			tm = 12;
			ty = yy - 1;
		}
		Display_Calender(hdc,ty,tm,0,dx,dy);
		if(Tatef == TRUE){
			dy= (8 + clktop) * FontY;	/* 縦表記 */
		} else {
			dx = ((7 * 3) + 1) * FontX;	/* 横表記 */
			dy = clktop * FontY;
		}
	}
	Display_Calender(hdc,yy,mon,dd,dx,dy);	/* 当月 */
	if(Treef!=FALSE){
		/* 来月 */
		if(Tatef == TRUE){
			dy=(8 * 2 + clktop) * FontY;		/* 縦表記 */
		} else {
			dx = (((7 * 3) + 1) * 2) * FontX;	/* 横表記 */
			dy = clktop * FontY;
		}
		ty = yy;
		tm = mon + 1;
		if(tm > 12){
			tm = 1;
			ty = yy + 1;
		}
		Display_Calender(hdc,ty,tm,0,dx,dy);
	}

	SelectObject(hdc,hofont);
	DeleteObject(hfont);
}

extern	void	CalendCheckmenu(HMENU hmnu);

/*	ウィンドウをカレンダーのサイズにリサイズ	*/
void	Resize_Calender(HWND hwnd)
{
	RECT	rect1,rect2;
	int	flame,title;

	HWND	hwnd_desk;
	RECT	scr;

	int	xsize,ysize,clktop;
	int	x1,y1;

	HMENU	hmenu;

	GetWindowRect(hwnd,&rect1);	/* ウィンドウサイズを得る */
	GetClientRect(hwnd,&rect2);	/* クライアントサイズを得る */

	title=rect1.bottom-rect1.top-rect2.bottom;	/* ﾀｲﾄﾙﾊﾞｰ分のサイズ */
	flame=rect1.right-rect1.left-rect2.right;	/* 枠分のサイズ */

	hmenu=GetMenu(hwnd);	/* メニューハンドルを得る	*/
	CalendCheckmenu(hmenu);	/* メニューのチェックを付け直す	*/

	if(Clockf!=FALSE){
		clktop=2;	/* 時計表示する */
	} else {
		clktop=0;	/* 時計表示しない */
	}

	if(Calendf == FALSE){
		if(Clockf!=FALSE){
			/* 時計表示あり、カレンダー表示無し */
			ysize = clktop - 1;
			xsize = 7 * 3;
		} else {
			/* 時計表示、カレンダー表示、共に無し */
			xsize=ysize=0;
		}
	} else {
		if(Treef == FALSE){
			/* １ヶ月だけ */
			ysize = 7 + clktop;
			xsize = 7 * 3;
		} else {
			if(Tatef == TRUE){
				/* ３ヶ月分縦表示 */
				ysize=8*3-1+clktop;
				xsize=7*3;
			} else {
				/* ３ヶ月横表示 */
				ysize = 7 + clktop;
				xsize = (7 * 3 + 1) * 3 - 1;
			}
		}
	}
	
	hwnd_desk=GetDesktopWindow();	/* デスクトップのサイズを得る */
	GetWindowRect(hwnd_desk,&scr);
	
	/* 右上 */
	x1=((scr.right-scr.left)-((xsize*FontX)+flame));
	y1=0;
	
	/* ウィンドウサイズを再設定 */
	SetWindowPos(hwnd,0,x1,y1,(xsize*FontX)+flame,(ysize*FontY)+title,0);
}

//	COLORREF値をiniファイルから読む(項目がない場合0xFFFFFFFFを返す)
COLORREF	GetPrivateProfileColor(LPTSTR lpAppName, LPTSTR lpKeyName, LPTSTR lpFileName)
{
	TCHAR	wkstr[256];
	unsigned long rgb;
	COLORREF xcol;

	// iniファイルから文字列読み込み
	GetPrivateProfileString(lpAppName, lpKeyName, _T(""), wkstr, 256, lpFileName);
	if (_tcslen(wkstr) < 1) {
		return (COLORREF)(0xFFFFFFFF);
	}
	
	// 32bit値として変換
	rgb = (unsigned long)_tcstol(wkstr, NULL, 16);

	// 下位24bitが有効
	rgb &= 0xFFFFFF;

	// COLORREFとしてしまう
	xcol = (COLORREF)rgb;

	// COLORREFとして返す
	return xcol;
}

/*	iniファイル読み込み	*/
void	ReadProfile()
{
	TCHAR	str[1024];
	int	l;
	int	xflg, xflg2;
	COLORREF	wcol;

	/* 相対パスフラグ	*/
	xflg = 0;
	GetPrivateProfileString(WCHARM, _T("PathFlg"), _T("0"), str, 256, Inifile);
	xflg = _tcstol(str, 0, 10);

	/* Itemメニュー内容のディレクトリパス	*/
	GetPrivateProfileString(WCHARM, _T("BootPath"), _T(""), str, 256, Inifile);
	l = _tcslen(str);
	if (0 < l) {
		if (xflg != 0) {
			/* 実行ファイルからの相対指定	*/
			_makepathT(BootDir, _T(""), windir, str, _T(""));
		}
		else {
			/* 絶対パス指定	*/
			_tcscpy(BootDir, str);
		}
	}

	/* D&Dメニュー内容のディレクトリパス	*/
	GetPrivateProfileString(WCHARM, _T("DropPath"), _T(""), str, 256, Inifile);
	l = _tcslen(str);
	if (0 < l) {
		if (xflg != 0) {
			/* 実行ファイルからの相対指定	*/
			_makepathT(DropDir, _T(""), windir, str, _T(""));
		}
		else {
			/* 絶対パス指定	*/
			_tcscpy(DropDir, str);
		}
	}

	// 祝日ファイル
	GetPrivateProfileString(WCHARM, _T("Holyfile"), _T(""), str, 256, Inifile);
	l = _tcslen(str);
	if (0 < l) {
		if (xflg != 0) {
			/* 実行ファイルからの相対指定	*/
			_makepathT(holyfile, _T(""), windir, str, _T(""));
		}
		else {
			/* 絶対パス指定	*/
			_tcscpy(holyfile, str);
		}
	}

	// スタートアップ
	GetPrivateProfileString(WCHARM, _T("Startup"), _T(""), str, 256, Inifile);
	l = _tcslen(str);
	if (0 < l) {
		if (xflg != 0) {
			/* 実行ファイルからの相対指定	*/
			_makepathT(StartupDir, _T(""), windir, str, _T(""));
		}
		else {
			/* 絶対パス指定	*/
			_tcscpy(StartupDir, str);
		}
	}

	/* 相対パスフラグその２	*/
	xflg2 = 0;
	GetPrivateProfileString(WCHARM, _T("LeftPathFlg"), _T("0"), str, 256, Inifile);
	xflg2 = (int)(_ttoi(str));

	/* ダブルクリック実行ファイル	*/
	GetPrivateProfileString(WCHARM, _T("LeftPrg"), _T(""), str, 256, Inifile);
	l = _tcslen(str);
	if (0 < l) {
		if (xflg2 != 0) {
			/* 実行ファイルからの相対指定	*/
			_makepathT(ExecPrg, _T(""), windir, str, _T(""));
		}
		else {
			/* 絶対パス指定	*/
			_tcscpy(ExecPrg, str);
		}
	}
#if 0
	//	'ウィンドウ位置
	//	Posmode = Val(GetProfStr$("Charmy", "WinPos", Inifile$))
	//	'カレンダー表示
	//	d = Val(GetProfStr$("Charmy", "CalUse", Inifile$))
	//	If d <> 0 Then Calendf = True Else Calendf = False
	//	'時計表示
	//	d = Val(GetProfStr$("Charmy", "ClockUse", Inifile$))
	//	If d <> 0 Then Clockf = True Else Clockf = False
	//	'時計表示位置
	//	Form1.Label1.Alignment = Val(GetProfStr$("Charmy", "ClockPoint", Inifile$))
	//	'３ヶ月カレンダー
	//	d = Val(GetProfStr$("Charmy", "CalType", Inifile$))
	//	If d <> 0 Then Treef = True Else Treef = False
	//	'縦表示
	//	d = Val(GetProfStr$("Charmy", "CalStyle", Inifile$))
	//	If d <> 0 Then Tatef = True Else Tatef = False
#endif

	// 'バックカラー
	wcol = GetPrivateProfileColor(WCHARM, _T("ColorBack"), Inifile);
	if (((wcol) & 0xFF000000) == 0) {
		ColorBack = wcol;
	}
	// '時計
	wcol = GetPrivateProfileColor(WCHARM, _T("ColorClock"), Inifile);
	if (((wcol) & 0xFF000000) == 0) {
		ColorClock = wcol;
	}
	// '平日
	wcol = GetPrivateProfileColor(WCHARM, _T("ColorNorm"), Inifile);
	if (((wcol) & 0xFF000000) == 0) {
		ColorNorm = wcol;
	}
	// '日曜日
	wcol = GetPrivateProfileColor(WCHARM, _T("ColorSun"), Inifile);
	if (((wcol) & 0xFF000000) == 0) {
		ColorSun = wcol;
	}
	// '祝日
	wcol = GetPrivateProfileColor(WCHARM, _T("ColorHol"), Inifile);
	if (((wcol) & 0xFF000000) == 0) {
		ColorHol = wcol;
	}
	// '土曜日(週休２日制用)
	wcol = GetPrivateProfileColor(WCHARM, _T("ColorSat"), Inifile);
	if (((wcol) & 0xFF000000) == 0) {
		ColorSat = wcol;
	}
	// '年、月
	wcol = GetPrivateProfileColor(WCHARM, _T("ColorYear"), Inifile);
	if (((wcol) & 0xFF000000) == 0) {
		ColorYear = wcol;
	}
}

//	COLORREF値を16進数でiniファイルへ書き込み
void	WritePrivateProfileColor(LPTSTR lpAppName, LPTSTR lpKeyName, COLORREF col, LPTSTR lpFileName)
{
	TCHAR	wkstr[1024];

	_stprintf(wkstr, _T("%02X%02X%02X"), GetBValue(col), GetGValue(col), GetRValue(col));
	WritePrivateProfileString(lpAppName, lpKeyName, wkstr, lpFileName);
}

void	WriteProfile()
{
	TCHAR	wkstr[1024];

	WritePrivateProfileString(WCHARM, _T("DropPath"), DropDir, Inifile);
	WritePrivateProfileString(WCHARM, _T("BootPath"), BootDir, Inifile);
	WritePrivateProfileString(WCHARM, _T("PathFlg"), _T("0"), Inifile);

#if 0
//	'ウィンドウ位置
//	PutProfStr "Charmy", "WinPos", snum$(Posmode), Inifile$
//	'カレンダー表示
//	PutProfStr "Charmy", "CalUse", snum$(Calendf), Inifile$
//	'時計表示
//	PutProfStr "Charmy", "ClockUse", snum$(Clockf), Inifile$
//	'時計表示位置
//	PutProfStr "Charmy", "ClockPoint", snum$(Form1.Label1.Alignment), Inifile$

//	'３ヶ月カレンダー
//	PutProfStr "Charmy", "CalType", snum$(Treef), Inifile$
//	'縦表示
//	PutProfStr "Charmy", "CalStyle", snum$(Tatef), Inifile$
//	'週休二日制
//	'PutProfStr "Charmy", "Weekend", snum$(Weekend), Inifile$
#endif

	// '左右ダブルクリック起動プログラム
	WritePrivateProfileString(WCHARM, _T("LeftPathFlg"), _T("0"), Inifile);
	WritePrivateProfileString(WCHARM, _T("LeftPrg"), ExecPrg, Inifile);
	// PutProfStr "Charmy", "RightPrg", ExecPrg$(1), Inifile$

	// 'バックカラー
	WritePrivateProfileColor(WCHARM, _T("ColorBack"), ColorBack, Inifile);
	// '時計
	WritePrivateProfileColor(WCHARM, _T("ColorClock"), ColorClock, Inifile);
	// '平日
	WritePrivateProfileColor(WCHARM, _T("ColorNorm"), ColorNorm, Inifile);
	// '日曜日
	WritePrivateProfileColor(WCHARM, _T("ColorSun"), ColorSun, Inifile);
	// '祝日
	WritePrivateProfileColor(WCHARM, _T("ColorHol"), ColorHol, Inifile);
	// '土曜日(週休２日制用)
	WritePrivateProfileColor(WCHARM, _T("ColorSat"), ColorSat, Inifile);
	// '年、月
	WritePrivateProfileColor(WCHARM, _T("ColorYear"), ColorYear, Inifile);

	// 祝日ファイル
	WritePrivateProfileString(WCHARM, _T("Holyfile"), holyfile, Inifile);
}

/*	休日か否か	*/
int	isHoryday(int yy, int mon, int dd)
{
	int	i;

	for(i=0;i<hordaymax;i++){
		if(yy==horday_y[i] && mon==horday_m[i] && dd==horday_d[i]){
			return TRUE;
		}
	}

	return FALSE;
}

/*	休日データを読み込む	*/
long	Horyday_load(LPTSTR fname)
{
	TCHAR	st_buff[1024];
	long	lmax;
	int	s1;
	int	ln;
	TCHAR	ms[10];
	TCHAR	ds[10];
	int	yy, mm, dd;
	FILE	*ifp;
	TCHAR	*dp1,*dp2;
	SYSTEMTIME st;

	lmax = hordaymax = 0;

	// 現在日時を得る
	GetLocalTime(&st);

#if 1
	_wfopen_s(&ifp, fname, _T("r"));
#else
	ifp = fopen(fname, "r");
#endif // _MSC_VER
	if(ifp==NULL){
		/* ファイルオープンエラー	*/
		return 1;
	}

	while(feof(ifp)==FALSE){
		/* 行読み込み	*/
		_fgetts(st_buff,1023,ifp);

		dp1=_tcschr(st_buff,'/');
		if(dp1!=NULL){
			/* あった	*/
			dp2=dp1;
			*dp2='\0';
			dp2++;

			// 初期化
			yy = mm = dd = 0;

			/* 年を変換(「/」までになるはず)	*/
			yy = (int)_tcstol(st_buff, NULL, 10);

			/* 月を切り離し	*/
			dp1=_tcschr(dp2,'/');
			if(dp1!=NULL){
				*dp1='\0';
				dp1++;
				_tcscpy(ms,dp2);
				dp2=dp1;
				mm = (int)_tcstol(ms, NULL, 10);
			}

			/* 日を切り離し	*/
			dp1=_tcschr(dp2,',');
			if(dp1!=NULL){
				*dp1='\0';
				dp1++;
				_tcscpy(ds,dp2);
				dp2=dp1;
				dd = (int)_tcstol(ds, NULL, 10);
			}

			if (yy > 0 && mm > 0 && dd > 0 && hordaymax < 199) {
				// 設定がある場合＆バッファカウントOK
				if (yy >= (st.wYear-1)) {
					// 年が去年以降(最低でも去年、今年、来年)
					horday_y[hordaymax]=yy;		/* 休日バッファ　年	*/
					horday_m[hordaymax]=mm;		/* 　　〃　　　　月	*/
					horday_d[hordaymax]=dd;		/* 　　〃　　　　日	*/
					hordaymax++;
					lmax++;
				}
			}

		}
	}

	/* ここでCLOSE	*/
	fclose(ifp);

	return lmax;
}
