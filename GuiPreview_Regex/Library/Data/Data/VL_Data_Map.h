#ifndef VL_DATA_MAP
#define VL_DATA_MAP

#include "VL_Data_List.h"

namespace vl
{
	namespace collection
	{
/*
		template<typename _Key , typename _Value , typename _ExtKey=_Key>
		class VL_ListedMap : public VL_Base
		{
		public:
			VBool Add(_Key Key , _Value Value);			添加数据对，如果原本存在则返回false并用新的value替换，否则返回true
			void Delete(_ExtKey Key);					删除数据对
			VInt IndexOfKey(_ExtKey Key);				获得Key的序号
			VBool Exists(_ExtKey Key)const;				判断一个Key是否存在
			void Clear();								清除所有数据
			_Value& operator [] (_ExtKey Key);			获得Key对应的Value
			VInt KeyCount()const;						获得Key的数目
			_Key KeyOfIndex(VInt Index);				获得序号对应的Key
			_Value& ValueOfIndex(VInt Index);			获得需要对应的Value
		};
*/
		template<typename _Key , typename _Value , typename _ExtKey=_Key>
		class VL_ListedMap : public VL_Base
		{
		public:
			typedef VL_Compair<_Key , _Value>							KeyValuePair;
			typedef VL_OrderedList<KeyValuePair , false , _ExtKey>		KeyValueList;
		protected:
			KeyValueList				FKeyValues;
		public:

			VBool Add(_Key Key , _Value Value)
			{
				VInt Index=IndexOfKey(Key);
				if(Index==-1)
				{
					KeyValuePair Pair;
					Pair.Key=Key;
					Pair.Value=Value;
					FKeyValues.Add(Pair);
					return true;
				}
				else
				{
					FKeyValues[Index].Value=Value;
					return false;
				}
			}

			void Delete(_ExtKey Key)
			{
				VInt Index=IndexOfKey(Key);
				if(Index>-1)
				{
					FKeyValues.Delete(Index);
				}
			}

			void DeleteByIndex(VInt Index)
			{
				FKeyValues.Delete(Index);
			}

			VInt IndexOfKey(_ExtKey Key)
			{
				return FKeyValues.IndexOf(Key);
			}

			VBool Exists(_ExtKey Key)const
			{
				return FKeyValues.Exists(Key);
			}

			void Clear()
			{
				FKeyValues.Clear();
			}

			_Value& operator [] (_ExtKey Key)
			{
				return FKeyValues[IndexOfKey(Key)].Value;
			}

			VInt KeyCount()const
			{
				return FKeyValues.GetCount();
			}

			_Key KeyOfIndex(VInt Index)
			{
				return FKeyValues[Index].Key;
			}

			_Value& ValueOfIndex(VInt Index)
			{
				return FKeyValues[Index].Value;
			}

			void RemoveUnusedSpace()
			{
				FKeyValues.SetCapacity(FKeyValues.GetCount());
			}
		};

/*
		template<typename _Key , typename _Value , VBool _SolidValue , typename _ExtKey=_Key , typename _ExtValue=_Value>
		class VL_MultiMap : public VL_Base
		{
		public:
			typedef VL_List<_Value , _SolidValue , _ExtValue>			ValueList;
			typedef VL_AutoPtr<ValueList>								ValueListPtr;
			typedef VL_Compair<_Key , ValueListPtr>						KeyValuePair;
			typedef VL_OrderedList<KeyValuePair , false , _ExtKey>		KeyValueList;
		public:

			void Add(_Key Key , _Value Value);							添加一个数据对
			void AddDistinct(_Key Key , _Value Value);					添加一个不存在的数据对
			void Insert(_Key Key , VInt Index , _Value Value);			插入一个数据对
			void InsertDistinct(_Key Key , _Value Value);				插入一个不存在的数据对
			void Delete(_ExtKey Key);									把Key对应的数据对全部删掉
			VInt IndexOfKey(_ExtKey Key);								获得Key对应的序号
			VBool Exists(_ExtKey Key)const;								判断一个Key是否存在
			void Clear();												清除所有数据对
			ValueList& operator[] (_ExtKey Key);						获得Key所对应的所有Value
			VInt KeyCount()const;										获得Key的数量
			_Key KeyOfIndex(VInt Index);								获得序号所对应的Key
			ValueList& ValueOfIndex(VInt Index);						获得序号所对应的所有Value
		};
*/

		template<typename _Key , typename _Value , VBool _SolidValue , typename _ExtKey=_Key , typename _ExtValue=_Value>
		class VL_MultiMap : public VL_Base
		{
		public:
			typedef VL_List<_Value , _SolidValue , _ExtValue>			ValueList;
			typedef VL_AutoPtr<ValueList>								ValueListPtr;
			typedef VL_Compair<_Key , ValueListPtr>						KeyValuePair;
			typedef VL_OrderedList<KeyValuePair , false , _ExtKey>		KeyValueList;
		protected:
			KeyValueList				FKeyValues;

			ValueList* CreateList(_Key Key)
			{
				VInt Index=FKeyValues.IndexOf(Key);
				if(Index==-1)
				{
					KeyValuePair Pair;
					Pair.Key=Key;
					Pair.Value=new ValueList;
					FKeyValues.Add(Pair);
					return Pair.Value.Object();
				}
				else
				{
					return FKeyValues[Index].Value.Object();
				}
			}
		public:

			void Add(_Key Key , _Value Value)
			{
				CreateList(Key)->Add(Value);
			}

			void AddDistinct(_Key Key , _Value Value)
			{
				CreateList(Key)->AddDistinct(Value);
			}

			void Insert(_Key Key , VInt Index , _Value Value)
			{
				CreateList(Key)->Insert(Index,Value);
			}

			void InsertDistinct(_Key Key , _Value Value)
			{
				CreateList(Key)->InsertDistinct(Value);
			}

			void Delete(_ExtKey Key)
			{
				VInt Index=FKeyValues.IndexOf(Key);
				if(Index>=0)
				{
					FKeyValues.Delete(Index);
				}
			}

			void DeleteByIndex(VInt Index)
			{
				FKeyValues.Delete(Index);
			}

			VInt IndexOfKey(_ExtKey Key)
			{
				return FKeyValues.IndexOf(Key);
			}

			VBool Exists(_ExtKey Key)const
			{
				return FKeyValues.Exists(Key);
			}

			void Clear()
			{
				FKeyValues.Clear();
			}

			ValueList& operator[] (_ExtKey Key)
			{
				VInt Index=FKeyValues.IndexOf(Key);
				if(Index>=0)
				{
					return ValueOfIndex(Index);
				}
				else
				{
					return *(ValueList*)(0);
				}
			}

			VInt KeyCount()const
			{
				return FKeyValues.GetCount();
			}

			_Key KeyOfIndex(VInt Index)
			{
				return FKeyValues[Index].Key;
			}

			ValueList& ValueOfIndex(VInt Index)
			{
				return *FKeyValues[Index].Value.Object();
			}
		};

	}
}

#endif