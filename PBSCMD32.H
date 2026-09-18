/*	OPENコモンダイアログ	*/
extern	DWORD	common_FileOpen( HWND hWnd ,LPTSTR fname,LPTSTR title,LPTSTR szTemp);

/*	フォントハンドルを得る(VB用)	*/
extern	HFONT	FAR PASCAL CreateFont_VB(LPTSTR name,UINT size,UINT bo,UINT it,UINT str,UINT und);

/*	プリントダイアログ	*/
extern	UINT	FAR PASCAL common_Print(HWND hWnd,HDC FAR *hdc);
/*	印刷開始(VB用)	*/
extern	UINT	FAR PASCAL StartDoc_VB(HDC hDC,LPSTR docname);
/*	印刷終了(VB用。EndDocはVBの予約語…)	*/
extern	UINT	FAR PASCAL EndDoc_VB(HDC hDC);

#ifdef _MSC_VER
/*	TCHAR文字列(LPTSTR)→char文字列	*/
extern	int	TCHAR2char(char* dst, LPTSTR src, int bufsize);

/*	char文字列→TCHAR文字列(LPTSTR)	*/
extern	void	char2TCHAR(LPTSTR dst, char* src, int bufsize);
#endif
