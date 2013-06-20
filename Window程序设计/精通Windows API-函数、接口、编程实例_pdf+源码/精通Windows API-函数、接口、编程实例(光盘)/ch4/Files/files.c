/* ************************************
 *《精通Windows API》 
 * 示例代码
 * files.c
 * 4.3.1	删除、复制、重命名、移动文件
 **************************************/

/* 头文件　*/
#include <windows.h>
#include <stdio.h>

/* ************************************
 * int main( int argc, PCHAR argv[] )
 * 功能	应用程序主函数，根据输入参数
 *		删除、复制、重命名文件
 *
 * 参数	删除文件：
 *			-d 文件路径
 *		将文件路径1的文件复制到文件路径2：
 *			-c 文件路径1 文件路径2
 *		将文件路径1的文件移动、重命名为文件路径2的文件
 *			-m 文件路径1 文件路径2
 **************************************/
int main(int argc, PCHAR argv[])
{
	//-d参数，删除文件。
	if(0==lstrcmp("-d",argv[1]) && argc==3)
	{
		if(!DeleteFile(argv[2]))
		{
			printf("删除文件错误：%x\n",GetLastError());
		}
		else
		{
			printf("删除成功！\n");
		}
	}
	//-c参数，复制文件。
	//如果文件存在，询问用户是否覆盖
	else if(0==lstrcmp("-c",argv[1]) && argc==4)
	{
		//复制，不覆盖已经存在的文件
		if(!CopyFile(argv[2],argv[3],TRUE))
		{
			//LastError == 0x50，文件存在。
			if(GetLastError() == 0x50)
			{
				printf("文件%s已经存在，是否覆盖？y/n：",argv[3]);
				if('y'==getchar())
				{
					//复制，覆盖已经存在的文件。
					if(!CopyFile(argv[2],argv[3],FALSE))
					{
						printf("复制文件错误，%d\n",GetLastError());
					}
					else
					{
						printf("复制成功！\n");
					}
				}
				else
				{
					return 0;
				}
			}
		}
		else
		{
			printf("复制成功！\n");
		}
	}
	//-m参数，移动、重命名文件。
	else if(0==lstrcmp("-m",argv[1]) && argc==4)
	{
		if(!MoveFile(argv[2],argv[3]))
		{
			printf("移动文件错误：%d\n",GetLastError());
		}
		else
		{
			printf("移动文件成功！\n");
		}
	}
	else
	{
		printf("参数错误！\n");
	}
}

