#include	<windows.h>
#include	<tchar.h>

#ifdef _MSC_VER
#elif	!defined(BCC32)
#define	WINCE
#endif

#include	"pbscmd32.h"
#include	"pbsdir32.h"

#ifdef _MSC_VER
void	_splitpathT(LPTSTR fpath, LPTSTR dr, LPTSTR dir, LPTSTR name, LPTSTR ext)
{
	_tsplitpath_s(fpath, dr, _MAX_DRIVE, dir, _MAX_DIR, name, _MAX_FNAME, ext, _MAX_EXT);
}

void	_makepathT(LPTSTR fpath, LPTSTR dr, LPTSTR dir, LPTSTR name, LPTSTR ext)
{
	_tmakepath_s(fpath, _MAX_PATH, dr, dir, name, ext);
}
#elif	defined(WINCE) || defined(BCC32)
extern	void	_splitpathT(LPTSTR fpath,LPTSTR dr,LPTSTR dir,LPTSTR name,LPTSTR ext);

extern	void	_makepathT(LPTSTR fpath,LPTSTR dr,LPTSTR dir,LPTSTR name,LPTSTR ext);

void	_splitpathT(LPTSTR fpath,LPTSTR dr,LPTSTR dir,LPTSTR name,LPTSTR ext)
{
	UINT	i,l,m;
	TCHAR	tpath[256];
	
	_tcscpy(tpath,fpath);	/* 書きつぶすのでコピーを取る */
	l=_tcslen(tpath);
	
	dr[0]=dir[0]=ext[0]=name[0]=0;	/* 初期化 */
	m=0;
	for(i=0;i<l;i++){
		if(m==0){
			if(tpath[l-i-1]==_T('.')){
				/* ここまでが拡張子 */
				_tcscpy(ext,&tpath[l-i-1]);
				tpath[l-i-1]=0;
				m=1;
			} else if(tpath[l-i-1]==_T('\\')){
				/* 拡張子が無く、パスの区切り */
				_tcscpy(name,&tpath[l-i]);
				tpath[l-i]=0;
				m=2;
			}
		} else if(m==1){
			/* 拡張子は切り終わった */
			if(tpath[l-i-1]==_T('\\')){
				/* パスの区切り */
				_tcscpy(name,&tpath[l-i]);
				tpath[l-i]=0;
				m=2;
				break;		/* ループを抜ける */
			}
		}
	}
	if(m==0 || m==1){
		/* パスの区切りがなかった(CEならふつーないはず) */
		_tcscpy(name,tpath);
	} else {
		/* 拡張子、名前も区切り済み */
		_tcscpy(dir,tpath);
	}
}

void	_makepathT(LPTSTR fpath,LPTSTR dr,LPTSTR dir,LPTSTR name,LPTSTR ext)
{
	UINT	l;
	
	_tcscpy(fpath,dir);
	l=_tcslen(fpath);
	if(l>0 && fpath[l-1]!=_T('\\')){
		/* 末尾がパスの区切りで終わってない */
		_tcscat(fpath,_T("\\"));
	}
	
	_tcscat(fpath,name);
	l=_tcslen(fpath);
	if(fpath[l-1]!=_T('.') && ext[0]!=_T('.') && _tcslen(ext)>0){
		/* 名前の末尾と拡張子の先頭に点がない */
		_tcscat(fpath,_T("."));
	}
	
	_tcscat(fpath,ext);
}
#endif

DWORD	fattr32(LPTSTR att)
{
	DWORD	retd;

	retd=0;
	if(_tcschr(att,_T('n'))!=NULL)retd|=FILE_ATTRIBUTE_NORMAL;
	if(_tcschr(att,_T('r'))!=NULL)retd|=FILE_ATTRIBUTE_READONLY;
	if(_tcschr(att,_T('h'))!=NULL)retd|=FILE_ATTRIBUTE_HIDDEN;
	if(_tcschr(att,_T('s'))!=NULL)retd|=FILE_ATTRIBUTE_SYSTEM;
	if(_tcschr(att,_T('d'))!=NULL)retd|=FILE_ATTRIBUTE_DIRECTORY;
	if(_tcschr(att,_T('a'))!=NULL)retd|=FILE_ATTRIBUTE_ARCHIVE;

	return retd;
}

