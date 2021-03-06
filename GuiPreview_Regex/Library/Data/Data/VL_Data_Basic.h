#ifndef VL_DATA_BASIC
#define VL_DATA_BASIC

#pragma unmanaged

#define _CRT_SECURE_NO_DEPRECATE
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <typeinfo.h>

namespace vl
{
	typedef unsigned __int8			VByte;
	typedef	__int32					VInt;
	typedef unsigned __int32		VUInt;
	typedef	double					VDouble;
	typedef	float					VFloat;
	typedef bool					VBool;
	typedef void*					VPointer;
	typedef VByte*					VBuffer;

	typedef wchar_t					VWChar;
	typedef wchar_t*				PWChar;
	typedef const wchar_t			VCWChar;
	typedef const wchar_t*			PCWChar;

	typedef unsigned char			V8Char;
	typedef unsigned char*			P8Char;
	typedef const unsigned char		VC8Char;
	typedef const unsigned char*	PC8Char;

	typedef char					VChar;
	typedef char*					PChar;
	typedef const char				VCChar;
	typedef const char*				PCChar;

	typedef unsigned	__int8		VInt8u;		//VByte
	typedef signed		__int8		VInt8s;
	typedef unsigned	__int16		VInt16u;
	typedef signed		__int16		VInt16s;
	typedef unsigned	__int32		VInt32u;
	typedef signed		__int32		VInt32s;	//VInt
	typedef unsigned	__int64		VInt64u;
	typedef signed		__int64		VInt64s;

	extern const VFloat Pi_f;

	/*四舍五入*/
	extern VInt Round(VDouble Num);
	extern VDouble Roundf(VDouble Num);
	/*取整*/
	extern VInt Trunc(VDouble Num);
	extern VDouble Truncf(VDouble Num);

	class VL_Base
	{
	public:
		virtual ~VL_Base(){};
	};

	class IVL_Interface
	{
	public:
		virtual ~IVL_Interface(){};
	};

/*********************************************************************************************************
Element
*********************************************************************************************************/
/*
	VL_AutoPtr的作用是可以把对指针释放时机的决策交给一个动态的决策系统。
	VL_AutoPtr在构造的时候可以接受各种类的指针（不可一是new []创建的直接对象）、各种类型的智能指针。
	在构造的时候如果遇到不同类型的智能指针，则智能指针会在内部尝试进行类型转换，失败的时候接受0作为指针值。
	VL_AutoPtr有两个缺陷：
	1：不可处理循环引用；
	2：不可多次有一个指针直接构造，除了第一次的智能指针构造之外，其它的智能指针必须有智能指针构造，否则会在释放的时候发生重复释放的错误。

	template<typename _Type>
	class VL_AutoPtr : public VL_Base
	{
		typedef VL_AutoPtr<_Type> _Owner;
	protected:
	public:
		VL_AutoPtr	();
		VL_AutoPtr	(const _Owner& Owner);
		template<typename T>
		VL_AutoPtr	(VL_AutoPtr<T> Ptr);
		VL_AutoPtr	(_Type* Object);
		~VL_AutoPtr	();

		_Owner&		operator = (_Type* Object);			赋值
		_Owner&		operator = (_Owner& Owner);			赋值
		operator	VBool();							返回智能指针内部是否包含指针
		_Type*		Object();							返回智能指针的指针值
		VBool		operator == (_Owner& Owner);		比较两个智能指针是否等价
		VBool		operator == (_Type* Object);		比较智能指针所包含的数据跟对象是否一致
		VBool		operator != (_Owner& Owner);
		VBool		operator != (_Type* Object);
		VBool		operator < (_Owner& Owner);
		VBool		operator > (_Owner& Owner);
		VBool		operator <= (_Owner& Owner);
		VBool		operator >= (_Owner& Owner);
		_Type*		operator -> ();
		VBool		IsOwned();							返回智能指针是否被控制
		void		SetOwned(VBool Owned);				设置智能指针的控制属性，为false则智能指针不负责指针的释放
		_Type*		Disconnect();						取消控制，返回指针
		VInt*		AutoPtrGetReferenceCount();			获得引用计数的引用
		VBool*		AutoPtrGetOwned();					获得是否控制的引用
		template<typename T>
		_Owner&		operator = (VL_AutoPtr<T> Ptr);		跨类型智能指针复制
	};
*/
	template<typename _Type>
	class VL_AutoPtr : public VL_Base
	{
		typedef VL_AutoPtr<_Type> _Owner;
	protected:
		VInt*	FReferenceCount;	/*引用计数指针，由所有相互连通的智能指针共享*/
		VBool*	FOwned;				/*控制标记由所有相互连通的智能指针共享*/
		_Type*	FObject;			/*智能指针的控制对象*/

