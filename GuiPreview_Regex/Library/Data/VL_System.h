/*******************************************************************************
Vczh Library++ 2.0
操作系统
开发者：陈梓瀚

内部类：
  VL_BasicFile			：文件信息维护
  VL_Folder				：文件夹信息维护
类：
  VFileName				：文件名分析器
  VTime					：时间
  VL_SynCriticalSection	：锁
  VL_SynEvent			：事件对象
  VL_SynMutex			：互斥对象
  VL_SynSemaphore		：计数对象
  VL_Thread				：线程类
函数：
  WaitFor
  WaitForSynObjects
  WaitForSynObjectsTimeOut
*******************************************************************************/

#ifndef VL_SYSTEM
#define VL_SYSTEM

#include "Data\VL_Data_String.h"
#include "Data\VL_Data_List.h"
#include <time.h>
#include <windows.h>

namespace vl
{
	namespace system
	{
		using namespace collection;

		class VFileName
		{
		protected:
			VUnicodeString							FFileName;
			VUnicodeString							_GetPath();
			VUnicodeString							_GetName();
		public:
			VFileName(VMbcsString FileName);
			VFileName(VUnicodeString FileName);
			VFileName(PChar FileName);
			VFileName(PWChar FileName);

			VFileName&								operator = (VFileName FileName);
			VFileName&								operator = (VUnicodeString FileName);
			VFileName&								operator = (VMbcsString FileName);

			VFileName								MakeRelative(VFileName Dest);
			VFileName								MakeAbsolute(VFileName Dest);
			void									Back(VInt Count);
			void									Join(VFileName Part);
			VFileName								GetPath();
			VFileName								GetName();
			VMbcsString								GetStrA();
			VUnicodeString							GetStrW();
		};

		class VTime
		{
		public:
			VInt									Year,Month,Day;
			VInt									Hour,Minute,Second,MSec;
			VInt									WeekDay;

			VTime(time_t SecTime);
			VTime(tm CrtTime);
			VTime(FILETIME FileTime);
			VTime(SYSTEMTIME SystemTime);
			VTime(VTime& Time);
			VTime();

			VTime&									operator=(time_t SecTime);
			VTime&									operator=(tm CrtTime);
			VTime&									operator=(FILETIME FileTime);
			VTime&									operator=(SYSTEMTIME SystemTime);
			VTime&									operator=(VTime& Time);

			void									PickTime();
			VUnicodeString							DateString();
			VUnicodeString							TimeString(VBool UseMSec);
			VUnicodeString							WeekString();
		};

/*********************************************************************************************************
Multitasking
*********************************************************************************************************/

		namespace synchronization
		{
			class VL_SynCriticalSection : public VL_Base
			{
			protected:
				CRITICAL_SECTION					FCriticalSection;
			public:
				VL_SynCriticalSection();
				~VL_SynCriticalSection();

				void								Enter();
				VBool								TryEnter();
				void								Leave();
			};

			class VL_SynObject : public VL_Base
			{
			public:
				typedef VL_AutoPtr<VL_SynObject>				Ptr;
				typedef VL_List<Ptr , false , VL_SynObject*>	List;

				enum AccessResult
				{
					arSucceed,
					arAlreadyExists,
					arFail
				};

				enum WaitResult
				{
					wrAbandoned,
					wrTimeOut,
					wrSignaled
				};
			protected:
				HANDLE								FHandle;
				VBool								FAlreadyExists;
				VBool								FNeedToClose;
				VUnicodeString						FName;

				virtual HANDLE						CreateHandle(VPointer Parameter , PWChar Name)=0;
				AccessResult						CreateObject(VPointer Parameter , VUnicodeString Name);
				
				virtual HANDLE						OpenHandle(VPointer Parameter , PWChar Name)=0;
				AccessResult						OpenObject(VPointer Parameter , VUnicodeString);
			public:
				VL_SynObject();
				~VL_SynObject();

				HANDLE								GetHandle();
				WaitResult							WaitFor(DWORD Milliseconds=INFINITE);
				VBool								IsSignaled();
				VBool								IsAlreadyExists();					//true代表名字已经被其他地方使用
				VBool								IsReady();							//true代表对象可用
				void								Close();
				VUnicodeString						GetName();
			};
			extern void								WaitForSynObjects(VL_SynObject::List& SynObjects);
			extern VBool							WaitForSynObjectsTimeOut(VL_SynObject::List& SynObjects , DWORD Milliseconds);

			class VL_SynEvent : public VL_SynObject
			{
			protected:
				HANDLE								CreateHandle(VPointer Parameter , PWChar Name);
				HANDLE								OpenHandle(VPointer Parameter , PWChar Name);
			public:
				AccessResult						Create(VBool Signal , VBool AutoRest , VUnicodeString Name=L"");
				AccessResult						Open(VUnicodeString Name);

				VBool								Signal();
				VBool								Unsignal();
			};

			class VL_SynMutex : public VL_SynObject
			{
			protected:
				HANDLE								CreateHandle(VPointer Parameter , PWChar Name);
				HANDLE								OpenHandle(VPointer Parameter , PWChar Name);
			public:
				AccessResult						Create(VBool Own , VUnicodeString Name=L"");
				AccessResult						Open(VUnicodeString Name);

