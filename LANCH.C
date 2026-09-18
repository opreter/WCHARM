#include	<windows.h>
#include	<tchar.h>
#include	<stdio.h>

#include	"resource.h"	/* リソース専用ヘッダ */

#include	"pbsdir32.h"

#include	"lanch.h"

extern	int	access32(LPTSTR file);

extern	TCHAR	Ddr[20],Ddir[1024],Dname[256],Dext[256];
extern	HMENU		hMnu;		// メニューハンドル

int	DropMax;	/* D&Dアプリ数 */
EXEFILES *Drop;		/* D&Dアプリリスト */
HLOCAL	hDropExe;	/* D&Dアプリ用メモリハンドル */
HMENU	hDMenu;		/* D&D起動用メニューハンドル */
TCHAR	DropDir[1024];	/* D&D起動用ショートカットのあるパス */

TCHAR	BootDir[1024];	/* メニュー用ショートカットのあるパス */
int	BootMax;	/* アイテムメニュー数 */
EXEFILES *Boot;		/* アイテム実行アプリリスト */
HLOCAL	hBootExe;	/* アイテム実行アプリ用メモリハンドル */
HMENU	hBMenu;		/* アイテム実行アプリメニューハンドル */

EXEFILES *pTmp;		/* アイテム実行アプリリスト */

/* 拡張子検索リスト	*/
#define	SEXTMAX	6
TCHAR	Sexts[SEXTMAX + 1][20]={
	_T("*.lnk"),
	_T("*.pif"),
	_T("*.bat"),
	_T("*.exe"),
	_T("*.url"),
	_T("*.website"),
	_T("")
};

/*	起動メニュー用実行ファイルを検索	*/
HLOCAL	LanchListSet(LPTSTR dirs,int *xmax)
{
	int	i,dmax,cnt,j;
	HLOCAL	hlist;
	LPTSTR	*item;
	EXEFILES tdat;

	if(access32(dirs)!=0){
		/* ターゲットパスにアクセスできない	*/
		*xmax=0;
		return NULL;
	}

	dmax=getdirmax32(dirs);
	if(dmax<1){
		/* ディレクトリ内容が無い	*/
		*xmax=0;
		return NULL;
	}

	hlist=LocalAlloc(LHND,sizeof(EXEFILES)*dmax);
	if(hlist==NULL){		/* 確保できなかった */
		*xmax=0;
		return NULL;
	}

	/* エリアをロック	*/
	pTmp=(EXEFILES *)LocalLock(hlist);
	if (pTmp == NULL) {
		// ロックに失敗(絶対起こらないがMFCに怒られる)
		LocalFree(hlist);
		*xmax = 0;
		return NULL;
	}

	cnt=0;
	for(j=0;j<SEXTMAX;j++){
		/* 拡張子検索 */
		dmax=getdir32_items(dirs,Sexts[j],_T("nrha"),&item);
		for(i=0;i<dmax;i++,cnt++){
			_tcscpy(pTmp[cnt].File,item[i]);
		
			/* メニューリストに表示するのはファイル名のみ */
			_splitpathT(item[i], Ddr, Ddir, Dname, Dext);
			_tcscpy(pTmp[cnt].Menu,Dname);
		}
	
		LocalFree(item);	/* 検索用ファイルリストは解放 */
	}

	/* ファイルをひっくるめてソート */
	for(i=0;i<cnt-1;i++){
		for(j=i+1;j<cnt;j++){
			if(_tcscmp(pTmp[i].Menu,pTmp[j].Menu)>0){
				memcpy(&tdat,&pTmp[i],sizeof(EXEFILES));
				memcpy(&pTmp[i],&pTmp[j],sizeof(EXEFILES));
				memcpy(&pTmp[j],&tdat,sizeof(EXEFILES));
			}
		}
	}

	/* リストメモリをアンロック	*/
	LocalUnlock(hlist);

	/* 最大数を渡す	*/
	*xmax=cnt;

	/* メモリハンドルを返す	*/
	return hlist;
}

/*	アイテム起動用メニュー初期化	*/
void	Lanchinit(HWND hwnd,HMENU hmnu)
{
	int	cnt,i,j;
	HLOCAL	hlst;

	hlst=LanchListSet(BootDir,&cnt);
	if(hlst == NULL){
		/* 検索でエラーになった	*/
		BootMax=0;
		hBootExe=NULL;
		return;
	}

	/* メモリロック	*/
	Boot=(EXEFILES *)LocalLock(hlst);
	if (Boot == NULL) {
		// ロックに失敗(絶対起こらないがMFCに怒られる)
		LocalFree(hlst);
		BootMax = 0;
		hBootExe = NULL;
		return;
	}

	/* 検索結果を反映	*/
	BootMax = cnt;
	hBootExe = hlst;

	/* Itemメニューのハンドルを得る */
	hBMenu=GetSubMenu(hmnu,0);

	/* メニュー登録 */
	for(i=0;i<BootMax;i++){
		AppendMenu(
			hBMenu, 
			(MF_ENABLED | MF_STRING), 
			(MNU_ITEM+1)+i,
			Boot[i].Menu
		);
	}
	
	/* ０番用のダミーメニューリストを削除 */
	DeleteMenu(hBMenu,MNU_ITEM,MF_BYCOMMAND);
	
	DrawMenuBar(hwnd);
}