		/*初始化为空对象*/
		void Init()
		{
			FReferenceCount=0;
			FOwned=0;
			FObject=0;
		}
		/*增加引用计数*/
		void Inc()
		{
			if(FReferenceCount)
			{
				(*FReferenceCount)++;
			}
		}
		/*减少引用计数，并在合适的实际释放对象*/
		void Dec()
		{
			if(FReferenceCount)
			{
				if(!--*FReferenceCount)
				{
					if(FObject&&*FOwned)
					{
						delete FObject;
					}
					delete FReferenceCount;
					delete FOwned;
					Init();
				}
			}
		}
		/*修改对象*/
		void SetObject(_Type* Object)
		{
			Dec();
			if(Object)
			{
				FReferenceCount=new VInt(1);
				FOwned=new VBool(true);
				FObject=Object;
			}
			else
			{
				Init();
			}
		}
	public:
		VL_AutoPtr()
		{
			Init();
		}
		VL_AutoPtr(const _Owner& Owner)
		{
			FReferenceCount=Owner.FReferenceCount;
			FOwned=Owner.FOwned;
			FObject=Owner.FObject;
			Inc();
		}
		template<typename T>
		VL_AutoPtr(VL_AutoPtr<T> Ptr)
		{
			_Type* Casted_Object=dynamic_cast<_Type*>(Ptr.Object());
			if(Casted_Object)
			{
				FReferenceCount=Ptr.AutoPtrGetReferenceCount();
				FOwned=Ptr.AutoPtrGetOwned();
				FObject=Casted_Object;
				Inc();
			}
			else
			{
				Init();
				SetObject(0);
			}
		}
		VL_AutoPtr(_Type* Object)
		{
			Init();
			SetObject(Object);
		}
		~VL_AutoPtr()
		{
			Dec();
		}

		_Owner& operator = (_Type* Object)
		{
			SetObject(Object);
			return *this;
		}
		_Owner& operator = (const _Owner& Owner)
		{
			if(this!=&Owner)
			{
				Dec();
				FReferenceCount=Owner.FReferenceCount;
				FOwned=Owner.FOwned;
				FObject=Owner.FObject;
				Inc();
			}
			return *this;
		}
		operator VBool()const
		{
			return FObject!=0;
		}
		_Type* Object()const
		{
			return FObject;
		}

		VBool operator == (_Owner& Owner)const
		{
			return FReferenceCount==Owner.FReferenceCount;
		}
		VBool operator == (_Type* Object)const
		{
			return FObject==Object;
		}
		VBool operator != (_Owner& Owner)const
		{
			return *this!=Owner;
		}
		VBool operator != (_Type* Object)const
		{
			return *this!=Object;
		}

		VBool operator < (_Owner& Owner)const
		{
			return FObject<Owner.FObject;
		}
		VBool operator > (_Owner& Owner)const
		{
			return FObject>Owner.FObject;
		}
		VBool operator <= (_Owner& Owner)const
		{
			return FObject<=Owner.FObject;
		}
		VBool operator >= (_Owner& Owner)const
		{
			return FObject>=Owner.FObject;
		}

		_Type* operator -> ()const
		{
			return FObject;
		}

