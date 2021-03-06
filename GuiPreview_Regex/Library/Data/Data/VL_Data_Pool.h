/*******************************************************************************
Vczh Library++ 2.0
数据结构::对象池
开发者：陈梓瀚

接口：
类：
  VL_FixedPool<_Object>		：固顶尺寸的对象池
  VL_Pool<_Object>			：对象池
函数：
*******************************************************************************/
#ifndef VL_POOL
#define VL_POOL

#include "VL_Data_Basic.h"
#include "VL_Data_List.h"
#include "VL_Data_BalanceTree.h"

namespace vl
{
	namespace pool
	{
		using namespace collection;

/*
		template<typename _Object , typename _Init=VL_PoolInit<_Object>>
		class VL_FixedPool : public VL_Base
		{
			typedef VL_FixedPool<_Object> _Pool;
		public:
			VL_FixedPool(VInt Size);
			~VL_FixedPool();
			VInt GetUsedCount();					获得已经分配的对象的数量
			VInt GetFreeCount();					获得还可以分配的对象的数量
			VInt GetSize();							获得对象的总数量
			VBool IsFull();							空间是否已经用完
			_Object* Alloc();						分配一个对象，失败返回0
			VBool Free(_Object* Object);			回收一个对象，失败返回false

			VBool operator < (_Pool& Pool);			两个固顶对象池的关系
			VBool operator > (_Pool& Pool);
			VBool operator == (_Pool& Pool);
			VBool operator <= (_Pool& Pool);
			VBool operator >= (_Pool& Pool);
			VBool operator != (_Pool& Pool);

			VBool operator < (_Object* Object);		固顶对象池与对象的关系
			VBool operator > (_Object* Object);
			VBool operator == (_Object* Object);
			VBool operator <= (_Object* Object);
			VBool operator >= (_Object* Object);
			VBool operator != (_Object* Object);
		};
*/
		template<typename _Object>
		class VL_PoolInit : public VL_Base
		{
		public:
			void Init(_Object* Object)
			{
			};
		};

		template<typename _Object , typename _Init=VL_PoolInit<_Object>>
		class VL_FixedPool : public VL_Base
		{
			typedef VL_FixedPool<_Object> _Pool;
		protected:
			VInt					FSize;
			_Object*				FObjects;
			VBool*					FObjUsed;
			VInt*					FFrees;
			VInt					FFreeCount;
			_Init*					FInitializer;
		public:
			VL_FixedPool(VInt Size , _Init* Initializer)
			{
				FInitializer=Initializer;
				if(Size<2)
				{
					Size=2;
				}
				FSize=Size;
				FFreeCount=FSize;
				FObjects=new _Object[FSize];
				FObjUsed=new VBool[FSize];
				FFrees=new VInt[FSize];
				for(VInt i=0;i<FSize;i++)
				{
					FObjUsed[i]=false;
					FFrees[i]=i;
				}
			}

			~VL_FixedPool()
			{
				delete[] FFrees;
				delete[] FObjUsed;
				delete[] FObjects;
			}

			VInt GetUsedCount()
			{
				return FSize-FFreeCount;
			}

			VInt GetFreeCount()
			{
				return FFreeCount;
			}

			VInt GetSize()
			{
				return FSize;
			}

			VBool IsFull()
			{
				return FFreeCount==0;
			}

			_Object* Alloc()
			{
				if(FFreeCount)
				{
					VInt Index=FFrees[--FFreeCount];
					FObjUsed[Index]=true;
					FInitializer->Init(&FObjects[Index]);
					return &FObjects[Index];
				}
				else
				{
					return 0;
				}
			}

			VBool Free(_Object* Object)
			{
				VInt Index=(VInt)(Object-FObjects);
				if((Index<0) || (Index>=FSize) || !FObjUsed[Index])
				{
					return false;
				}
				else
				{
					FObjUsed[Index]=false;
					FFrees[FFreeCount++]=Index;
					return true;
				}
			}

			VBool operator < (_Pool& Pool)
			{
				return FObjects<Pool.FObjects;
			}
			VBool operator > (_Pool& Pool)
			{
				return FObjects>Pool.FObjects;
			}
			VBool operator == (_Pool& Pool)
			{
				return FObjects==Pool.FObjects;
			}
			VBool operator <= (_Pool& Pool)
			{
				return FObjects<=Pool.FObjects;
			}
			VBool operator >= (_Pool& Pool)
			{
				return FObjects>=Pool.FObjects;
			}
			VBool operator != (_Pool& Pool)
			{
				return FObjects!=Pool.FObjects;
			}

