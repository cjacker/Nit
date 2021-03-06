#ifndef VL_DATA_CUSTOMBINTREE
#define VL_DATA_CUSTOMBINTREE

#include "VL_Data_Iterator.h"

namespace vl
{
	namespace collection
	{
/*
		template<typename _Type>
		class VL_CustomBintree abstract:public VL_Base
		{
		public:
			class VL_cobNode;
			typedef VL_CustomBintree<_Type> _CustomBintree;
		public:
			VL_CustomBintree();
			~VL_CustomBintree();
			VBool IteratorExists();			返回迭代器是否被引用
			void Clear();					清除元素
		};

*/
		template<typename _Type>
		class VL_CustomBintree abstract:public VL_Base
		{
		public:
			class VL_cobNode;
			typedef VL_CustomBintree<_Type> _CustomBintree;
		protected:
			VL_MarkManager	FIteratorManager;
			VL_cobNode*		FRoot;

			enum VLE_IteratorKind
			{
				vikPreorder,
				vikInorder,
				vikPostorder
			};
		
			class VL_cobNode:public VL_Base
			{
				friend class _CustomBintree;
			protected:
				VL_cobNode*	FLeft;
				VL_cobNode*	FRight;
				VL_cobNode*	FParent;
				_Type		FData;

				void _Replace(VL_cobNode* OldNode , VL_cobNode* NewNode)
				{
					if(OldNode)
					{
						if(OldNode==FLeft)
						{
							_SetLeft(NewNode);
						}
						else if(OldNode==FRight)
						{
							_SetRight(NewNode);
						}
					}
				}
				virtual VL_cobNode* _SetLeft(VL_cobNode* Node)
				{
					VL_cobNode* Temp=FLeft;
					if(FLeft=Node)
					{
						if(FLeft->FParent)
						{
							FLeft->FParent->_Replace(FLeft,0);
						}
						FLeft->FParent=this;
					}
					if(Temp)
					{
						Temp->FParent=0;
					}
					return Temp;
				}
				virtual VL_cobNode* _SetRight(VL_cobNode* Node)
				{
					VL_cobNode* Temp=FRight;
					if(FRight=Node)
					{
						if(FRight->FParent)
						{
							FRight->FParent->_Replace(FRight,0);
						}
						FRight->FParent=this;
					}
					if(Temp)
					{
						Temp->FParent=0;
					}
					return Temp;
				}
			public:
				VL_cobNode()
				{
					FLeft=0;
					FRight=0;
					FParent=0;
				}
				~VL_cobNode()
				{
					if(FLeft)delete FLeft;
					if(FRight)delete FRight;
				}

				_Type& Data()
				{
					return FData;
				}
				VL_cobNode* GetLeft()
				{
					return FLeft;
				}
				VL_cobNode* GetRight()
				{
					return FRight;
				}
				VL_cobNode* GetParent()
				{
					return FParent;
				}
				VL_cobNode* GetChild(VBool Left)
				{
					return Left?FLeft:FRight;
				}
			};

			static VL_cobNode* GetNext(VL_cobNode* FNode , VLE_IteratorKind Kind , VBool FLeftNode=1 , VBool FRightNode=0)
			{
				switch(Kind)
				{
				case vikPreorder:
					{
						if(FNode->GetChild(FLeftNode))
						{
							FNode=FNode->GetChild(FLeftNode);
						}
						else if(FNode->GetChild(FRightNode))
						{
							FNode=FNode->GetChild(FRightNode);
						}
						else
						{
							for(;;)
							{
								VL_cobNode* Prev=FNode;
								if(FNode=FNode->GetParent())
								{
									if(Prev==FNode->GetChild(FLeftNode))
									{
										if(FNode->GetChild(FRightNode))
										{
											FNode=FNode->GetChild(FRightNode);
											break;
										}
									}
								}
								else
								{
									break;
								}
							}
						}
					}
					break;
				case vikInorder:
					{
						VBool NeedSearchLeft=0;
						if(FNode->GetChild(FRightNode))
						{
							FNode=FNode->GetChild(FRightNode);
							NeedSearchLeft=1;
						}
						else
						{
							for(;;)
							{
								VL_cobNode* Prev=FNode;
								if(FNode=FNode->GetParent())
								{
									if(Prev==FNode->GetChild(FLeftNode))
									{
										break;
									}
								}
								else
								{
									break;
								}
							}
						}
						if(FNode&&NeedSearchLeft)
						{
							while(FNode->GetChild(FLeftNode))
							{
								FNode=FNode->GetChild(FLeftNode);
							}
						}
					}
					break;
				case vikPostorder:
					{
						VBool NeedSearchChild=0;
						for(;;)
						{
							VL_cobNode* Prev=FNode;
							if(FNode=FNode->GetParent())
							{
								if(Prev==FNode->GetChild(FLeftNode))
								{
									if(FNode->GetChild(FRightNode))
									{
										FNode=FNode->GetChild(FRightNode);
										NeedSearchChild=1;
										break;
									}
									else
									{
										break;
									}
								}
								else
								{
									break;
								}
							}
							else
							{
								break;
							}
						}
						if(FNode&&NeedSearchChild)
						{
							for(;;)
							{
								if(FNode->GetChild(FLeftNode))
								{
									FNode=FNode->GetChild(FLeftNode);
								}
								else if(FNode->GetChild(FRightNode))
								{
									FNode=FNode->GetChild(FRightNode);
								}
								else
								{
									break;
								}
							}
						}
					}
					break;
				}
				return FNode;
			}
		protected:

