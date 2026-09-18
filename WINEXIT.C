#include	<windows.h>
#include	<tchar.h>

#if _MSC_VER >= 1900
#include	<VersionHelpers.h>
#endif

extern	TCHAR	szAppName[256];	// アプリ名

int	OnSetPrivilege(void);

void	Pbs_exitwin(HWND hwnd,int Mode)
{
	long	lmode;
	TCHAR	wmess[256];
	int	r;
	BOOL	bf;
	DWORD	dwVer;
	WORD	wVer;
	BYTE	bVer;
	
	lmode = EWX_SHUTDOWN;
	_tcscpy(wmess,_T("システムをシャットダウンします"));
	if(Mode == 1){
		lmode = EWX_LOGOFF;
		_tcscpy(wmess,_T("システムをログオフします"));
		bf=FALSE;
	} else if(Mode == 2){
		lmode = EWX_REBOOT;
		_tcscpy(wmess,_T("システムを再起動します"));
		bf=TRUE;
	}
	
	r = MessageBox(hwnd,wmess, szAppName,MB_OKCANCEL + MB_ICONINFORMATION);
	if(r == IDOK){
#if _MSC_VER >= 1900
		if (IsWindowsXPOrGreater()) {		/* WindowsXP以降とする	*/
			r = OnSetPrivilege();		/* シャットダウン特権を得る */
			if (r != 0) {
				MessageBox(
					hwnd,
					_T("システム権限がありません"),
					NULL,
					(MB_OK + MB_ICONEXCLAMATION)
				);
				return;
			}
		}
#else
		dwVer=GetVersion();	/* Windowsのバージョンを得る	*/
		wVer=LOWORD(dwVer);
		bVer=(wVer & 0x0f);
		if(bVer>4){		/* Ver5以降ならば	*/
			r=OnSetPrivilege();	/* シャットダウン特権を得る */
			if(r!=0){
				MessageBox(
					hwnd,
					_T("システム権限がありません"),
					NULL,
					(MB_OK + MB_ICONEXCLAMATION)
				);
				return;
			}
		}
#endif

		r = ExitWindowsEx(lmode,0);
		if(r == 0){
			r = MessageBox(hwnd,_T("終了を拒否しているのがいます"),szAppName, (MB_OK + MB_ICONEXCLAMATION));
		}
	}
}

int	OnSetPrivilege()
{
	HANDLE hHandle;
	LUID sLuid;
	LUID_AND_ATTRIBUTES sLuidAttr;
	TOKEN_PRIVILEGES sTokenPrgl;

	// アクセストークンを開く
	if (OpenProcessToken(
		GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES,
		&hHandle
	)==FALSE){
		return 1;
	}
	
	// 特権のLUIDを取得
	if(LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &sLuid)==FALSE){
		return 2;
	}
	
	// LUID_AND_ATTRIBUTES 構造体の準備
	// SE_SHUTDOWN_NAME を…
	// 有効に。
	sLuidAttr.Luid = sLuid;
	sLuidAttr.Attributes = SE_PRIVILEGE_ENABLED;
	
	// TOKEN_PREVILEGES 構造体の準備
	sTokenPrgl.PrivilegeCount = 1;	// 構造体中の特権の数は1個。
	sTokenPrgl.Privileges[0] = sLuidAttr;
	
	// アクセストークンの変更を実行
	if (AdjustTokenPrivileges(
		hHandle, 
		FALSE, 
		&sTokenPrgl,
		0, 
		NULL, 
		NULL
	)==FALSE){
		return 3;
	}
	
	// アクセストークンを閉じる
	if(CloseHandle(hHandle)==FALSE){
		return 4;
	}
	
	// 特権付加成功
	return 0;
}
