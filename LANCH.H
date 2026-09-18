typedef struct {
	TCHAR	Menu[256];
	TCHAR	File[256];
} EXEFILES;

extern	int	DropMax;		/* D&D起動用アプリ数 */
extern	EXEFILES *Drop;			/* D&D起動用アプリ構造体 */
extern	TCHAR	DropDir[1024];		/* D&D起動用ショートカットのあるパス */

extern	HMENU	Dropinit(void);		/* D&D起動用アプリ検索 */
extern	void	Dropfree(void);		/* D&D起動用リストメモリ解放 */
extern	void	DropExec(HWND hwnd,int sel,LPTSTR cmd);

extern	HMENU	hDMenu;			/* D&D起動用メニューハンドル */

extern	TCHAR	BootDir[1024];		/* メニュー用ショートカットのパス */

extern	void	Lanchinit(HWND hend,HMENU hmenu);
extern	void	Lanchfree(void);
extern	void	LanchExec(HWND hwnd,int sel);

extern	TCHAR	StartupDir[1024];	// スタートアップフォルダ
extern	void	StartupExec(HWND hwnd);

//	プログラム実行
extern	void	Pbs_cmdexec(HWND hwnd, LPTSTR fpath, LPTSTR cmd);