			template<typename _Node>
			class VL_cobIterator:public virtual VL_MarkManagerItem
			{
			protected:
				VLE_IteratorKind	FKind;
				VL_cobNode*			FNode;
				_CustomBintree*		FTree;
				VBool				FLeftNode;
				VBool				FRightNode;
			public:
				VL_cobIterator(_CustomBintree* Tree , VLE_IteratorKind Kind , VBool LeftFirst)
				{
					FTree=Tree;
					FKind=Kind;
					FNode=0;
					FLeftNode=LeftFirst;
					FRightNode=!LeftFirst;
					First();
				}
				~VL_cobIterator()
				{
					if(GetMark())
					{
						GetMark()->SetItem(0);
						GetMark()->DestroyItem();
					}
					if(FNode)delete FNode;
				}

				void First()
				{
					switch(FKind)
					{
					case vikPreorder:
						FNode=FTree->FRoot;
						break;
					case vikInorder:
						FNode=FTree->FRoot;
						if(FNode)
						{
							while(FNode->GetChild(FLeftNode))
							{
								FNode=FNode->GetChild(FLeftNode);
							}
						}
						break;
					case vikPostorder:
						FNode=FTree->FRoot;
						for(;;)
						{
							if(FNode->GetChild(FLeftNode))
							{
								FNode=FNode->GetChild(FLeftNode);
							}
							else if(FNode->GetChild(FRightNode))
							{
								FNode=FNode->GetChild(FRightNode);
							}
							else
							{
								break;
							}
						}
					}
				}
				void Next()
				{
					if(IsAvailable())
					{
						FNode=_CustomBintree::GetNext(FNode,FKind,FLeftNode,FRightNode);
					}
				}
				VBool IsFinished()
				{
					return FNode==0;
				}
				VBool IsAvailable()
				{
					return FNode!=0;
				}
			};

			template<typename _Node>
			class VL_cobNodeIterator:public VL_cobIterator<_Node> , public IVL_Iterator<_Node>
			{
			public:
				VL_cobNodeIterator(_CustomBintree* Tree , VLE_IteratorKind Kind , VBool LeftFirst):VL_cobIterator(Tree,Kind,LeftFirst)
				{
				}
				_Node& Item()
				{
					return *(_Node*)(FNode);
				}
				void First()
				{
					VL_cobIterator<_Node>::First();
				}
				void Next()
				{
					VL_cobIterator<_Node>::Next();
				}
				VBool IsFinished()
				{
					return VL_cobIterator<_Node>::IsFinished();
				}
				VBool IsAvailable()
				{
					return VL_cobIterator<_Node>::IsAvailable();
				}
			};

			template<typename _Node>
			class VL_cobDataIterator:public VL_cobIterator<_Node> , public IVL_Iterator<_Type>
			{
			public:
				VL_cobDataIterator(_CustomBintree* Tree , VLE_IteratorKind Kind , VBool LeftFirst):VL_cobIterator(Tree,Kind,LeftFirst)
				{
				}
				_Type& Item()
				{
					return FNode->Data();
				}
				void First()
				{
					VL_cobIterator<_Node>::First();
				}
				void Next()
				{
					VL_cobIterator<_Node>::Next();
				}
				VBool IsFinished()
				{
					return VL_cobIterator<_Node>::IsFinished();
				}
				VBool IsAvailable()
				{
					return VL_cobIterator<_Node>::IsAvailable();
				}
			};
		protected:
			VL_cobNode* _SetRoot(VL_cobNode* Root)
			{
				VL_cobNode* Temp=FRoot;
				if(Temp&&Temp->FParent)
				{
					Temp->FParent->_Replace(Temp,0);
					Temp->FParent=0;
				}
				if(FRoot=Root)
				{
					FRoot->FParent=0;
				}
				return Temp;
			}
		public:
			VL_CustomBintree()
			{
				FRoot=0;
			}
			~VL_CustomBintree()
			{
				delete FRoot;
			}
			VBool IteratorExists()
			{
				return !FIteratorManager.IsEmpty();
			}
			void Clear()
			{
				if(FRoot)
				{
					delete FRoot;
					FRoot=0;
				}
			}
		};

	}
}

#endif