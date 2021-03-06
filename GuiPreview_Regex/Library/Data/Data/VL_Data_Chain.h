#ifndef VL_Data_Chain
#define VL_Data_Chain

#include "VL_Data_Iterator.h"

namespace vl
{
	namespace collection
	{
/*
		template<typename _Type , typename _ExtType=_Type>
		class VL_Chain:public VL_Base
		{
		public:
			class VL_ocNode;
			typedef IVL_ModifiableIterator<_Type> IVL_ChainIterator;
			typedef IVL_BiIterator<_Type> IVL_ChainBiIterator;
			typedef VL_Chain<_Type,_ExtType> _Chain;
		public:
			class VL_ocNode:public VL_Base
			{
			public:
				_Type&				Data();										获得元素引用
				VL_ocNode*			GetPrev();									获得上一个节点
				VL_ocNode*			GetNext();									获得下一个节点
				VBool				IsLocked();									返回链表内容是否被锁定
				VL_ocNode*			InsertPrev(_Type Data);						在自身前插入元素
				VL_ocNode*			InsertNext(_Type Data);						在自身后插入元素
				VL_ocNode*			Delete(VBool ReturnPrev=1);					删除自身
			};
			typedef VL_ocNode Node;
		public:
			VL_Chain();
			~VL_Chain();
			VInt					GetCount();									获得元素数量
			VL_ocNode*				GetFirstNode();								获得第一个节点，空链表返回0
			VL_ocNode*				GetLastNode();								获得最后一个节点，空链表返回0
			void					InsertAtFirst(_Type Data);					在开头追加元素
			void					InsertAtLast(_Type Data);					在末尾追加元素
			void					Clear();									清除链表
			VL_ocNode*				NodeOf(_ExtType Data);						返回元素所在的节点，找不到返回0
			VBool					Exists(_ExtType Data);						返回元素是否存在
			_Type&					operator [] (VInt Index);					通过位置获得元素引用
			IVL_ChainIterator*		GetForwardIterator();						获得顺序迭代器
			IVL_ChainIterator*		GetBackwardIterator();						获得逆序迭代器
			IVL_ChainBiIterator*	GetBidirectionalIterator(VBool MoveToLast);	获得双向迭代器
		};
*/
		template<typename _Type , typename _ExtType=_Type>
		class VL_Chain:public VL_Base
		{
		public:
			class VL_ocNode;
			typedef IVL_ModifiableIterator<_Type> IVL_ChainIterator;
			typedef IVL_BiIterator<_Type> IVL_ChainBiIterator;
			typedef VL_Chain<_Type,_ExtType> _Chain;
		protected:
			VL_MarkManager	FIteratorManager;
			VInt			FCount;
			VL_ocNode*		FHead;
			VL_ocNode*		FTail;
		public:

			class VL_ocNode:public VL_Base
			{
				friend class VL_Chain;
				friend class VL_ocIterator;
				friend class VL_ocBiIterator;
			protected:
				VL_ocNode*				FPrev;
				VL_ocNode*				FNext;
				_Type					FData;
				_Chain*			FChain;

				void _SetPrev(VL_ocNode* Prev)
				{
					FPrev=Prev;
				}
				void _SetNext(VL_ocNode* Next)
				{
					FNext=Next;
				}
				VL_ocNode* _GetPrev()
				{
					return FPrev;
				}
				VL_ocNode* _GetNext()
				{
					return FNext;
				}
				VL_ocNode* _InsertPrev(_Type Data)
				{
					VL_ocNode* Node=new VL_ocNode(FChain);
					Node->FData=Data;
					Node->FPrev=FPrev;
					Node->FNext=this;
					FPrev->FNext=Node;
					FPrev=Node;
					FChain->FCount++;
					return Node;
				}
				VL_ocNode* _InsertNext(_Type Data)
				{
					VL_ocNode* Node=new VL_ocNode(FChain);
					Node->FData=Data;
					Node->FPrev=this;
					Node->FNext=FNext;
					FNext->FPrev=Node;
					FNext=Node;
					FChain->FCount++;
					return Node;
				}
				VL_ocNode* _Delete(VBool ReturnPrev=1)
				{
					VL_ocNode* Node=ReturnPrev?_GetPrev():_GetNext();
					FPrev->FNext=FNext;
					FNext->FPrev=FPrev;
					FChain->FCount--;
					delete this;
					return Node;
				}
			public:
				VL_ocNode(_Chain* Chain)
				{
					FPrev=0;
					FNext=0;
					FChain=Chain;
				}
				~VL_ocNode()
				{
				}