			VBool operator < (_Object* Object)
			{
				VInt Index=(VInt)(Object-FObjects);
				return Index>=FSize;
			}
			VBool operator > (_Object* Object)
			{
				VInt Index=(VInt)(Object-FObjects);
				return Index<0;
			}
			VBool operator == (_Object* Object)
			{
				VInt Index=(VInt)(Object-FObjects);
				return (Index>=0) && (Index<FSize);
			}
			VBool operator <= (_Object* Object)
			{
				VInt Index=(VInt)(Object-FObjects);
				return Index>=0;
			}
			VBool operator >= (_Object* Object)
			{
				VInt Index=(VInt)(Object-FObjects);
				return Index<FSize;
			}
			VBool operator != (_Object* Object)
			{
				VInt Index=(VInt)(Object-FObjects);
				return (Index<0) || (Index>=FSize);
			}
		};

/*
		template<typename _Object , typename _Init=VL_PoolInit<_Object>>
		class VL_Pool : public VL_Base
		{
		public:
			VL_Pool(VInt PackSize);					构造对象池，PackSize为当需要的时候向系统索取的对象数量
			_Object* Alloc();						分配对象
			VBool Free(_Object* Object);			回收对象
		};
*/
		template<typename _Object , typename _Init=VL_PoolInit<_Object>>
		class VL_Pool : public VL_Base
		{
		protected:
			typedef VL_FixedPool<_Object , _Init>			_PoolObj;
			typedef VL_AutoPtr<_PoolObj>					_PoolPtr;
			typedef VL_List<_PoolPtr , false , _PoolObj*>	_PoolList;
		protected:
			class _PoolWrapper : public VL_Base
			{
			public:
				_PoolObj*	Pool;

				_PoolWrapper(_PoolObj* aPool=0)
				{
					Pool=aPool;
				}

				VBool operator < (_PoolWrapper Wrapper)
				{
					return Pool<Wrapper.Pool;
				}
				VBool operator > (_PoolWrapper Wrapper)
				{
					return Pool>Wrapper.Pool;
				}
				VBool operator == (_PoolWrapper Wrapper)
				{
					return Pool==Wrapper.Pool;
				}
				VBool operator <= (_PoolWrapper Wrapper)
				{
					return Pool<=Wrapper.Pool;
				}
				VBool operator >= (_PoolWrapper Wrapper)
				{
					return Pool>=Wrapper.Pool;
				}
				VBool operator != (_PoolWrapper Wrapper)
				{
					return Pool!=Wrapper.Pool;
				}

				VBool operator < (_Object* Object)
				{
					return (*Pool)<(Object);
				}
				VBool operator > (_Object* Object)
				{
					return (*Pool)>(Object);
				}
				VBool operator == (_Object* Object)
				{
					return (*Pool)==(Object);
				}
				VBool operator <= (_Object* Object)
				{
					return (*Pool)<=(Object);
				}
				VBool operator >= (_Object* Object)
				{
					return (*Pool)>=(Object);
				}
				VBool operator != (_Object* Object)
				{
					return (*Pool)!=(Object);
				}
			};
		protected:
			typedef VL_BalTree<_PoolWrapper>	_PoolTree;
		protected:
			_PoolList		FOwnPools;
			_PoolTree		FPools;
			VInt			FPackSize;
			_Init			FInitializer;
		public:
			VL_Pool(VInt PackSize)
			{
				FPackSize=PackSize;
			}

			_Object* Alloc()
			{
				_PoolObj* FreePool=0;
				for(VInt i=0;i<FOwnPools.GetCount();i++)
				{
					_PoolObj* CurrentPool=FOwnPools[i].Object();
					if(!CurrentPool->IsFull())
					{
						FreePool=CurrentPool;
						break;
					}
				}
				if(!FreePool)
				{
					FreePool=new _PoolObj(FPackSize,&FInitializer);
					FOwnPools.Add(FreePool);
					FPools.Add(FreePool);
				}
				return FreePool->Alloc();
			}

			VBool Free(_Object* Object)
			{
				typename _PoolTree::VL_osbNode* Node=FPools.NodeFromData<_Object*>(Object);
				if(Node)
				{
					return Node->Data().Pool->Free(Object);
				}
				else
				{
					return false;
				}
			}

			_Init* GetInitializer()
			{
				return &FInitializer;
			}
		};

	}
}

#endif