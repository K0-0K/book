/* ************************************
*《精通Windows API》
* 示例代码
* xioctl.c
* 16.2  IO控制、内核通信
**************************************/
/* 头文件 */
#include <ntddk.h>// 包括了很多NT内核的类型、结构、函数定义，开发驱动时需要包括此头文件
#include <string.h>
#include "xioctl.h"
/* 常量与预定义 */
#define NT_DEVICE_NAME      L"\\Device\\XIOCTL"
#define DOS_DEVICE_NAME     L"\\DosDevices\\IoctlTest"

#if DBG
#define XIOCTL_KDPRINT(_x_) \
                DbgPrint("XIOCTL.SYS: ");\
                DbgPrint _x_;
#else
#define XIOCTL_KDPRINT(_x_)
#endif 

/* 函数声明 */
NTSTATUS DriverEntry(PDRIVER_OBJECT  DriverObject, PUNICODE_STRING RegistryPath);
NTSTATUS XioctlCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS XioctlDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp );
VOID XioctlUnloadDriver(PDRIVER_OBJECT DriverObject );
VOID PrintIrpInfo( PIRP Irp );
VOID PrintChars( PCHAR BufferAddress, ULONG CountChars );

#ifdef ALLOC_PRAGMA
#pragma alloc_text( INIT, DriverEntry )
#pragma alloc_text( PAGE, XioctlCreateClose)
#pragma alloc_text( PAGE, XioctlDeviceControl)
#pragma alloc_text( PAGE, XioctlUnloadDriver)
#pragma alloc_text( PAGE, PrintIrpInfo)
#pragma alloc_text( PAGE, PrintChars)
#endif // ALLOC_PRAGMA


/*************************************
* DriverEntry
* 功能	驱动的入口函数，分配了相关处理例程
**************************************/
NTSTATUS
DriverEntry(
    IN OUT PDRIVER_OBJECT   DriverObject,
    IN PUNICODE_STRING      RegistryPath
    )
{
    NTSTATUS        ntStatus;
    UNICODE_STRING  ntUnicodeString;    // 设备名
    UNICODE_STRING  ntWin32NameString;    // Win32 设备名
    PDEVICE_OBJECT  deviceObject = NULL;    // 设备对象


    RtlInitUnicodeString( &ntUnicodeString, NT_DEVICE_NAME );
    // 创建设备
    ntStatus = IoCreateDevice(
        DriverObject,                   // 驱动对象 DriverEntry 的参数
        0,                              // 不使用设备扩展
        &ntUnicodeString,               // 设备名 "\Device\XIOCTL"
        FILE_DEVICE_UNKNOWN,            // 设备类型
        FILE_DEVICE_SECURE_OPEN,   		  // 
        FALSE,                          // 
        &deviceObject );                // 设备对象

    if ( !NT_SUCCESS( ntStatus ) )
    {
        XIOCTL_KDPRINT(("Couldn't create the device object\n"));
        return ntStatus;
    }
    // 初始化处理例程
    DriverObject->MajorFunction[IRP_MJ_CREATE] = XioctlCreateClose;// 创建时会调用
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = XioctlCreateClose;// 关闭时会调用
    // 处理IO控制
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = XioctlDeviceControl;
    DriverObject->DriverUnload = XioctlUnloadDriver;// 卸载时会调用
   
    // WIN32 设备名
    RtlInitUnicodeString( &ntWin32NameString, DOS_DEVICE_NAME );

    // 在设备名和WIN32设备名之间创建符号连接
    ntStatus = IoCreateSymbolicLink(
                        &ntWin32NameString, &ntUnicodeString );

    if ( !NT_SUCCESS( ntStatus ) )
    {
        XIOCTL_KDPRINT(("Couldn't create symbolic link\n"));
        IoDeleteDevice( deviceObject );
    }

    return ntStatus;
}

/*************************************
* XioctlCreateClose
* 功能	驱动对象的处理例程，由DriverEntry指定
*				本函数中驱动对象在创建和关闭时调用的例程。
*				没有实际的功能，只是将状态设置为 成功
**************************************/
NTSTATUS
XioctlCreateClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

{
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    
    IoCompleteRequest( Irp, IO_NO_INCREMENT );
    
    return STATUS_SUCCESS;
}

/*************************************
* XioctlUnloadDriver
* 功能	卸载驱动时调用的例程，
*       删除符号连接，删除设备
**************************************/
VOID
XioctlUnloadDriver(
    IN PDRIVER_OBJECT DriverObject
    )

