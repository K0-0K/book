/* ************************************
*《精通Windows API》
* 示例代码
* ioctl.c
* 16.2 IO控制、内核通信
**************************************/
/* 头文件 */
#include <Windows.h>
#include <Winioctl.h>
#include <stdio.h>
/* 函数声明 */
DWORD EnjectCdrom(LPSTR szCdRomName);
DWORD PrintNTFSInfo(LPSTR szVolumeName);

/*************************************
* main
* 功能	-cdrom <盘符> 弹出光盘
*			-ntfs <盘符> 显示nfts分区的信息
**************************************/
int main(int argc, char* argv[])
{
	CHAR szName[64];
	if(argc == 3)
	{
		// 构造设备名
		wsprintf(szName, "\\\\.\\%s.", argv[2]);
		// 弹出光盘
		if(lstrcmp(argv[1],"-cdrom") == 0)
		{
			EnjectCdrom( szName );
			return 0;
		}
		// 获取NTFS分区详细信息
		if(lstrcmp(argv[1],"-ntfs") == 0)
		{
			PrintNTFSInfo( szName );
			return 0;
		}
	}
	// 使用方法
	printf("usage:  \n\t %s -cdrom <volume>\n\t %s -ntfs <volume>\nlike this: \n\t -cdrom G:", 
		argv[0], argv[0]);

	return 0;
}

/*************************************
* DWORD EnjectCdrom(LPSTR szCdRomName)
* 功能	弹出指定的光盘
* 参数	szCdRomName，设备名
**************************************/
DWORD EnjectCdrom(LPSTR szCdRomName)
{
	HANDLE hDevice;
	DWORD dwBytesReturned;

	hDevice = CreateFile(szCdRomName, // 设备名
		GENERIC_ALL, // 存取权限
		FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, // 共享方式
		NULL, // 默认安全属性
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, 
		NULL); 
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Could not open file (error %d)\n", GetLastError());
		return 0;
	}
	// 发送IOCTL
	if(!DeviceIoControl(
		(HANDLE) hDevice, // 设备句柄
		IOCTL_STORAGE_EJECT_MEDIA, // 控制码
		NULL, // 输入缓存
		0, // 输入缓存大小
		NULL, // 输出缓存
		0, // 输出缓存大小
		&dwBytesReturned, // 实际需要的输输入缓存大小
		NULL // 非OVERLAPPED
		))
	{
		printf("DeviceIoControl error (%d)",GetLastError());
		return 0;
	}
	return 1;

}

/*************************************
* DWORD PrintNTFSInfo(LPSTR szVolumeName)
* 功能	获取显示指定的NTFS驱动器信息
* 参数	szVolumeName，设备名
**************************************/
DWORD PrintNTFSInfo(LPSTR szVolumeName)
{
	// FSCTL_GET_NTFS_VOLUME_DATA IO控制的返回值保存在
	// NTFS_VOLUME_DATA_BUFFER结构中
	NTFS_VOLUME_DATA_BUFFER nvdb;
	DWORD dwBufferSize;
	HANDLE hDevice;
	// 清空参数
	ZeroMemory(&nvdb,sizeof(nvdb));

	hDevice = CreateFile(szVolumeName, 
		GENERIC_ALL, 
		FILE_SHARE_READ|FILE_SHARE_WRITE, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL); 
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Could not open file (error %d)\n", GetLastError());
		return 0;
	}

	if(DeviceIoControl(
		hDevice, // 设备句柄
		FSCTL_GET_NTFS_VOLUME_DATA, // 控制码
		NULL, // 输入缓存
		0, // 输入缓存大小
		&nvdb, // 输出缓存
		sizeof( NTFS_VOLUME_DATA_BUFFER ), // 输出缓存大小
		&dwBufferSize, // 返回的实际数据大小
		NULL // 非OVERLAPPED
		))
	{
		// 打印获取的信息
		printf("SerialNumber %lu\n",nvdb.VolumeSerialNumber);
		printf("Starting logical cluster number of the master file table: %lu\n",nvdb.MftStartLcn);
		printf("Length of the master file table: %lu\n",nvdb.MftValidDataLength);
		printf("... ...\n");
	}
	else
	{
		printf("DeviceIoControl error: (%d)\n",GetLastError());
		return 0;
	}
	return 1;
}