/*	アイテム起動用メニュー解放	*/
void	Lanchfree()
{
	int	i;
	
	if(hBootExe==NULL)return;
	
	/* 空メニューを追加 */
	AppendMenu(
		hBMenu, 
		(MF_GRAYED | MF_STRING), 
		MNU_POP,
		_T("(なし)")
	);
	
	/* 登録したアプリメニューを全削除 */
	for(i=0;i<BootMax;i++){
		DeleteMenu(hBMenu,(MNU_ITEM+(BootMax-i)),MF_BYCOMMAND);
	}
	
	LocalUnlock(hBootExe);
	LocalFree(hBootExe);
}

//	プログラム実行
void	Pbs_cmdexec(HWND hwnd, LPTSTR fpath, LPTSTR cmd)
{
	ShellExecute(
		hwnd,
		NULL,
		fpath,
		cmd,
		NULL,
		SW_SHOWNORMAL
	);
}

/*	アイテムアプリを実行	*/
void	LanchExec(HWND hwnd,int sel)
{
	TCHAR	fpath[1024];
	
	/* フルパスを作成 */
	_makepathT(fpath, _T(""), BootDir, Boot[sel].File, _T(""));
	
	/* ショートカットを実行 */
	Pbs_cmdexec(hwnd, fpath, NULL);
}

/*	D&D起動用メニューリスト初期化	*/
HMENU	Dropinit()
{
	int	cnt,i,j;
	HLOCAL	hlst;

	hlst=LanchListSet(DropDir,&cnt);
	if(hlst == NULL){
		/* 検索でエラーになった	*/
		DropMax=0;
		hDropExe=NULL;
		
		hDMenu=CreatePopupMenu();	/* メニュー生成 */
		/* 空メニューを１個だけ作っておく */
		AppendMenu(
			hDMenu, 
			(MF_GRAYED | MF_STRING), 
			MNU_POP,
			_T("(なし)")
		);
		return hDMenu;
	}

	Drop=(EXEFILES *)LocalLock(hlst);
	if (Drop == NULL) {
		// ロックに失敗(絶対起こらないがMFCに怒られる)
		LocalFree(hlst);
		DropMax = 0;
		hDropExe = NULL;

		hDMenu = CreatePopupMenu();	/* メニュー生成 */
		/* 空メニューを１個だけ作っておく */
		AppendMenu(
			hDMenu,
			(MF_GRAYED | MF_STRING),
			MNU_POP,
			_T("(なし)")
		);
		return hDMenu;
	}

	/* 検索結果を反映	*/
	DropMax = cnt;
	hDropExe = hlst;

	hDMenu=CreatePopupMenu();	/* メニュー生成 */
	if(DropMax>0){
		/* メニュー登録 */
		for(i=0;i<DropMax;i++){
			AppendMenu(
				hDMenu, 
				MF_ENABLED | MF_STRING, 
				MNU_POP+i,
				Drop[i].Menu
			);
		}
	} else {
		/* 空メニューを１個だけ作っておく */
		AppendMenu(
			hDMenu, 
			(MF_GRAYED | MF_STRING), 
			MNU_POP,
			_T("(なし)")
		);
	}
	
	return hDMenu;
}

/*	D&D起動用メニューリストを解放	*/
void	Dropfree()
{
	DestroyMenu(hDMenu);		/* ポップアップメニューを破棄 */
	
	if(hDropExe==NULL)return;
	
	LocalUnlock(hDropExe);
	LocalFree(hDropExe);
}

void	DropExec(HWND hwnd,int sel,LPTSTR cmd)
{
	TCHAR	fpath[1024];
	
	/* フルパスを作成 */
	_makepathT(fpath, _T(""), DropDir, Drop[sel].File, _T(""));
	
	/* ショートカットを実行 */
	Pbs_cmdexec(hwnd, fpath, cmd);
}

//	スタートアップフォルダにある内容を実行
void	StartupExec(HWND hwnd)
{
	int	cnt, i, j;
	HLOCAL	hlst;
	EXEFILES* pstartup;
	TCHAR	fpath[1024];

	// フォルダ内容検索
	hlst = LanchListSet(StartupDir, &cnt);
	if (hlst == NULL) {
		/* 検索でエラーになった	*/
		return;
	}

	/* メモリロック	*/
	pstartup = (EXEFILES*)LocalLock(hlst);
	if (pstartup == NULL) {
		/* ロック失敗(絶対ありえない)	*/
		LocalFree(hlst);
		return;
	}

	for (i = 0; i < cnt; i++) {
		/* フルパスを作成 */
		_makepathT(fpath, _T(""), StartupDir, pstartup[i].File, _T(""));

		/* ショートカットを実行 */
		Pbs_cmdexec(hwnd, fpath, NULL);
	}

	// スタートアップの場合、もう不要なのでメモリ開放
	LocalUnlock(hlst);
	LocalFree(hlst);
}