UINT	getdirmax32(LPTSTR ipath)
{
	HANDLE	hdir;
	TCHAR	tfpath[1024];
	UINT	dmax;
	WIN32_FIND_DATA findstr;
	
	_makepathT(tfpath, _T(""), ipath, _T("*"), _T("*"));
	
	hdir=FindFirstFile(tfpath,&findstr);
	if(hdir == INVALID_HANDLE_VALUE)return 0;
	
	dmax=1;
	while(FindNextFile(hdir,&findstr)){
		dmax++;
	}
	
	FindClose(hdir);
	
	return dmax;
}

UINT	getdir32(LPTSTR path,LPTSTR mask,LPTSTR attr,LPTSTR items[])
{
	int     max;
	WIN32_FIND_DATA fb;
	TCHAR	fpath[1024];
	HANDLE	hdir;
	
	_makepathT(fpath, _T(""), path, mask, _T(""));

	max=0;
	hdir=FindFirstFile(fpath,&fb);
	if(hdir == INVALID_HANDLE_VALUE)return 0;
	
	if((fb.dwFileAttributes & fattr32(attr))!=0){
		_tcscpy(items[max],fb.cFileName);
		if((fb.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!=0){
			_tcscat(items[max],_T("\\"));
		}
		max++;
	}
	while(FindNextFile(hdir,&fb)){
		if((fb.dwFileAttributes & fattr32(attr))!=0){
			_tcscpy(items[max],fb.cFileName);
			if((fb.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!=0){
				_tcscat(items[max],_T("\\"));
			}
			max++;
		}
	}
	
	FindClose(hdir);
	
	return max;
}

#ifdef	WINCE
/*	実体バッファをあらかじめ確保しておく	*/
#define	IDXMAX	1024
static	BYTE	itemb[IDXMAX*sizeof(TCHAR)*128];
#endif
#if 1
#define	IDXMAX	1024
static	TCHAR	itemb[IDXMAX][1024];
#endif

UINT	getdir32_items(LPTSTR path,LPTSTR mask,LPTSTR attr,LPTSTR **itemp)
{
	LONG	dsize;
	LPTSTR	dp;
	LPTSTR	td;
	UINT	dmax,i,j;
	LPTSTR	*item;
	
	dmax=getdirmax32(path);
	if(dmax<1)return dmax;
#ifdef	WINCE
	if(IDXMAX<dmax)return 0;
#endif
#if (defined(_MSC_VER) && defined(UNICODE))
	if (IDXMAX < dmax)return 0;
#endif
	
#ifdef	WINCE
	dsize = sizeof(LPTSTR*) * dmax;
#elif (defined(_MSC_VER) && defined(UNICODE))
	dsize = sizeof(LPTSTR*) * (dmax+1);
#else
	dsize=(sizeof(LPTSTR *)+(sizeof(TCHAR)*128))*dmax;
#endif
	
	item=(LPTSTR *)LocalAlloc(LPTR,dsize);
	if(item==NULL)return 0;
	
	/* インデックスに実体を割り当てる */
#if (defined(_MSC_VER) && defined(UNICODE))
	for (i = 0; i < dmax; i++) {
		dp = itemb[i];
		item[i] = dp;
	}
#else
#ifdef	WINCE
	dp = itemb;
#else
	dp=(LPTSTR)item;
	dp+=(sizeof(LPTSTR *)*dmax);
#endif
	for(i=0;i<dmax;i++){
		item[i]=dp;
		dp+=(sizeof(TCHAR)*128);
	}
#endif
	
	/* ディレクトリ内容取得 */
	dmax=getdir32(path,mask,attr,item);
	if (dmax < 1) {
		*itemp = item;
		return dmax;
	}
	
	/* インデックスをソートしておく */
	for(i=0;i<(dmax-1);i++){
		for(j=i+1;j<dmax;j++){
			if(_tcscmp(item[i],item[j])>0){
				td=item[i];
				item[i]=item[j];
				item[j]=td;
			}
		}
	}
	
	*itemp=item;
	return dmax;
}

#if 0
//	テスト用コード
main()
{
	UINT	dmax,i;
	LPTSTR	*item;
	
	dmax=getdir32_items(_T("D:\\EOS\\harpy32"),_T("*.*"),_T("nra"),&item);
	printf("%d",dmax);
	for(i=0;i<dmax;i++){
		printf("%s\n",item[i]);
	}
	
	LocalFree(item);
}
#endif
