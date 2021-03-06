#ifndef VL_DATA_SEARCHTREE
#define VL_DATA_SEARCHTREE

#include "VL_Data_CustomBintree.h"

namespace vl
{
	namespace collection
	{
/*
		template<typename _Type , typename _ExtType=_Type>
		class VL_SearchTree:public VL_CustomBintree<_Type> , public IVL_OrderedContainer<_Type , _ExtType>
		{
		public:
			class VL_osbNode;
			typedef IVL_Iterator<_Type> IVL_SearchTreeIterator;
			typedef VL_cobDataIterator<VL_osbNode> VL_SearchTreeIterator;
		public:
			class VL_osbNode:public VL_cobNode
			{
			public:
				VL_osbNode();
				VL_osbNode(_Type Data);
				VL_osbNode*	GetLeft();
				VL_osbNode*	GetRight();
				VL_osbNode*	GetParent();
				VL_osbNode*	GetChild(VBool Left);
			};
		public:
			VL_osbNode*				GetRoot();								获得根节点，空树返回0
			VL_osbNode*				NodeFrom(_ExtType Data);				获得元素所在的节点，找不到返回0
			VBool					Exists(_ExtType Data);					获得某元素是否存在
			_Type&					GetItem(_ExtType Data);					通过元素值返回元素引用
			void					Add(_Type Data);						增加元素
			void					Delete(VL_osbNode* Node);				删除节点
			void					Delete(_ExtType Data);					删除元素
			void					DeleteItem(_ExtType Data);				删除元素
			void					Clear();								清除元素
			IVL_SearchTreeIterator*	GetPreorderIterator(VBool LeftFirst);	获得前序迭代器
			IVL_SearchTreeIterator*	GetInorderIterator(VBool LeftFirst);	获得中序迭代器
			IVL_SearchTreeIterator*	GetPostorderIterator(VBool LeftFirst);	活的后续迭代器
			IVL_SearchTreeIterator*	GetOrderIterator(VBool Ascending);		获得迭代器
			IVL_SearchTreeIterator*	GetForwardIterator();					获得正序迭代器
			IVL_SearchTreeIterator*	GetBackwardIterator();					获得逆序迭代器
		};
*/
		template<typename _Type , typename _ExtType=_Type>
		class VL_SearchTree:public VL_CustomBintree<_Type>
		{
		public:
			class VL_osbNode;
			typedef IVL_Iterator<_Type> IVL_SearchTreeIterator;
			typedef VL_cobDataIterator<VL_osbNode> VL_SearchTreeIterator;
		public:
			class VL_osbNode:public VL_cobNode
			{
				friend class VL_SearchTree<_Type,_ExtType>;
			public:
				VL_osbNode()
				{
				}
				VL_osbNode(_Type Data)
				{
					FData=Data;
				}
				VL_osbNode* GetLeft()
				{
					return (VL_osbNode*)(FLeft);
				}
				VL_osbNode* GetRight()
				{
					return (VL_osbNode*)(FRight);
				}
				VL_osbNode* GetParent()
				{
					return (VL_osbNode*)(FParent);
				}
				VL_osbNode* GetChild(VBool Left)
				{
					return (VL_osbNode*)(Left?FLeft:FRight);
				}
			protected:
				template<typename _SearchType>
				VL_osbNode* SearchData(_SearchType Data)const
				{
					VL_osbNode* Node=const_cast<VL_osbNode*>(this);
					while(Node)
					{
						if(Node->FData==Data)
						{
							return Node;
						}
						else if(Node->FData>Data)
						{
							Node=Node->GetLeft();
						}
						else
						{
							Node=Node->GetRight();
						}
					}
					return Node;
				}
				virtual void AfterAdd(VL_cobNode*& Root)
				{
				}
				virtual void AfterDelete(VL_cobNode*& Root)
				{
				}
				void Add(VL_osbNode* AddNode,VL_cobNode*& Root)
				{
					VL_osbNode* Node=this;
					VL_osbNode* Prev=this;
					VBool ToLeft=0;
					while(Node)
					{
						Prev=Node;
						if(Node->FData==AddNode->FData)
						{
							ToLeft=1;
							Node=Node->GetLeft();
						}
						else if(Node->FData>AddNode->FData)
						{
							ToLeft=1;
							Node=Node->GetLeft();
						}
						else
						{
							ToLeft=0;
							Node=Node->GetRight();
						}
					}
					if(ToLeft)
					{
						Prev->_SetLeft(AddNode);
						Prev->AfterAdd(Root);
					}
					else
					{
						Prev->_SetRight(AddNode);
						Prev->AfterAdd(Root);
					}
				}
				void Delete(VL_cobNode*& Root)
				{
					VL_osbNode* Node=this;
					VL_cobNode* Parent=0;
					for(;;)
					{
						if(Node->FLeft)
						{
							if(Node->FRight)
							{
								VL_osbNode* Next=(VL_osbNode*)(VL_CustomBintree<_Type>::GetNext(Node,vikInorder));
								Node->Data()=Next->Data();
								Node=Next;
							}
							else
							{
								Parent=Node->FParent;
								if(Node->FParent)
								{
									((VL_osbNode*)Node->FParent)->_Replace(Node,Node->FLeft);
								}
								else
								{
									Root=Node->FLeft;
									((VL_osbNode*)Node)->_Replace(Node->FLeft,0);
								}
								delete Node;
								break;
							}
						}
						else if(Node->FRight)
						{
							Parent=Node->FParent;
							if(Node->FParent)
							{
								((VL_osbNode*)Node->FParent)->_Replace(Node,Node->FRight);
							}
							else
							{
								Root=Node->FRight;
								((VL_osbNode*)Node)->_Replace(Node->FRight,0);
							}
							delete Node;
							break;
						}
						else
						{
							Parent=Node->FParent;
							if(Node->FParent)
							{
								((VL_osbNode*)Node->FParent)->_Replace(Node,0);
							}
							else
							{
								Root=0;
							}
							delete Node;
							break;
						}
					}
					if(Parent)
					{
						((VL_osbNode*)Parent)->AfterDelete(Root);
					}
				}
			};
		protected:
			virtual VL_osbNode* CreateNode(_Type Data)
			{
				return new VL_osbNode(Data);
			}
		public:
			VL_osbNode* GetRoot()const
			{
				if(FRoot)
				{
					return (VL_osbNode*)(FRoot);
				}
				else
				{
					return 0;
				}
			}