				_Type& Data()
				{
					return FData;
				}
				VL_ocNode* GetPrev()
				{
					return FPrev==FChain->FHead?0:FPrev;
				}
				VL_ocNode* GetNext()
				{
					return FNext==FChain->FTail?0:FNext;
				}
				VBool IsLocked()
				{
					return !FChain->FIteratorManager.IsEmpty();
				}
				VL_ocNode* InsertPrev(_Type Data)
				{
					if(!IsLocked())
					{
						return _InsertPrev(Data);
					}
					else
					{
						return 0;
					}
				}
				VL_ocNode* InsertNext(_Type Data)
				{
					if(!IsLocked())
					{
						return _InsertNext(Data);
					}
					else
					{
						return 0;
					}
				}
				VL_ocNode* Delete(VBool ReturnPrev=1)
				{
					if(!IsLocked())
					{
						VL_ocNode* Node=ReturnPrev?GetPrev():GetNext();
						FPrev->FNext=FNext;
						FNext->FPrev=FPrev;
						FChain->FCount--;
						delete this;
						return Node;
					}
					else
					{
						return 0;
					}
				}
			};

			typedef VL_ocNode Node;
		protected:
			
			class VL_ocIterator:public virtual VL_MarkManagerItem , public IVL_ChainIterator
			{
			protected:
				_Chain*	FChain;
				VL_ocNode*		FNode;
				VBool			FForward;
			public:
				VL_ocIterator(_Chain* Chain , VBool Forward)
				{
					FChain=Chain;
					FForward=Forward;
					First();
				}
				~VL_ocIterator()
				{
					if(GetMark())
					{
						GetMark()->SetItem(0);
						GetMark()->DestroyItem();
					}
				}

				_Type& Item()
				{
					return FNode->Data();
				}
				void First()
				{
					FNode=FForward?FChain->FHead->_GetNext():FChain->FTail->_GetPrev();
				}
				void Next()
				{
					if(FNode)
					{
						if(FForward)
						{
							if(FNode!=FChain->FTail)
							{
								FNode=FNode->_GetNext();
							}
						}
						else
						{
							if(FNode!=FChain->FHead)
							{
								FNode=FNode->_GetPrev();
							}
						}
					}
				}
				VBool IsFinished()
				{
					return FNode==(FForward?FChain->FTail:FChain->FHead);
				}
				VBool IsAvailable()
				{
					return FNode!=0&&FNode!=FChain->FHead&&FNode!=FChain->FTail;
				}
				void Delete()
				{
					FNode=FNode->_Delete(FForward);
				}
				void InsertPrev(_Type Item)
				{
					if(FForward)
					{
						FNode->_InsertPrev(Item);
					}
					else
					{
						FNode->_InsertNext(Item);
					}
				}
				void InsertNext(_Type Item)
				{
					if(FForward)
					{
						FNode->_InsertNext(Item);
					}
					else
					{
						FNode->_InsertPrev(Item);
					}
				}
			};