		VBool IsOwned()const
		{
			if(FReferenceCount)
			{
				return *FOwned;
			}
			else
			{
				return false;
			}
		}
		void SetOwned(VBool Owned)
		{
			if(FReferenceCount)
			{
				*FOwned=Owned;
			}
		}
		_Type* Disconnect()
		{
			SetOwned(false);
			return FObject;
		}
		VInt* AutoPtrGetReferenceCount()const
		{
			return FReferenceCount;
		}
		VBool* AutoPtrGetOwned()const
		{
			return FOwned;
		}
		template<typename T>
		_Owner& operator = (VL_AutoPtr<T> Ptr)
		{
			_Type* Casted_Object=dynamic_cast<_Type*>(Ptr.Object());
			if(Casted_Object)
			{
				Dec();
				FReferenceCount=Ptr.AutoPtrGetReferenceCount();
				FOwned=Ptr.AutoPtrGetOwned();
				FObject=Casted_Object;
				Inc();
			}
			else
			{
				SetObject(0);
			}
			return *this;
		}
	};
/*
	VL_Compair的作用是可以把有序容器修改成比较Key的Key-Value容器。

	template<typename _Key , typename _Value>
	class VL_Compair : public VL_Base
	{
		typedef VL_Compair<_Key , _Value> _Compair;
	public:
		_Key		Key;
		_Value		Value;

		_Compair	();
		_Compair	(_Compair& Compair);
		_Compair&	operator = (_Compair& Compair);		比较函数只比较Key的大小
		VBool		operator < (_Compair& c);
		VBool		operator > (_Compair& c);
		VBool		operator == (_Compair& c);
		VBool		operator <= (_Compair& c);
		VBool		operator >= (_Compair& c);
		VBool		operator != (_Compair& c);
		VBool		operator < (_Key& c);				比较函数值比较Key的大小
		VBool		operator > (_Key& c);
		VBool		operator == (_Key& c);
		VBool		operator <= (_Key& c);
		VBool		operator >= (_Key& c);
		VBool		operator != (_Key& c);
	};
*/
	template<typename _Key , typename _Value>
	class VL_Compair : public VL_Base
	{
		typedef VL_Compair<_Key , _Value> _Compair;
	public:
		typedef _Key	KeyType;
		typedef _Value	ValueType;

		_Key	Key;
		_Value	Value;

		_Compair()
		{
		}

		_Compair(const _Compair& Compair)
		{
			Key=Compair.Key;
			Value=Compair.Value;
		}

		_Compair& operator = (const _Compair& Compair)
		{
			Key=Compair.Key;
			Value=Compair.Value;
			return *this;
		}

		VBool operator < (const _Compair& c)const
		{
			return Key<c.Key;
		}
		VBool operator > (const _Compair& c)const
		{
			return Key>c.Key;
		}
		VBool operator == (const _Compair& c)const
		{
			return Key==c.Key;
		}
		VBool operator <= (const _Compair& c)const
		{
			return Key<=c.Key;
		}
		VBool operator >= (const _Compair& c)const
		{
			return Key>=c.Key;
		}
		VBool operator != (const _Compair& c)const
		{
			return Key!=c.Key;
		}
		
		VBool operator < (const _Key& c)const
		{
			return Key<c;
		}
		VBool operator > (const _Key& c)const
		{
			return Key>c;
		}
		VBool operator == (const _Key& c)const
		{
			return Key==c;
		}
		VBool operator <= (const _Key& c)const
		{
			return Key<=c;
		}
		VBool operator >= (const _Key& c)const
		{
			return Key>=c;
		}
		VBool operator != (const _Key& c)const
		{
			return Key!=c;
		}
	};
/*
	VL_Fullpair的作用是用于构造一个类型不同的二维可比较向量

	template<typename _Key , typename _Value>
	class VL_Fullpair : public VL_Base
	{
		typedef VL_Fullpair<_Key , _Value> _Fullpair;
	public:
		_Key		Key;
		_Value		Value;

		_Fullpair	();
		_Fullpair	(_Fullpair& _Fullpair);
		_Fullpair&	operator = (_Fullpair& Fullpair);		比较函数首先比较Key的大小
		VBool		operator < (_Fullpair& f);
		VBool		operator > (_Fullpair& f);
		VBool		operator == (_Fullpair& f);
		VBool		operator <= (_Fullpair& f);
		VBool		operator >= (_Fullpair& f);
		VBool		operator != (_Fullpair& f);
	};
*/
	template<typename _Key , typename _Value>
	class VL_Fullpair : public VL_Base
	{
		typedef VL_Fullpair<_Key , _Value> _Fullpair;
	public:
		typedef _Key	KeyType;
		typedef _Value	ValueType;

		_Key	Key;
		_Value	Value;

		_Fullpair()
		{
		}

		_Fullpair(const _Fullpair& Fullpair)
		{
			Key=Fullpair.Key;
			Value=Fullpair.Value;
		}

		_Fullpair& operator = (const _Fullpair& Fullpair)
		{
			Key=Fullpair.Key;
			Value=Fullpair.Value;
			return *this;
		}

