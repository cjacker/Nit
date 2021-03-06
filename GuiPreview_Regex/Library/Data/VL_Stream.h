/*******************************************************************************
Vczh Library++ 2.0
数据结构::数据流
开发者：陈梓瀚

接口：
  IVL_Stream				：流
  IVL_InputStream			：输入流
  IVL_OutputStream			：输出流
内部类：
  VL_FileStream				：文件流
功能类：
  VL_TextConverter			：提供GBK，Unicode，Unicode Big Endian和UTF-8之间的转换
流类：
  VL_FileInputStream		：文件输入流
  VL_FileOutputStream		：文件输出流
  VL_CachedFileInputStream	：文件输入流
  VL_CachedFileOutputStream	：文件输出流
  VL_MemoryShellStream		：非自维护内存双向流
  VL_MemoryStream			：自维护内存双向流
流控制类：
  VL_TextInput				：具有编码的字符串输入控制
  VL_TextOutput				：具有编码的字符串输出控制
  VL_BinaryInput			：二进制数据输入控制
  VL_BinaryOutput			：二进制数据输出控制
函数：
*******************************************************************************/
#ifndef VL_STREAM
#define VL_STREAM

#define _CRT_SECURE_NO_DEPRECATE
#include "Data\VL_Data_String.h"
#include "Data\VL_Data_List.h"
#include <stdio.h>

namespace vl
{
	namespace stream
	{
		using namespace collection;

		/*字节流的相对位置*/
		enum VLE_StreamPosition
		{
			vspBegin,	//开始位置
			vspEnd,		//结尾位置
			vspCurrent	//当前位置
		};
/*********************************************************************************************************
Stream Interfaces
*********************************************************************************************************/

		/*定义输入输出字节流的共有功能*/
		class IVL_Stream : public IVL_Interface
		{
		public:
			/*返回数据指针的位置*/
			virtual	VInt						Position()=0;
			/*返回流的大小*/
			virtual VInt						Size()=0;
			/*通过绝对位置设置数据指针的位置*/
			virtual void						MoveTo(VInt Pos)=0;
			/*通过相对位置设置数据指针的位置*/
			virtual void						MoveOffset(VInt Offset , VLE_StreamPosition BasePos)=0;
			/*关闭流，用于某些外部接入数据*/
			virtual void						Close()=0;
		};

		/*定义输入字节流的特有功能*/
		class IVL_InputStream : public virtual IVL_Stream
		{
		public:
			/*读取指定长度的数据，并让数据指针往后偏移*/
			virtual VInt						Read(VPointer Data , VInt Count)=0;
			/*返回是否已经到达末尾*/
			virtual VBool						IsEnd()=0;
		};

		/*定义输出字节流的特有功能*/
		class IVL_OutputStream : public virtual IVL_Stream
		{
		public:
			/*写入指定长度的数据，并让数据指针往后偏移*/
			virtual void						Write(VPointer Data , VInt Count)=0;
		};

/*********************************************************************************************************
File Stream
*********************************************************************************************************/

		/*文件流*/
		class VL_FileStream abstract : public VL_Base
		{
		protected:
			FILE*	FStream;

			virtual VInt						_Position();
			virtual VInt						_Size();
			virtual void						_MoveTo(VInt Pos);
			virtual void						_MoveOffset(VInt Offset , VLE_StreamPosition BasePos);
			virtual void						_Close();
		public:
			VL_FileStream();
			~VL_FileStream();

			VBool								Available();
		};

		/*文件输入流*/
		class VL_FileInputStream : public VL_FileStream , public IVL_InputStream
		{
		public:
			VL_FileInputStream(VMbcsString FileName);
			VL_FileInputStream(VUnicodeString FileName);

			VInt								Position();
			VInt								Size();
			void								MoveTo(VInt Pos);
			void								MoveOffset(VInt Offset , VLE_StreamPosition BasePos);
			void								Close();
			VInt								Read(VPointer Data , VInt Count);
			VBool								IsEnd();
		};

