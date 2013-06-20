/* ************************************
 *《精通Windows API》 
 * 示例代码
 * GetVolumeInfo.c
 * 4.2.1	遍历驱动器并获取驱动器属性
 **************************************/

/* 头文件　*/
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
/* 预定义　*/
#define BUFSIZE 1024
/* 函数申明　*/
BOOL GetDirverInfo(LPSTR szDrive);

/* ************************************
 * 功能	应用程序主函数，遍历驱动器并调用
 *			GetDirverInfo 获取驱动器属性
 **************************************/
void main(void)
{
	CHAR szLogicalDriveStrings[BUFSIZE];
	PCHAR szDrive;

	ZeroMemory(szLogicalDriveStrings,BUFSIZE);
	// 获取逻辑驱动器卷标名
	GetLogicalDriveStrings(BUFSIZE - 1,szLogicalDriveStrings);	
	szDrive = (PCHAR)szLogicalDriveStrings;
	// 循环处理每个卷
	do
	{
		if(!GetDirverInfo(szDrive))
		{
			printf("\nGet Volume Information Error: %d", GetLastError());
		}
		szDrive += (lstrlen(szDrive)+1);
	}
	while(*szDrive!='\x00');
}

/* ************************************
 * BOOL GetDirverInfo(LPSTR szDrive)
 * 功能	获取驱动器的属性
 * 参数	LPSTR szDrive
 * 	指明要获取属性的驱动器的根路径 如 C:\
 * 返回值 BOOL 是否成功
 **************************************/
BOOL GetDirverInfo(LPSTR szDrive)
{
	UINT uDriveType;
	DWORD dwVolumeSerialNumber;
	DWORD dwMaximumComponentLength;
	DWORD dwFileSystemFlags;
	TCHAR szFileSystemNameBuffer[BUFSIZE];
	printf("\n%s\n",szDrive);
	uDriveType = GetDriveType(szDrive);
	// 判断类型
	switch(uDriveType)
	{
	case DRIVE_UNKNOWN:
		printf("The drive type cannot be determined. ");
		break;
	case DRIVE_NO_ROOT_DIR:
		printf("The root path is invalid, for example, no volume is mounted at the path. ");
		break;
	case DRIVE_REMOVABLE:
		printf("The drive is a type that has removable media, for example, a floppy drive or removable hard disk. ");
		break;
	case DRIVE_FIXED:
		printf("The drive is a type that cannot be removed, for example, a fixed hard drive. ");
		break;
	case DRIVE_REMOTE:
		printf("The drive is a remote (network) drive. ");
		break;
	case DRIVE_CDROM:
		printf("The drive is a CD-ROM drive. ");
		break;
	case DRIVE_RAMDISK:
		printf("The drive is a RAM disk. ");
		break;
	default:
		break;
	}
	if (!GetVolumeInformation(
		szDrive, NULL, 0,
		&dwVolumeSerialNumber,
		&dwMaximumComponentLength,
		&dwFileSystemFlags,
		szFileSystemNameBuffer,
		BUFSIZE
		))
	{
		return FALSE;
	}
	printf ("\nVolume Serial Number is %u",dwVolumeSerialNumber);
	printf ("\nMaximum Component Length is %u",dwMaximumComponentLength);
	printf ("\nSystem Type is %s\n",szFileSystemNameBuffer);

	if(dwFileSystemFlags & FILE_SUPPORTS_REPARSE_POINTS)
	{
		printf ("The file system does not support volume mount points.\n");
	}
	if(dwFileSystemFlags & FILE_VOLUME_QUOTAS)
	{
		printf ("The file system supports disk quotas.\n");
	}
	if(dwFileSystemFlags & FILE_CASE_SENSITIVE_SEARCH)
	{
		printf ("The file system supports case-sensitive file names.\n");
	}
	//you can use these value to get more informaion
	//
	//FILE_CASE_PRESERVED_NAMES
	//FILE_CASE_SENSITIVE_SEARCH
	//FILE_FILE_COMPRESSION
	//FILE_NAMED_STREAMS
	//FILE_PERSISTENT_ACLS
	//FILE_READ_ONLY_VOLUME
	//FILE_SUPPORTS_ENCRYPTION
	//FILE_SUPPORTS_OBJECT_IDS
	//FILE_SUPPORTS_REPARSE_POINTS
	//FILE_SUPPORTS_SPARSE_FILES
	//FILE_UNICODE_ON_DISK
	//FILE_VOLUME_IS_COMPRESSED
	//FILE_VOLUME_QUOTAS
	printf("...\n");
	return TRUE;
}