		VBool operator < (const _Fullpair& f)const
		{
			if(Key<f.Key)
			{
				return true;
			}
			else if(Key==f.Key)
			{
				return Value<f.Value;
			}
			else
			{
				return false;
			}
		}

		VBool operator <= (const _Fullpair& f)const
		{
			if(Key<f.Key)
			{
				return true;
			}
			else if(Key==f.Key)
			{
				return Value<=f.Value;
			}
			else
			{
				return false;
			}
		}

		VBool operator > (const _Fullpair& f)const
		{
			if(Key>f.Key)
			{
				return true;
			}
			else if(Key==f.Key)
			{
				return Value>f.Value;
			}
			else
			{
				return false;
			}
		}

		VBool operator >= (const _Fullpair& f)const
		{
			if(Key>f.Key)
			{
				return true;
			}
			else if(Key==f.Key)
			{
				return Value>=f.Value;
			}
			else
			{
				return false;
			}
		}

		VBool operator == (const _Fullpair& f)const
		{
			if(Key==f.Key)
			{
				return Value==f.Value;
			}
			else
			{
				return false;
			}
		}

		VBool operator != (const _Fullpair& f)const
		{
			if(Key!=f.Key)
			{
				return true;
			}
			else
			{
				return Value!=f.Value;
			}
		}
	};
/*
	VL_Comvec的作用是描述一个有比较关系的向量，该响亮的所有对象的类型是相同的。
	决定不同的同类型VL_Comvec大小的方法如下，先比较第一个元素，如果相同则比较下一个，否则当前结果就是这次比较的结果

	template<typename _Type , int _Count>
	class VL_Comvec : public VL_Base
	{
		typedef VL_Comvec<_Type , _Count> _Comvec;
	public:
		VL_Comvec	();
		VL_Comvec	(_Comvec& Comvec);
		_Comvec&	operator = (_Comvec& Comvec);
		_Type&		operator [] (VInt Index);
		VBool		operator < (_Comvec& c);
		VBool		operator > (_Comvec& c);
		VBool		operator == (_Comvec& c);
		VBool		operator <= (_Comvec& c);
		VBool		operator >= (_Comvec& c);
		VBool		operator != (_Comvec& c);
	};
*/
	template<typename _Type , int _Count>
	class VL_Comvec : public VL_Base
	{
		typedef VL_Comvec<_Type , _Count> _Comvec;
	protected:
		_Type	FElements[_Count];
	public:
		VL_Comvec()
		{
		}
		VL_Comvec(const _Comvec& Comvec)
		{
			for(VInt i=0;i<_Count;i++)
			{
				FElements[i]=Comvec.FElements[i];
			}
		}

		_Comvec& operator = (const _Comvec& Comvec)
		{
			for(VInt i=0;i<_Count;i++)
			{
				FElements[i]=Comvec.FElements[i];
			}
			return *this;
		}

		_Type& operator [] (VInt Index)
		{
			return FElements[Index];
		}

		VBool operator < (const _Comvec& c)const
		{
			for(VInt i=0;i<_Count;i++)
			{
				if(FElements[i]<c.FElements[i])
				{
					return true;
				}
				else if(FElements[i]>c.FElements[i])
				{
					return false;
				}
			}
			return false;
		}
		VBool operator > (const _Comvec& c)const
		{
			for(VInt i=0;i<_Count;i++)
			{
				if(FElements[i]<c.FElements[i])
				{
					return false;
				}
				else if(FElements[i]>c.FElements[i])
				{
					return true;
				}
			}
			return false;
		}
		VBool operator == (const _Comvec& c)const
		{
			for(VInt i=0;i<_Count;i++)
			{
				if(FElements[i]!=c.FElements[i])
				{
					return false;
				}
			}
			return true;
		}
		VBool operator <= (const _Comvec& c)const
		{
			for(VInt i=0;i<_Count;i++)
			{
				if(FElements[i]<c.FElements[i])
				{
					return true;
				}
				else if(FElements[i]>c.FElements[i])
				{
					return false;
				}
			}
			return true;
		}
		VBool operator >= (const _Comvec& c)const
		{
			for(VInt i=0;i<_Count;i++)
			{
				if(FElements[i]<c.FElements[i])
				{
					return false;
				}
				else if(Flements[i]>c.FElements[i])
				{
					return true;
				}
			}
			return true;
		}
		VBool operator != (const _Comvec& c)const
		{
			for(VInt i=0;i<_Count;i++)
			{
				if(FElements[i]!=c.FElements[i])
				{
					return true;
				}
			}
			return false;
		}
	};

/*
	template<typename _Type , VInt _Dims>
	{
	public:
		VL_Array();
		VL_Array(VL_ArrayInfo<_Type>* Info , _Type* Elements);
		VL_Array(VL_ArrayDimension<vacNewArray , _Dims& Dims);
		VL_Array(VL_Array<_Type , _Dims>& Array);
		~VL_Array();

		VL_Array<_Type , _Dims>&	operator =(VL_Array<_Type , _Dims>& Array);
		VL_ArrayReference<_Dims-1>	operator [](VInt Dims);
		VL_Array<_Type , _Dims>		Reference();
		VL_Array<_Type , _Dims>		Clone();
		void						Copy(VL_Array<_Type , _Dims>& Array);
		VL_ArrayInfo<_Type>*		GetInfo();
		_Type*						GetElements();
		VInt						GetCount();
	};
*/
	enum VLE_ArrayCommander
	{
		vacNewArray
	};

