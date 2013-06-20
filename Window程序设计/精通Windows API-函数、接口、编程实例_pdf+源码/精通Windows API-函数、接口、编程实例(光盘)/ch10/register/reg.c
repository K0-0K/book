/* ************************************
*《精通Windows API》 
* 示例代码
* reg.c
* 10.3  注册表
**************************************/
/* 头文件 */
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
/* 预定义 */
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
/* ************************************
* void QueryKey(HKEY hKey) 
* 功能	列举指定注册表项的子键
**************************************/
void QueryKey(HKEY hKey) 
{ 
	TCHAR    achKey[MAX_KEY_LENGTH];   
	DWORD    cbName;                 
	TCHAR    achClass[MAX_PATH] = TEXT("");  
	DWORD    cchClassName = MAX_PATH; 
	DWORD    cSubKeys=0;        
	DWORD    cbMaxSubKey;          
	DWORD    cchMaxClass;           
	DWORD    cValues;              
	DWORD    cchMaxValue;        
	DWORD    cbMaxValueData;      
	DWORD    cbSecurityDescriptor; 
	FILETIME ftLastWriteTime;    

	DWORD i, retCode; 

	TCHAR  achValue[MAX_VALUE_NAME]; 
	DWORD cchValue = MAX_VALUE_NAME; 

	// 获取类名和数量
	retCode = RegQueryInfoKey(
		hKey,                    // 键的句柄
		achClass,                //  类名 
		&cchClassName,           // 类名长度 
		NULL,                    // 保留
		&cSubKeys,               // 子键的数量
		&cbMaxSubKey,            // 子键长度
		&cchMaxClass,            // 类长度
		&cValues,                // 子键键值数量
		&cchMaxValue,            // 子键名长度
		&cbMaxValueData,         // 键值长度
		&cbSecurityDescriptor,   // 安全描述符
		&ftLastWriteTime);       // 最后写时间 

	// 列举子键    
	if (cSubKeys)
	{
		printf( "\nNumber of subkeys: %d\n", cSubKeys);

		for (i=0; i<cSubKeys; i++) 
		{ 
			cbName = MAX_KEY_LENGTH;
			retCode = RegEnumKeyEx(hKey, i,
				achKey, 
				&cbName, 
				NULL, 
				NULL, 
				NULL, 
				&ftLastWriteTime); 
			if (retCode == ERROR_SUCCESS) 
			{
				printf(TEXT("(%d) %s\n"), i+1, achKey);
			}
		}
	} 

	// 列举键值
	if (cValues) 
	{
		printf( "\nNumber of values: %d\n", cValues);

		for (i=0, retCode=ERROR_SUCCESS; i<cValues; i++) 
		{ 
			cchValue = MAX_VALUE_NAME; 
			achValue[0] = '\0'; 
			retCode = RegEnumValue(hKey, i, 
				achValue, 
				&cchValue, 
				NULL, 
				NULL,
				NULL,
				NULL);

			if (retCode == ERROR_SUCCESS ) 
			{ 
				printf(TEXT("(%d) %s\n"), i+1, achValue); 
			} 
		}
	}
}
/* ************************************
* void AddKey(HKEY hKey)
* 功能	增加一个子键，并设置键值
**************************************/
void AddKey(HKEY hKey)
{
	HKEY hSubKey;
	DWORD dwKeyValue = 100;
	// 创建键
	RegCreateKey(hKey,"MySoftware",&hSubKey);
	// 设置键值
	if( ERROR_SUCCESS != RegSetValueEx(
		hSubKey,
		"TEST",
		0,
		REG_DWORD,
		(LPBYTE)&dwKeyValue,
		sizeof(DWORD)))
	{
		printf("error\n");
	}
}
/* ************************************
* void main(void)
* 功能	打开键，获得键句柄
**************************************/
void main(void)
{
	HKEY hTestKey;

	if( RegOpenKeyEx( HKEY_CURRENT_USER,
		TEXT("SOFTWARE"),
		0,
		KEY_READ | KEY_WRITE,
		&hTestKey) == ERROR_SUCCESS
		)
	{
		// 增加键
		AddKey(hTestKey);
		// 列举子键
		QueryKey(hTestKey);
	}
}