				VBool								IsOwned();
				VBool								Request();
				void								Release();
			};

			class VL_SynSemaphore : public VL_SynObject
			{
			protected:
				HANDLE								CreateHandle(VPointer Parameter , PWChar Name);
				HANDLE								OpenHandle(VPointer Parameter , PWChar Name);
			public:
				AccessResult						Create(VInt InitCount , VInt MaxCount , VUnicodeString Name);
				AccessResult						Open(VUnicodeString Name);

				VBool								Increase(VInt Count);
				VBool								Decrease();
			};
		}

		class VL_Thread : public VL_Base
		{
		protected:
			VBool									FFreeOnTerminated;	//结束时删除线程对象
			VBool									FSuspended;			//是否暂停
			VBool									FNeedToTerminate;	//是否需要中止，由线成函数读取并决定
			VBool									FTerminated;		//是否已经中止

			HANDLE									FHandle;

			virtual void							Run()=0;
			static DWORD							WINAPI ExecuteThread(LPVOID Parameter);
		public:
			VL_Thread(VBool CreateSuspended , VBool FreeOnTerminated);
			~VL_Thread();

			void									Execute();	//执行
			void									Suspend();	//暂停
			void									Resume();	//恢复
			void									WaitFor();	//等待

			VBool									GetFreeOnTerminated();
			void									SetFreeOnTerminated(VBool FreeOnTerminated);
			VBool									GetNeedToTerminate();
			void									SetNeedToTerminate(VBool NeedToTerminate);
			VBool									IsSuspended();
			VBool									IsTerminated();
			HANDLE									GetHandle();
		};
		
		typedef void (*VLFP_ThreadProc)(VPointer Parameter);

		class VL_ProcThread : public VL_Thread
		{
		protected:
			VLFP_ThreadProc							FProc;
			VPointer								FParameter;

			void Run();
		public:
			VL_ProcThread(VLFP_ThreadProc Proc , VPointer Parameter , VBool CreateSuspended , VBool FreeOnTerminated);
		};

		typedef VL_List<VL_Thread* , true> VL_ThreadList;
		typedef VL_List<VL_AutoPtr<VL_Thread> , false , VL_Thread*> VL_ManagedThreadList;

		extern void WaitFor(VL_ThreadList& Threads , VBool Execute);
		extern void WaitFor(VL_ManagedThreadList& Threads , VBool Execute);

/*********************************************************************************************************
File System
*********************************************************************************************************/

		struct VLS_VFSO_VolumeInformation
		{
			VUnicodeString							Label;
			VUInt									SerialNumber;
			VInt									MaximumComponentLength;
			VUnicodeString							FileSystem;

			VBool									SupportNamedStream;
			VBool									ReadOnly;
			VBool									CaseSensitive;
			VBool									SupportFileCompression;
			VBool									SupportFileEncryption;
			VBool									SupportUnicodeFileName;
			VBool									VolumeCompressed;
		};

		enum VLE_VFSO_VolumeType
		{
			vvtUnknown,
			vvtNoRootDir,
			vvtRemovable,
			vvtFixed,
			vvtRemote,
			vvtCDRom,
			vvtRamDisk
		};

		struct VLS_VFSO_FileAttributes
		{
			VBool									Archive;
			VBool									Compressed;
			VBool									Directory;
			VBool									Encrypted;
			VBool									Hidden;
			VBool									Readonly;
			VBool									System;
			VBool									Temporary;
		};

		extern VL_Array<VUnicodeString , 1>			VFSO_GetLogicalDrives		();
		extern VBool								VFSO_GetVolumeInformation	(VUnicodeString Path , VLS_VFSO_VolumeInformation& Information);
		extern VLE_VFSO_VolumeType					VFSO_GetVolumeType			(VUnicodeString Path);

		enum VLE_VFSO_CreateDirectory
		{
			vcdSucceed,
			vcdPathExists,
			vcdNotFound
		};
		extern VLE_VFSO_CreateDirectory				VFSO_CreateDirectory		(VUnicodeString Path);
		extern VBool								VFSO_RemoveDirectory		(VUnicodeString Path);
		extern VBool								VFSO_CopyFile				(VUnicodeString OldFile , VUnicodeString NewFile , VBool FailIfExists);
		extern VBool								VFSO_MoveFile				(VUnicodeString OldFile , VUnicodeString NewFile);
		extern VBool								VFSO_DeleteFile				(VUnicodeString File);
		extern VBool								VFSO_GetFileAttributes		(VUnicodeString File , VLS_VFSO_FileAttributes& Attributes);
		extern VBool								VFSO_SetFileAttributes		(VUnicodeString File , VLS_VFSO_FileAttributes& Attributes);
		extern VInt64s								VFSO_GetFileSize			(VUnicodeString File);
		extern VBool								VFSO_DirectoryExists		(VUnicodeString File);
		extern VBool								VFSO_FileExists				(VUnicodeString File);
		extern VBool								VFSO_EnumerateFiles			(VUnicodeString Path , VL_List<VUnicodeString , false>& Files , VL_List<VUnicodeString , false>& Directories);

	}
}

#endif