	template<VLE_ArrayCommander _Commander , VInt _Dims>
	class VL_ArrayDimension : public VL_Base
	{
	public:
		VInt	Dims[_Dims+1];

		VL_ArrayDimension()
		{
			for(VInt i=0;i<_Dims;i++)
			{
				Dims[i]=0;
			}
		}

		VL_ArrayDimension(VInt Dim , VInt* PrevDims)
		{
			Dims[0]=Dim;
			memcpy(Dims+1,PrevDims,sizeof(VInt)*(_Dims-1));
		}

		VL_ArrayDimension<_Commander , _Dims+1> operator [](VInt Dim)
		{
			return VL_ArrayDimension<_Commander , _Dims+1>(Dim,Dims);
		}

		VInt GetSize()
		{
			VInt Size=1;
			for(VInt i=0;i<_Dims;i++)
			{
				Size*=Dims[i];
			}
			return Size;
		}
	};

	extern VL_ArrayDimension<vacNewArray , 0> NewArray;

	template<typename _Type>
	class VL_ArrayInfo : public VL_Base
	{
	public:
		_Type*	Elements;
		VInt*	Dims;
		VInt	RefCount;
		VInt	Size;

		VL_ArrayInfo(VInt aDims)
		{
			Dims=new VInt[aDims];
		}

		~VL_ArrayInfo()
		{
			delete[] Dims;
		}
	};

	template<typename _Type , VInt _Dims>
	class VL_Array : public VL_Base
	{
	protected:

	public:

		template<VInt _RefDims>
		class VL_ArrayReference : public VL_Base
		{
			friend class VL_Array;
		protected:
			VL_ArrayInfo<_Type>*	FInfo;
			_Type*					FElements;
			VInt					FSubSize;
		public:

			VL_ArrayReference(_Type* Elements , VL_ArrayInfo<_Type>* Info , VInt Dim)
			{
				FInfo=Info;
				FSubSize=FInfo->Size/FInfo->Dims[_RefDims];
				FElements=Elements+Dim*FSubSize;
			}

			VL_ArrayReference(VL_ArrayInfo<_Type>* Info , _Type* Elements , VInt SubSize)
			{
				FInfo=Info;
				FElements=Elements;
				FSubSize=SubSize;
			}

			VL_ArrayReference<_RefDims-1> operator [](VInt Dim)
			{
				VInt SubSize=FSubSize/FInfo->Dims[_RefDims-1];
				return VL_ArrayReference<_RefDims-1>(FInfo,FElements+Dim*SubSize,SubSize);
			}

			operator VL_Array<_Type , _RefDims>()
			{
				return VL_Array<_Type , _Dims-1>(FInfo,FElements);
			}

			VL_Array<_Type , _RefDims> Reference()
			{
				return VL_Array<_Type , _Dims-1>(FInfo,FElements);
			}

			VL_Array<_Type , _RefDims> Clone()
			{
				return VL_Array<_Type , _Dims-1>(FInfo,FElements).Clone();
			}

