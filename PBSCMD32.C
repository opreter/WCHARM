#include	<windows.h>
#include	<commdlg.h>
#include	<tchar.h>
#include	<stdio.h>

#ifdef	WINCE
#include	"tgetfile.h"
#endif

#ifndef	BCC32
#define	OPENFNAME_SIZE	(sizeof( OPENFILENAME ))
#else
//	Win2Kで拡張されたらしいんだわ
#define	OPENFNAME_SIZE	(76)
#endif

static	TCHAR szName[256];

/*	OPENコモンダイアログ	*/
DWORD	common_FileOpen( HWND hWnd ,LPTSTR fname,LPTSTR title,LPTSTR szTemp)
{
	OPENFILENAME ofnTemp;
	DWORD Errval;	// エラー値
	TCHAR Errfmt[]=_T("GetOpenFileName でエラー #%ld が発生しました");
#ifndef	WINCE
	TCHAR Errstr[80]; // エラーバッファ
#endif

	_tcscpy(szName,fname);
	ofnTemp.lStructSize = OPENFNAME_SIZE;		/* 構造体サイズ */
	ofnTemp.hwndOwner = hWnd;			/* 呼び元ウィンドウ */
	ofnTemp.hInstance = 0;
	ofnTemp.lpstrFilter = (LPTSTR)szTemp;		/* ﾜｲﾙﾄﾞｶｰﾄﾞﾌｨﾙﾀ */
	ofnTemp.lpstrCustomFilter = NULL;
	ofnTemp.nMaxCustFilter = 0;
	ofnTemp.nFilterIndex = 1;			/* ﾌｨﾙﾀｲﾝﾃﾞｯｸｽ */
	ofnTemp.lpstrFile = szName;	// 結果は、この変数に格納される
	ofnTemp.nMaxFile = sizeof(szName);
	ofnTemp.lpstrFileTitle = NULL;
	ofnTemp.nMaxFileTitle = 0;
	ofnTemp.lpstrInitialDir = NULL;
	ofnTemp.lpstrTitle = title;	// ダイアログのタイトル
	ofnTemp.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
	ofnTemp.nFileOffset = 0;
	ofnTemp.nFileExtension = 0;
	ofnTemp.lpstrDefExt = _T("*");
	ofnTemp.lCustData = 0;
	ofnTemp.lpfnHook = NULL;
	ofnTemp.lpTemplateName = NULL;

#ifndef	WINCE
	Errval = 1;
	if(GetOpenFileName( &ofnTemp ) != TRUE)
	{
		Errval=CommDlgExtendedError();
		if(Errval!=0)	// 値 0 はユーザがキャンセルした時
		{
			_stprintf(Errstr,Errfmt,Errval);
			MessageBox(hWnd,Errstr,_T("警告"),MB_OK|MB_ICONSTOP);
		}

	}
#else
	/* tGetFile(ぽぽぺ版)を使う */
	Errval=1;
	if(tGetOpenFileName(&ofnTemp)!=TRUE){
		Errval=0;
	}
#endif
	
	_tcscpy(fname,szName);
	
	return Errval;
}

/*	フォントハンドルを得る(VB用)	*/
HFONT	FAR PASCAL CreateFont_VB(LPTSTR name,UINT size,UINT bo,UINT it,UINT str,UINT und)
{

	int	bomode;
	LOGFONT	lfont;
	
	bomode=FW_NORMAL;
	if(bo!=0)bomode=FW_BOLD;
	
	lfont.lfHeight=0-size;
	lfont.lfWidth=0;
	lfont.lfEscapement=0;
	lfont.lfOrientation=0;
	lfont.lfWeight=bomode;
	lfont.lfItalic=it;
	lfont.lfUnderline=und;
	lfont.lfStrikeOut=str;
	lfont.lfCharSet=DEFAULT_CHARSET;
	lfont.lfOutPrecision=OUT_DEFAULT_PRECIS;
	lfont.lfClipPrecision=CLIP_DEFAULT_PRECIS;
	lfont.lfQuality=DEFAULT_QUALITY;
	lfont.lfPitchAndFamily=FIXED_PITCH;
	_tcscpy(lfont.lfFaceName,name);
	
	return CreateFontIndirect(&lfont);
}

#ifndef	WINCE
/*	プリントダイアログ	*/
UINT	FAR PASCAL common_Print(HWND hWnd,HDC FAR *hdc)
{
	PRINTDLG pd ;
	int	r;
	
			// initialize
	memset( &pd, 0, sizeof(PRINTDLG) ) ;
	pd.lStructSize = sizeof(PRINTDLG);
	pd.hwndOwner   = hWnd ;
	pd.Flags       = PD_RETURNDC | PD_NOPAGENUMS | PD_NOSELECTION;
	pd.hInstance   = NULL ;
	
	*hdc=NULL;
	
	r=PrintDlg( &pd );
	if(r==TRUE){
		*hdc = pd.hDC ;
	}
	
	return r;
}

/*	印刷開始(VB用)	*/
UINT	FAR PASCAL StartDoc_VB(HDC hDC,LPSTR docname)
{
	DOCINFO  di ;
	
	di.cbSize      = sizeof(DOCINFO); 
	di.lpszDocName = docname;
	di.lpszOutput  = NULL ; 

	return StartDoc( hDC, &di ) ; 
}

/*	印刷終了(VB用。EndDocはVBの予約語…)	*/
UINT	FAR PASCAL EndDoc_VB(HDC hDC)
{
	return EndDoc(hDC);
}
#endif

#ifdef _MSC_VER
/*	TCHAR文字列(LPTSTR)→char文字列	*/
void	TCHAR2char(char* dst, LPTSTR src, int bufsize)
{
	ZeroMemory(&dst[0], bufsize);
	WideCharToMultiByte(CP_ACP, 0, &src[0], lstrlen(src), &dst[0], bufsize, NULL, NULL);
}

/*	char文字列→TCHAR文字列(LPTSTR)	*/
void	char2TCHAR(LPTSTR dst, char* src, int bufsize)
{
	ZeroMemory(&dst[0], bufsize);
	MultiByteToWideChar(CP_ACP, 0, &src[0], strlen(src), &dst[0], bufsize);
}
#endif // _MSC_VER