		class VL_CachedFileInputStream : public VL_FileInputStream
		{
		protected:
			VInt								FDefaultCacheSize;
			VInt								FCacheSize;
			VInt								FUnreadSize;
			VBuffer								FCache;
			VBuffer								FReadPoint;

			void								FillCache();
			void								Init(VInt CacheSize);
			VInt								_Position();
			void								_MoveTo(VInt Pos);
			void								_MoveOffset(VInt Offset , VLE_StreamPosition BasePos);
		public:
			VL_CachedFileInputStream(VMbcsString FileName , VInt CacheSize);
			VL_CachedFileInputStream(VUnicodeString FileName , VInt CacheSize);
			~VL_CachedFileInputStream();

			VInt								Read(VPointer Data , VInt Count);
		};

		/*文件输出流*/
		class VL_FileOutputStream : public VL_FileStream , public IVL_OutputStream
		{
		public:
			/*Append：为true代表从文件末尾续写，为false则清空文件的内容，并把指针移至开始处*/
			VL_FileOutputStream(VMbcsString FileName , VBool Append);
			VL_FileOutputStream(VUnicodeString FileName , VBool Append);

			VInt								Position();
			VInt								Size();
			void								MoveTo(VInt Pos);
			void								MoveOffset(VInt Offset , VLE_StreamPosition BasePos);
			void								Close();
			void								Write(VPointer Data , VInt Count);
		};

		class VL_CachedFileOutputStream : public VL_FileOutputStream
		{
		protected:
			VInt								FDefaultCacheSize;
			VBuffer								FCache;
			VInt								FUnwriteSize;
			VBuffer								FWritePoint;

			void								FlushCache();
			void								Init(VInt CacheSize);
			VInt								_Position();
			void								_MoveTo(VInt Pos);
			void								_MoveOffset(VInt Offset , VLE_StreamPosition BasePos);
			void								_Close();
		public:
			VL_CachedFileOutputStream(VMbcsString FileName, VBool Append , VInt CacheSize);
			VL_CachedFileOutputStream(VUnicodeString FileName, VBool Append , VInt CacheSize);
			~VL_CachedFileOutputStream();

			void								Write(VPointer Data , VInt Count);
		};

/*********************************************************************************************************
Memory Stream
*********************************************************************************************************/

		/*非自维护内存双向流，用于操作其它地方提供的内存数据*/
		class VL_MemoryShellStream : public IVL_InputStream , public IVL_OutputStream
		{
		protected:
			VPointer							FData;		//内存数据指针
			VInt								FMaxSize;	//有效空间范围
			VInt								FSize;		//使用尺寸
			VInt								FPosition;	//数据指针位置
		public:
			/*
			Data：数据指针
			Size：指针指向的可用范围
			MakeEmpty：为true则让流认为该内存无数据，初始化使用尺寸为0，否则初始化使用尺寸为Size
			*/
			VL_MemoryShellStream(VPointer Data , VInt Size , VBool MakeEmpty);
			~VL_MemoryShellStream();

			VInt								Position();
			VInt								Size();
			void								MoveTo(VInt Pos);
			void								MoveOffset(VInt Offset , VLE_StreamPosition BasePos);
			void								Close();
			void								Write(VPointer Data , VInt Count);
			VInt								Read(VPointer Data , VInt Count);
			VBool								IsEnd();
		};

		/*自维护内存双向流，流自己维护内存空间
		该流维护的内存空间并不是连续的，而是由构造函数的BlockSize指定。
		当用户写入的数据太长使得流维护的内存空间不够时，流会再申请一段BlockSize大小的内存并加入数据块链表。
		*/
		class VL_MemoryStream : public IVL_InputStream , public IVL_OutputStream
		{
		protected:
			/*数据块链表节点*/
			struct VLS_MemoryNode
			{
				VBuffer							Buffer;	//数据块指针
				VLS_MemoryNode*					Next;	//后继节点
				VInt							Usage;	//数据块被使用的大小