			void Copy(VL_Array<_Type , _RefDims>& Array)
			{
				VL_ArrayInfo<_Type>* ArrayInfo=Array.GetInfo();
				VInt Count=FInfo->Dims[_RefDims-1];
				if(Count>ArrayInfo->Dims[_RefDims-1])
				{
					Count=ArrayInfo->Dims[_RefDims-1];
				}
				for(VInt i=0;i<Count;i++)
				{
					operator [](i).Copy(Array[i].Reference());
				}
			}

			VInt GetCount()
			{
				return FInfo->Dims[_RefDims-1];
			}
		};

		template<>
		class VL_ArrayReference<1> : public VL_Base
		{
		protected:
			VL_ArrayInfo<_Type>*	FInfo;
			_Type*					FElements;
		public:

			VL_ArrayReference(_Type* Elements , VL_ArrayInfo<_Type>* Info , VInt Dim)
			{
				FInfo=Info;
				FElements=Elements+Dim*FInfo->Size/FInfo->Dims[1];
			}

			VL_ArrayReference(VL_ArrayInfo<_Type>* Info , _Type* Elements , VInt SubSize)
			{
				FInfo=Info;
				FElements=Elements;
			}

			_Type& operator [](VInt Dim)
			{
				return FElements[Dim];
			}

			operator VL_Array<_Type , 1>()
			{
				return VL_Array<_Type , 1>(FInfo,FElements);
			}

			VL_Array<_Type , 1> Reference()
			{
				return VL_Array<_Type , 1>(FInfo,FElements);
			}

			VL_Array<_Type , 1> Clone()
			{
				return VL_Array<_Type , 1>(FInfo,FElements).Clone();
			}

			void Copy(VL_Array<_Type , 1>& Array)
			{
				VL_ArrayInfo<_Type>* ArrayInfo=Array.GetInfo();
				_Type* ArrayElements=Array.GetElements();
				VInt Count=FInfo->Dims[0];
				if(Count>ArrayInfo->Dims[0])
				{
					Count=ArrayInfo->Dims[0];
				}
				for(VInt i=0;i<Count;i++)
				{
					FElements[i]=ArrayElements[i];
				}
			}

			VInt GetCount()
			{
				return FInfo->Dims[0];
			}
		};

	protected:
		VL_ArrayInfo<_Type>*	FInfo;
		_Type*					FElements;

		void Create(VInt Size)
		{
			FInfo=new VL_ArrayInfo<_Type>(_Dims);
			FInfo->Elements=new _Type[Size];
			FInfo->RefCount=1;
			FInfo->Size=Size;
			if(Size==0)
			{
				for(VInt i=0;i<_Dims;i++)
				{
					FInfo->Dims[i]=0;
				}
			}
			FElements=FInfo->Elements;
		}

		void Inc()
		{
			FInfo->RefCount++;
		}

		void Dec()
		{
			if(!--FInfo->RefCount)
			{
				delete[] FInfo->Elements;
				delete FInfo;
				FInfo=0;
				FElements=0;
			}
		}

		VL_Array(VInt Size)
		{
			Create(Size);
		}

	public:

		VL_Array()
		{
			Create(0);
		}

		VL_Array(VL_ArrayInfo<_Type>* Info , _Type* Elements)
		{
			FInfo=Info;
			FElements=Elements;
			Inc();
		}

		VL_Array(VL_ArrayDimension<vacNewArray , _Dims>& Dims)
		{
			Create(Dims.GetSize());
			memcpy(FInfo->Dims,Dims.Dims,sizeof(VInt)*_Dims);
		}

		~VL_Array()
		{
			Dec();
		}

		VL_Array(VL_Array<_Type , _Dims>& Array)
		{
			FInfo=Array.FInfo;
			FElements=Array.FElements;
			Inc();
		}

		VL_Array<_Type , _Dims>& operator =(VL_Array<_Type , _Dims>& Array)
		{
			Dec();
			FInfo=Array.FInfo;
			FElements=Array.FElements;
			Inc();
			return *this;
		}

		VL_ArrayReference<_Dims-1> operator [](VInt Dim)
		{
			return VL_ArrayReference<_Dims-1>(FElements,FInfo,Dim);
		}

		VL_Array<_Type , _Dims> Reference()
		{
			return *this;
		}