{
    PDEVICE_OBJECT deviceObject = DriverObject->DeviceObject;
    UNICODE_STRING uniWin32NameString;

    RtlInitUnicodeString( &uniWin32NameString, DOS_DEVICE_NAME );


    // 删除符号连接    
    IoDeleteSymbolicLink( &uniWin32NameString );
    // 删除设备
    if ( deviceObject != NULL )
    {
        IoDeleteDevice( deviceObject );
    }
}

/*************************************
* XioctlDeviceControl
* 功能	处理IO控制的例程
**************************************/
NTSTATUS
XioctlDeviceControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    PIO_STACK_LOCATION  irpSp;// 当前栈的位置
    NTSTATUS            ntStatus = STATUS_SUCCESS;// 执行状态，成功\失败
    ULONG               inBufLength; // 输入缓存大小
    ULONG               outBufLength; // 输出缓存大小
    PCHAR               inBuf, outBuf; // 输入输出缓存
    PCHAR               data = "This String is from Device Driver !!!";
    ULONG               datalen = strlen(data)+1;//输出数据的长度
    PMDL                mdl = NULL;
    PCHAR               buffer = NULL;  

		// 处理IRP
    irpSp = IoGetCurrentIrpStackLocation( Irp );
    inBufLength = irpSp->Parameters.DeviceIoControl.InputBufferLength;
    outBufLength = irpSp->Parameters.DeviceIoControl.OutputBufferLength;

    if(!inBufLength || !outBufLength)
    {
        ntStatus = STATUS_INVALID_PARAMETER;
        goto End;
    }

    // 判断IOCTL
    switch ( irpSp->Parameters.DeviceIoControl.IoControlCode )
    {
    case IOCTL_XIOCTL_BUFFER: 
    		// 显示收到的IRP
        XIOCTL_KDPRINT(("Called IOCTL_SIOCTL_METHOD_BUFFERED\n")); 
        PrintIrpInfo(Irp);

        // 设备IN OUT 缓存
        inBuf = Irp->AssociatedIrp.SystemBuffer;
        outBuf = Irp->AssociatedIrp.SystemBuffer;

        // 从输入缓存中获得信息
        XIOCTL_KDPRINT(("\tData from User : %s", inBuf));
        // 复制数据到输出缓存
        strncpy(outBuf, data, outBufLength);
				// 打印输出输出缓存的内容
        XIOCTL_KDPRINT(("\tData to User : %s", outBuf));
        // 设置IRP 
        Irp->IoStatus.Information = (outBufLength<datalen?outBufLength:datalen);

       break;
		// 还可以定义其他IO 控制码
		
    default:

        // 处理其他示知的IO code
        ntStatus = STATUS_INVALID_DEVICE_REQUEST;
        XIOCTL_KDPRINT(("ERROR: unrecognized IOCTL %x\n",
            irpSp->Parameters.DeviceIoControl.IoControlCode));
        break;
    }

End:
    // 设备状态，完成IPR处理
    Irp->IoStatus.Status = ntStatus;
    IoCompleteRequest( Irp, IO_NO_INCREMENT );
    return ntStatus;
}

/*************************************
* PrintIrpInfo
* 功能	打印IPR信息
**************************************/
VOID
PrintIrpInfo(
    PIRP Irp)
{
    PIO_STACK_LOCATION  irpSp;
    irpSp = IoGetCurrentIrpStackLocation( Irp );

    XIOCTL_KDPRINT(("\tIrp->AssociatedIrp.SystemBuffer = 0x%p\n",
        Irp->AssociatedIrp.SystemBuffer)); 
    XIOCTL_KDPRINT(("\tIrp->UserBuffer = 0x%p\n", Irp->UserBuffer)); 
    XIOCTL_KDPRINT(("\tirpSp->Parameters.DeviceIoControl.Type3InputBuffer = 0x%p\n",
        irpSp->Parameters.DeviceIoControl.Type3InputBuffer)); 
    XIOCTL_KDPRINT(("\tirpSp->Parameters.DeviceIoControl.InputBufferLength = %d\n",
        irpSp->Parameters.DeviceIoControl.InputBufferLength)); 
    XIOCTL_KDPRINT(("\tirpSp->Parameters.DeviceIoControl.OutputBufferLength = %d\n",
        irpSp->Parameters.DeviceIoControl.OutputBufferLength )); 
    return;
}