				VLS_MemoryNode(VL_MemoryStream* Stream);
				~VLS_MemoryNode();
			};
		protected:
			VL_List<VLS_MemoryNode*,true>		FBufferList;	//数据块链表节点的线性表表示，用于随机存取
			VLS_MemoryNode*						FBufferHead;	//数据块链表头节点
			VLS_MemoryNode*						FBufferTail;	//数据块链表尾节点
			VInt								FBlockSize;		//数据块大小
			VInt								FPosition;		//数据指针位置

			/*创建新的数据块并维护*/
			void								AddMemoryNode();
		public:
			/*BlockSize：指定分块内存的大小*/
			VL_MemoryStream(VInt BlockSize=8*1024);
			~VL_MemoryStream();

			VInt								Position();
			VInt								Size();
			void								MoveTo(VInt Pos);
			void								MoveOffset(VInt Offset , VLE_StreamPosition BasePos);
			void								Close();
			void								Write(VPointer Data , VInt Count);
			VInt								Read(VPointer Data , VInt Count);
			VBool								IsEnd();
		};

/*********************************************************************************************************
Text Stream Controller
*********************************************************************************************************/

		/*编码资料
		UCS-4和UTF-8的对应关系:
		U-00000000 - U-0000007F:  0xxxxxxx
		U-00000080 - U-000007FF:  110xxxxx 10xxxxxx
		U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx
		U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		BOM:
		FFFE	=Unicode			(vceUtf16)
		FEFF	=Unicode Big Endian	(vceUtf16_be)
		EFBBBF	=UTF-8				(vceUtf8)
		other	=MBCS(GBK)			(vceMbcs)
		*/

		/*字符编码*/
		enum VLE_CharEncode
		{
			vceMbcs,
			vceUtf8,
			vceUtf16,
			vceUtf16_be,
			vceBOM		//TextInput：根据BOM获得实际编码方法，其他：等效于vceMbcs
		};

		/*字符编码转换器*/
		class VL_TextConverter : VL_Base
		{
		protected:
			VBuffer								FData;	//字符数据指针
			VInt								FCount;	//字符数据尺寸
			VLE_CharEncode						FEncode;//当前编码类型
		public:
			VL_TextConverter();
			~VL_TextConverter();

			/*输入被转换的字符数据
			Data：字符数据指针
			Count：字符数据尺寸
			Encode：编码类型
			MakeCopy：为true则转换器自己复制一份副本，false则转换器接管数据指针并在使用结束后销毁
			*/
			void								SetText(VPointer Data , VInt Count , VLE_CharEncode Encode , VBool MakeCopy);
			/*根据指定编码类型转换数据*/
			void								Convert(VLE_CharEncode Encode);
			/*转换器维护数据的尺寸*/
			VInt								GetCount();
			/*转换器维护数据的数据指针*/
			VPointer							GetData();
		};

		/*具有编码的字符串输入控制*/
		class VL_TextInput : public VL_Base
		{
		protected:
			IVL_InputStream*					FStream;	//流
			VBool								FOwnStream;	//是否负责管理流的生命周期
			VLE_CharEncode						FEncode;	//编码类型
		public:
			/*
			Stream：数据来源
			OwnStream：为true则控制器负责管理流的生命周期
			Encode：设置数据来源的编码格式，如果为vceBOM则控制器会先读入若干字节的编码描述
			*/
			VL_TextInput(IVL_InputStream* Stream , VBool OwnStream , VLE_CharEncode Encode);
			~VL_TextInput();

			/*获得编码类型*/
			VLE_CharEncode						GetEncode();
			/*读取字符串
			String：结果
			Count：需要读取的数据的字节单位长度，为-1则读入剩下的所有数据
			*/
			void								Read(VMbcsString& String , VInt Count=-1);
			void								Read(VUnicodeString& String , VInt Count=-1);
		};

