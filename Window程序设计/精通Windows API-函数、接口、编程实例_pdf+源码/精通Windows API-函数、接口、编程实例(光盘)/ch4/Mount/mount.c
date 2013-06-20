/* ************************************
 *《Windows应用程序开发》 
 * 示例代码
 * mount.c
 * 4.2.2	卷挂载点操作
 **************************************/
/* 预编译声明 */
#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#define BUFSIZE            MAX_PATH
#define FILESYSNAMEBUFSIZE MAX_PATH

/* ************************************
 * ProcessVolumeMountPoint
 * 功能	列举挂载点
 **************************************/
BOOL ProcessVolumeMountPoint (HANDLE hPt,
							  TCHAR *PtBuf, DWORD dwPtBufSize,
							  TCHAR *Buf)
{
	BOOL bFlag;					// 结果
	TCHAR Path[BUFSIZE];	// 全路径
	TCHAR Target[BUFSIZE];		// 挂载点设备

	printf ("\tVolume mount point found is \"%s\"\n", PtBuf);

	lstrcpy (Path, Buf);
	lstrcat (Path, PtBuf);

	bFlag = GetVolumeNameForVolumeMountPoint(
		Path,  Target, 	BUFSIZE  	);

	if (!bFlag)
		printf ("\tAttempt to get volume name for %s failed.\n", Path);
	else 
		printf ("\tTarget of the volume mount point is %s.\n", Target);

	bFlag = FindNextVolumeMountPoint(
		hPt,  PtBuf,  dwPtBufSize );
	return (bFlag);
}
/* ************************************
 * ProcessVolume
 * 功能	判断卷类型，列举挂载点
 **************************************/
BOOL ProcessVolume (HANDLE hVol, TCHAR *Buf, DWORD iBufSize)
{
	BOOL bFlag;           // 返回标志
	HANDLE hPt;           // 卷句柄
	TCHAR PtBuf[BUFSIZE]; // 挂载点路径
	DWORD dwSysFlags;     // 文件系统标记
	TCHAR FileSysNameBuf[FILESYSNAMEBUFSIZE];

	printf ("Volume found is \"%s\".\n", Buf);

	// 是否NTFS
	GetVolumeInformation( Buf, NULL, 0, NULL, NULL,
		&dwSysFlags, FileSysNameBuf, 
		FILESYSNAMEBUFSIZE);

	if (! (dwSysFlags & FILE_SUPPORTS_REPARSE_POINTS)) 
	{
		printf ("\tThis file system does not support volume mount points.\n");
	} 
	else 
	{
		// 本卷中的挂载点
		hPt = FindFirstVolumeMountPoint(
			Buf, // 卷的跟跟踪
			PtBuf, // 挂载点路径
			BUFSIZE 
			);

		if (hPt == INVALID_HANDLE_VALUE)
		{
			printf ("\tNo volume mount points found!\n");
		} 
		else 
		{
			// 处理挂载点
			bFlag = ProcessVolumeMountPoint (hPt, 
				PtBuf, 
				BUFSIZE, 
				Buf);
			// 循环
			while (bFlag) 
				bFlag = 
				ProcessVolumeMountPoint (hPt, PtBuf, BUFSIZE, Buf);
			// 结束
			FindVolumeMountPointClose(hPt);
		}
	}

	// 下一个
	bFlag = FindNextVolume(
		hVol, Buf,  iBufSize);

	return (bFlag); 
}
/* ************************************
 * int GetMountPoint(void)
 * 功能	获取挂载点
 **************************************/
int GetMountPoint(void)
{
	TCHAR buf[BUFSIZE];		// 卷标识符
	HANDLE hVol;					// 卷句柄
	BOOL bFlag;					// 结果标志

	printf("Volume mount points info of this computer:\n\n");
	// 打开卷
	hVol = FindFirstVolume (buf, BUFSIZE );
	if (hVol == INVALID_HANDLE_VALUE)
	{
		printf ("No volumes found!\n");
		return (-1);
	}

	bFlag = ProcessVolume (hVol, buf, BUFSIZE);
	while (bFlag) 
	{
		bFlag = ProcessVolume (hVol, buf, BUFSIZE);
	}

	bFlag = FindVolumeClose(	hVol );
	return (bFlag);
}

/* ************************************
 * void Usage (PCHAR argv)
 * 功能	使用方法
 **************************************/
void Usage (PCHAR argv)
{
	printf( "\n\n\t%s, mount a volume at a mount point.\n", argv );
	printf( "\tFor example, \"mount D:\\mnt\\drives\\ E:\\\"\n" );
}

/* ************************************
 *	main
 * 功能	入口函数
 **************************************/
int main( int argc, PCHAR argv[] )
{
	BOOL bFlag;
	CHAR Buf[BUFSIZE];     

	if( argc != 3 ) 
	{
		GetMountPoint();
		Usage( argv[0] );
		return( -1 );
	}

	bFlag = GetVolumeNameForVolumeMountPoint(
		argv[2],		// 输入挂载点或目录
		Buf,				// 输出卷名
		BUFSIZE
		);

	if (bFlag != TRUE) 
	{
		printf( "Retrieving volume name for %s failed.\n", argv[2] );
		return (-2);
	}

	printf( "Volume name of %s is %s\n", argv[2], Buf );
	bFlag = SetVolumeMountPoint(
		argv[1],	// 挂载点
		Buf			// 需要挂载的卷
		);

	if (!bFlag)
	{
		printf ("Attempt to mount %s at %s failed. error code is\n", 
			argv[2], argv[1], GetLastError());
	}

	return (bFlag);
}