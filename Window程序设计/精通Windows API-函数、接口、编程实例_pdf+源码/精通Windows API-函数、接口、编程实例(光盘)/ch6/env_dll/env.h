#include <windows.h>

#define VARIABLES_APPEND	1
#define VARIABLES_RESET		2
#define VARIABLES_NULL		0

DWORD WINAPI EnumEnvironmentVariables();
DWORD WINAPI ChangeEnviromentVariables(
									   LPSTR szName, 
									   LPSTR szNewValue,
									   DWORD dwFlag);