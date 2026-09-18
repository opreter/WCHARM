#ifdef _MSC_VER
extern	void	_splitpathT(LPTSTR fpath, LPTSTR dr, LPTSTR dir, LPTSTR name, LPTSTR ext);
extern	void	_makepathT(LPTSTR fpath, LPTSTR dr, LPTSTR dir, LPTSTR name, LPTSTR ext);
#elif	defined(WINCE) || defined(BCC32)
extern	void	_splitpath(LPTSTR fpath,LPTSTR dr,LPTSTR dir,LPTSTR name,LPTSTR ext);

extern	void	_makepath(LPTSTR fpath,LPTSTR dr,LPTSTR dir,LPTSTR name,LPTSTR ext);
#endif

extern	UINT	getdirmax32(LPTSTR ipath);

extern	DWORD	fattr32(LPTSTR att);
extern	UINT	getdir32(LPTSTR path,LPTSTR mask,LPTSTR attr,LPTSTR items[]);
extern	UINT	getdir32_items(LPTSTR path,LPTSTR mask,LPTSTR attr,LPTSTR **itemp);