			template<typename _SearchType>
			VL_osbNode* NodeFromData(_SearchType Data)
			{
				VL_osbNode* Node=GetRoot();
				if(Node)
				{
					return Node->SearchData<_SearchType>(Data);
				}
				else
				{
					return 0;
				}
			}
			VL_osbNode* NodeFrom(_ExtType Data)const
			{
				VL_osbNode* Node=GetRoot();
				if(Node)
				{
					return Node->SearchData<_ExtType>(Data);
				}
				else
				{
					return 0;
				}
			}
			VBool Exists(_ExtType Data)const
			{
				return NodeFrom(Data)!=0;
			}
			_Type& GetItem(_ExtType Data)
			{
				VL_osbNode* Node=NodeFrom(Data);
				return Node->Data();
			}
			void Add(_Type Data)
			{
				VL_osbNode* Node=GetRoot();
				if(Node)
				{
					Node->Add(CreateNode(Data),FRoot);
				}
				else
				{
					FRoot=CreateNode(Data);
				}
			}
			void Delete(VL_osbNode* Node)
			{
				if(Node)
				{
					Node->Delete(FRoot);
				}
			}
			void Delete(_ExtType Data)
			{
				VL_osbNode* Node=NodeFrom(Data);
				if(Node)
				{
					Delete(Node);
				}
			}
			void DeleteItem(_ExtType Data)
			{
				Delete(Data);
			}
			void Clear()
			{
				VL_CustomBintree<_Type>::Clear();
			}
			
			IVL_SearchTreeIterator* GetPreorderIterator(VBool LeftFirst)
			{
				VL_SearchTreeIterator* Iterator=new VL_SearchTreeIterator(this,vikPreorder,LeftFirst);
				FIteratorManager.Mark(Iterator);
				return Iterator;
			}
			IVL_SearchTreeIterator* GetInorderIterator(VBool LeftFirst)
			{
				VL_SearchTreeIterator* Iterator=new VL_SearchTreeIterator(this,vikInorder,LeftFirst);
				FIteratorManager.Mark(Iterator);
				return Iterator;
			}
			IVL_SearchTreeIterator* GetPostorderIterator(VBool LeftFirst)
			{
				VL_SearchTreeIterator* Iterator=new VL_SearchTreeIterator(this,vikPostorder,LeftFirst);
				FIteratorManager.Mark(Iterator);
				return Iterator;
			}
			IVL_SearchTreeIterator* GetOrderIterator(VBool Ascending)
			{
				return GetInorderIterator(Ascending);
			}
			IVL_SearchTreeIterator* GetForwardIterator()
			{
				return GetInorderIterator(true);
			}
			IVL_SearchTreeIterator* GetBackwardIterator()
			{
				return GetInorderIterator(false);
			}
		};

	}
}

#endif