		VL_Array<_Type , _Dims> Clone()
		{
			VL_Array<_Type , _Dims> Array(FInfo->Size);
			for(VInt i=0;i<FInfo->Size;i++)
			{
				Array.FElements[i]=FElements[i];
			}
			memcpy(Array.FInfo->Dims,FInfo->Dims,sizeof(VInt)*_Dims);
			return Array;
		};

		void Copy(VL_Array<_Type , _Dims>& Array)
		{
			operator =(Array.Clone());
		}

		VL_ArrayInfo<_Type>* GetInfo()
		{
			return FInfo;
		}

		_Type* GetElements()
		{
			return FElements;
		}

		VInt GetCount()
		{
			return FInfo->Dims[_Dims-1];
		}
	};

	template<typename _Type>
	class VL_Array<_Type , 1> : public VL_Base
	{
	protected:
		VL_ArrayInfo<_Type>*	FInfo;
		_Type*					FElements;

		void Create(VInt Size)
		{
			FInfo=new VL_ArrayInfo<_Type>(1);
			FInfo->Elements=new _Type[Size];
			FInfo->RefCount=1;
			FInfo->Size=Size;
			FInfo->Dims[0]=0;
			FElements=FInfo->Elements;
		}

		void Inc()
		{
			FInfo->RefCount++;
		}

		void Dec()
		{
			if(!--FInfo->RefCount)
			{
				delete[] FInfo->Elements;
				delete FInfo;
				FInfo=0;
				FElements=0;
			}
		}

		VL_Array(VInt Size)
		{
			Create(Size);
		}

	public:

		VL_Array()
		{
			Create(0);
		}

		VL_Array(VL_ArrayInfo<_Type>* Info , _Type* Elements)
		{
			FInfo=Info;
			FElements=Elements;
			Inc();
		}

		VL_Array(VL_ArrayDimension<vacNewArray , 1>& Dims)
		{
			Create(Dims.GetSize());
			FInfo->Dims[0]=Dims.Dims[0];
		}

		~VL_Array()
		{
			Dec();
		}

		VL_Array(VL_Array<_Type , 1>& Array)
		{
			FInfo=Array.FInfo;
			FElements=Array.FElements;
			Inc();
		}

		VL_Array<_Type , 1>& operator =(VL_Array<_Type , 1>& Array)
		{
			Dec();
			FInfo=Array.FInfo;
			FElements=Array.FElements;
			Inc();
			return *this;
		}

		_Type& operator [](VInt Dim)
		{
			return FInfo->Elements[Dim];
		}

		VL_Array<_Type , 1> Reference()
		{
			return *this;
		}

		VL_Array<_Type , 1> Clone()
		{
			VL_Array<_Type , 1> Array(FInfo->Size);
			for(VInt i=0;i<FInfo->Size;i++)
			{
				Array.FInfo->Elements[i]=FInfo->Elements[i];
			}
			return Array;
		};

		void Copy(VL_Array<_Type , 1>& Array)
		{
			operator =(Array.Clone());
		}

		VL_ArrayInfo<_Type>* GetInfo()
		{
			return FInfo;
		}

		_Type* GetElements()
		{
			return FElements;
		}

		VInt GetCount()
		{
			return FInfo->Dims[0];
		}
	};
/*
	VL_Tuple的主要作用是可以构造一个类型不同的可比较的向量。
	向量之间的比较规则同VL_Comvec。

	template<
		typename _Type0,
		typename _Type1 = NullTupleElement,
		typename _Type2 = NullTupleElement,
		typename _Type3 = NullTupleElement,
		typename _Type4 = NullTupleElement,
		typename _Type5 = NullTupleElement,
		typename _Type6 = NullTupleElement,
		typename _Type7 = NullTupleElement,
		typename _Type8 = NullTupleElement,
		typename _Type9 = NullTupleElement
	>
	class VL_Tuple
	{
	public:

		template<VInt Index>
		_Type[Index]& Item()
		VBool operator < (_Tuple& Tuple)
		VBool operator <= (_Tuple& Tuple)
		VBool operator > (_Tuple& Tuple)
		VBool operator >= (_Tuple& Tuple)
		VBool operator == (_Tuple& Tuple)
		VBool operator != (_Tuple& Tuple)
	};
*/
	class NullTupleElement
	{
	public:
		VBool operator < (const NullTupleElement& e)const
		{
			return false;
		}
		VBool operator <= (const NullTupleElement& e)const
		{
			return true;
		}
		VBool operator > (const NullTupleElement& e)const
		{
			return false;
		}
		VBool operator >= (const NullTupleElement& e)const
		{
			return true;
		}
		VBool operator == (const NullTupleElement& e)const
		{
			return true;
		}
		VBool operator != (const NullTupleElement& e)const
		{
			return false;
		}
	};