			class VL_ocBiIterator:public virtual VL_MarkManagerItem , public IVL_ChainBiIterator
			{
			protected:
				_Chain*	FChain;
				VL_ocNode*		FNode;
				VBool			FUnavailableAtPrev;
			public:
				VL_ocBiIterator(_Chain* Chain)
				{
					FChain=Chain;
					First();
					FUnavailableAtPrev=0;
				}
				~VL_ocBiIterator()
				{
					if(GetMark())
					{
						GetMark()->SetItem(0);
						GetMark()->DestroyItem();
					}
				}

				_Type& Item()
				{
					return FNode->Data();
				}
				void First()
				{
					FNode=FChain->FHead->GetNext();
				}
				void Last()
				{
					FNode=FChain->FTail->GetPrev();
				}
				void Next()
				{
					if(FNode)
					{
						if((FNode=FNode->GetNext())==0)
						{
							FUnavailableAtPrev=0;
						}
					}
				}
				void Prev()
				{
					if(FNode)
					{
						if((FNode=FNode->GetPrev())==0)
						{
							FUnavailableAtPrev=1;
						}
					}
				}
				VBool IsFirst()
				{
					return (FUnavailableAtPrev&&!FNode)||(FNode==FChain->FHead->FNext);
				}
				VBool IsLast()
				{
					return (!FUnavailableAtPrev&&!FNode)||(FNode==FChain->FTail->FPrev);
				}
				VBool IsAvailable()
				{
					return FNode!=0;
				}
			};
		public:
			VL_Chain()
			{
				FHead=new VL_ocNode(this);
				FTail=new VL_ocNode(this);
				FHead->_SetNext(FTail);
				FTail->_SetPrev(FHead);
				FCount=0;
			}
			~VL_Chain()
			{
				VL_ocNode* Node=FHead;
				VL_ocNode* Temp=0;
				do
				{
					Temp=Node->_GetNext();
					delete Node;
				}while(Node=Temp);
			}

			VInt GetCount()
			{
				return FCount;
			}
			VL_ocNode* GetFirstNode()
			{
				return FHead->GetNext();
			}
			VL_ocNode* GetLastNode()
			{
				return FTail->GetPrev();
			}
			void InsertAtFirst(_Type Data)
			{
				FHead->InsertNext(Data);
			}
			void InsertAtLast(_Type Data)
			{
				FTail->InsertPrev(Data);
			}
			void Clear()
			{
				while(FHead->FNext!=FTail)
				{
					FHead->FNext->Delete();
				}
				FCount=0;
			}
			VL_ocNode* NodeOf(_ExtType Data)
			{
				VL_ocNode* Node=FHead->GetNext();
				while(Node)
				{
					if(Node->Data()==Data)
					{
						return Node;
					}
					Node=Node->GetNext();
				}
				return 0;
			}
			VBool Exists(_ExtType Data)
			{
				return NodeOf(Data)!=0;
			}
			_Type& operator [] (VInt Index)
			{
				if(Index>=FCount/2)
				{
					VL_ocNode* Node=FTail->GetPrev();
					Index=FCount-1-Index;
					while(Index--)
					{
						Node=Node->GetPrev();
					}
					return Node->Data();
				}
				else
				{
					VL_ocNode* Node=FHead->GetNext();
					while(Index--)
					{
						Node=Node->GetNext();
					}
					return Node->Data();
				}
			}

			IVL_ChainIterator* GetForwardIterator()
			{
				VL_ocIterator* Iterator=new VL_ocIterator(this,true);
				FIteratorManager.Mark(Iterator);
				return Iterator;
			}
			IVL_ChainIterator* GetBackwardIterator()
			{
				VL_ocIterator* Iterator=new VL_ocIterator(this,false);
				FIteratorManager.Mark(Iterator);
				return Iterator;
			}
			IVL_ChainBiIterator* GetBidirectionalIterator(VBool MoveToLast)
			{
				VL_ocBiIterator* Iterator=new VL_ocBiIterator(this);
				FIteratorManager.Mark(Iterator);
				if(MoveToLast)
				{
					Iterator->Last();
				}
				return Iterator;
			}
		};

	}
}

#endif