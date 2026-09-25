extern	TCHAR	VFontname[128];	/* フォント名 */
extern	UINT	VFontsize;	/* フォントサイズ(ポイント単位) */
extern	UINT	VFontpix;	/* フォントサイズ(ピクセル単位) */

extern	UINT	FontX,FontY;

extern	COLORREF	ColorBack;	/* バックカラー */
extern	COLORREF	ColorClock;	/* 時計 */
extern	COLORREF	ColorNorm;	/* 平日 */
extern	COLORREF	ColorSun;	/* 日曜日 */
extern	COLORREF	ColorHol;	/* 祝日 */
extern	COLORREF	ColorSat;	/* 土曜日(週休２日制用) */
extern	COLORREF	ColorYear;	/* 年、月 */

extern	BOOL	Calendf;		/* カレンダー表示フラグ */
extern	BOOL	Clockf;			/* 時計表示フラグ */
extern	BOOL	Treef;			/* ３ヶ月カレンダーフラグ */
extern	BOOL	Tatef;			/* 縦横表示フラグ(Trueで縦) */

extern	TCHAR	windir[1024];		/* iniファイルパス */
extern	TCHAR	Inifile[1024];		/* iniファイル名 */
extern	TCHAR	ExecPrg[1024];

extern	TCHAR	Ddr[20], Ddir[1024], Dname[256], Dext[256];

extern	BOOL	bTimer;			// カウントダウンタイマ起動中
extern	unsigned long lnTimer;		// カウントダウンタイマ。秒単位
extern	int	bkTimCnt;		// 前回の秒

/*	カレンダーデータ初期化	*/
extern	void	Calender_ini(LPTSTR path);
/*	カレンダー表示	*/
extern	void	Redrow_Calender(HDC hdc);
/*	時計表示(スレッド内でも呼ばれるのでグローバル変数は駄目だぞ！)	*/
extern	void	Dispclock(HWND hwnd,HDC hdc,LPTSTR fontname,int fontsize,COLORREF bcl,COLORREF fcl);
/*	ウィンドウをカレンダーのサイズにリサイズ	*/
extern	void	Resize_Calender(HWND hwnd);
//	１回のみのアラームをセット
extern	void	SetOneAlarm(LPTSTR atime);