	template<
		typename _Type0,
		typename _Type1 = NullTupleElement,
		typename _Type2 = NullTupleElement,
		typename _Type3 = NullTupleElement,
		typename _Type4 = NullTupleElement,
		typename _Type5 = NullTupleElement,
		typename _Type6 = NullTupleElement,
		typename _Type7 = NullTupleElement,
		typename _Type8 = NullTupleElement,
		typename _Type9 = NullTupleElement
	>
	class VL_Tuple
	{
		typedef VL_Tuple<
			_Type0,
			_Type1,
			_Type2,
			_Type3,
			_Type4,
			_Type5,
			_Type6,
			_Type7,
			_Type8,
			_Type9
		> _Tuple;
	protected:
		/*类型保存*/
		template<
			VInt		Index,
			typename	_Internal
		>
		class Element
		{
		public:
			typedef typename Element<Index-1 , typename _Internal::NextInternal>::Type Type;
		};
		template<
			typename	_Internal
		>
		class Element<0 , _Internal>
		{
		public:
			typedef typename _Internal::PairType::KeyType Type;
		};
		/*类型终结*/
		/*数据保存*/
		template<
			typename _Type0,
			typename _Type1,
			typename _Type2,
			typename _Type3,
			typename _Type4,
			typename _Type5,
			typename _Type6,
			typename _Type7,
			typename _Type8,
			typename _Type9
		>
		class TupleInternal
		{
		public:
			typedef TupleInternal<
				_Type0,
				_Type1,
				_Type2,
				_Type3,
				_Type4,
				_Type5,
				_Type6,
				_Type7,
				_Type8,
				_Type9
			> SelfInternal;
			typedef TupleInternal<
				_Type1,
				_Type2,
				_Type3,
				_Type4,
				_Type5,
				_Type6,
				_Type7,
				_Type8,
				_Type9,
				NullTupleElement
			> NextInternal;
			typedef VL_Fullpair<_Type0 , typename NextInternal::PairType> PairType;

			template<VInt _Index>
			inline static typename Element<_Index , SelfInternal>::Type& Item(typename SelfInternal::PairType& Pair)
			{
				return NextInternal::Item<_Index-1>(Pair.Value);
			}

			template<>
			inline static typename Element<0 , SelfInternal>::Type& Item<0>(typename SelfInternal::PairType& Pair)
			{
				return Pair.Key;
			}
		};
		/*数据终结*/
		template<>
		class TupleInternal<
			NullTupleElement,
			NullTupleElement,
			NullTupleElement,
			NullTupleElement,
			NullTupleElement,
			NullTupleElement,
			NullTupleElement,
			NullTupleElement,
			NullTupleElement,
			NullTupleElement
		>
		{
		public:
			typedef NullTupleElement PairType;
		};

	protected:

		typedef TupleInternal<
			_Type0,
			_Type1,
			_Type2,
			_Type3,
			_Type4,
			_Type5,
			_Type6,
			_Type7,
			_Type8,
			_Type9
		> _Internal;

		typename _Internal::PairType	FData;

	public:

		template<VInt Index>
		typename Element<Index , _Internal>::Type& Item()
		{
			return _Internal::Item<Index>(FData);
		};

		_Tuple& operator = (const _Tuple& Tuple)
		{
			FData=Tuple.FData;
			return *this;
		};

		VBool operator < (const _Tuple& Tuple)const
		{
			return FData<Tuple.FData;
		}

		VBool operator <= (const _Tuple& Tuple)const
		{
			return FData<=Tuple.FData;
		}

		VBool operator > (const _Tuple& Tuple)const
		{
			return FData>Tuple.FData;
		}

		VBool operator >= (const _Tuple& Tuple)const
		{
			return FData>=Tuple.FData;
		}

		VBool operator == (const _Tuple& Tuple)const
		{
			return FData==Tuple.FData;
		}

		VBool operator != (const _Tuple& Tuple)const
		{
			return FData!=Tuple.FData;
		}
	};
}

#endif