		/*具有编码的字符串输出控制*/
		class VL_TextOutput : public VL_Base
		{
		protected:
			IVL_OutputStream*					FStream;	//流
			VBool								FOwnStream;	//是否负责管理流的生命周期
			VLE_CharEncode						FEncode;	//编码类型
		public:
			/*
			Stream：目标数据
			OwnStream：为true则控制器负责管理流的生命周期
			Encode：设置输出的的编码格式
			*/
			VL_TextOutput(IVL_OutputStream* Stream , VBool OwnStream , VLE_CharEncode Encode , VBool MakeBOM);
			~VL_TextOutput();

			/*输出字符串
			Buffer：字符串数据指针
			Length：字符串的字符数量，为-1则控制器把Buffer视为NULL结尾的字符串并自动计算长度
			*/
			void								Write(PCChar Buffer , VInt Length=-1);
			void								Write(PCWChar Buffer , VInt Length=-1);
			/*输出字符串*/
			void								Write(VMbcsString String);
			void								Write(VUnicodeString String);
		};

/*********************************************************************************************************
Binary Stream Controller
*********************************************************************************************************/

		/*二进制数据输入控制*/
		class VL_BinaryInput : public VL_Base
		{
		protected:
			IVL_InputStream*					FStream;	//流
			VBool								FOwnStream;	//是否负责管理流的生命周期
		public:
			/*
			Stream：数据来源
			OwnStream：为true则控制器负责管理流的生命周期
			*/
			VL_BinaryInput(IVL_InputStream* Stream , VBool OwnStream);
			~VL_BinaryInput();

			/*读取数据*/
			void								Read(VBool& Data);
			void								Read(VInt8u& Data);
			void								Read(VInt8s& Data);
			void								Read(VInt16u& Data);
			void								Read(VInt16s& Data);
			void								Read(VInt32u& Data);
			void								Read(VInt32s& Data);
			void								Read(VInt64u& Data);
			void								Read(VInt64s& Data);
			void								Read(VFloat& Data);
			void								Read(VDouble& Data);
			/*读取字符串
			Data：结果
			Bytes：需要读取的数据的字节单位长度，如果当前位置有一个VInt类型的描述长度的数据，可令Bytes为-1
			*/
			void								Read(VMbcsString& Data , VInt Bytes=-1);
			void								Read(VUnicodeString& Data , VInt Bytes=-1);
			/*读取字符串
			数据所需格式：[VInt类型的长度描述][字符编码的BOM][字符数据]
			格式可使用VL_BinaryOutput的WriteEncoded自动生成
			*/
			void								ReadEncoded(VMbcsString& Data);
			void								ReadEncoded(VUnicodeString& Data);
		};

		/*二进制数据输出控制*/
		class VL_BinaryOutput : public VL_Base
		{
		protected:
			IVL_OutputStream*					FStream;	//流
			VBool								FOwnStream;	//是否负责管理流的生命周期
		public:
			/*
			Stream：数据来源
			OwnStream：为true则控制器负责管理流的生命周期
			*/
			VL_BinaryOutput(IVL_OutputStream* Stream , VBool OwnStream);
			~VL_BinaryOutput();

			/*输出数据*/
			void								Write(VBool Data);
			void								Write(VInt8u Data);
			void								Write(VInt8s Data);
			void								Write(VInt16u Data);
			void								Write(VInt16s Data);
			void								Write(VInt32u Data);
			void								Write(VInt32s Data);
			void								Write(VInt64u Data);
			void								Write(VInt64s Data);
			void								Write(VFloat Data);
			void								Write(VDouble Data);
			/*输出字符串
			Data：字符串
			WriteCount：为true则先写入一个VInt的数据描述字符串的数据长度
			*/
			void								Write(VMbcsString Data , VBool WriteCount=true);
			void								Write(VUnicodeString Data , VBool WriteCount=true);
			/*输出字符串
			Data：字符串
			Encode：字符编码
			输出的格式：[VInt类型的长度描述][字符编码的BOM][字符数据]
			*/
			void								WriteEncoded(VMbcsString Data , VLE_CharEncode Encode);
			void								WriteEncoded(VUnicodeString Data , VLE_CharEncode Encode);
		};

	}
}

#endif