#include "VL_WinView.h"

namespace vl
{
	namespace windows
	{
		const VInt LISTVIEW_TEXT_BUFFER_SIZE=1024;

		VL_IMPLEMENT_VOID_EVENT	(VE_ListViewItemEvent	,(Sender , ItemIndex , SubItemIndex)		,(VL_Base* Sender , VInt ItemIndex , VInt SubItemIndex));
		VL_IMPLEMENT_VOID_EVENT	(VE_ListViewEditEvent	,(Sender , ItemIndex , Accept , Text)		,(VL_Base* Sender , VInt ItemIndex , VBool& Accept , VUnicodeString Text));
		VL_IMPLEMENT_VOID_EVENT	(VE_ListViewSortEvent	,(Sender , Index1 , Index2 , Param , Result),(VL_Base* Sender , VInt Index1 , VInt Index2 , VPointer Param , VLE_ListViewSortResult& Result));

		VL_IMPLEMENT_VOID_EVENT	(VE_TreeViewItemEvent	,(Sender , Item)							,(VL_Base* Sender , VL_WinTreeViewItem Item));
		VL_IMPLEMENT_VOID_EVENT	(VE_TreeViewVerifyEvent	,(Sender , Item , Accept)					,(VL_Base* Sender , VL_WinTreeViewItem Item , VBool& Accept));
		VL_IMPLEMENT_VOID_EVENT	(VE_TreeViewEditEvent	,(Sender , Item , Accept , Text)			,(VL_Base* Sender , VL_WinTreeViewItem Item , VBool& Accept , VUnicodeString Text));

/*********************************************************************************************************
VL_WinListViewItem
*********************************************************************************************************/

	void FillListViewItem(LVITEM* Item , VInt Index , VInt SubItem , UINT Mask)
	{
		memset(Item,0,sizeof(*Item));
		Item->iItem=Index;
		Item->iSubItem=SubItem;
		Item->mask=Mask;
	}

	VL_WinListViewItem::VL_WinListViewItem(HWND Handle , VInt Index)
	{
		FHandle=Handle;
		FIndex=Index;
	}

	VL_WinListViewItem::VL_WinListViewItem()
	{
		FHandle=0;
		FIndex=0;
	}

	VL_WinListViewItem::VL_WinListViewItem(const VL_WinListViewItem& Item)
	{
		FHandle=Item.FHandle;
		FIndex=Item.FIndex;
	}

	VL_WinListViewItem& VL_WinListViewItem::operator=(const VL_WinListViewItem& Item)
	{
		FHandle=Item.FHandle;
		FIndex=Item.FIndex;
		return *this;
	}

	VL_WinListViewItem::operator VBool()
	{
		return FHandle!=0;
	}

	VInt VL_WinListViewItem::GetIndex()
	{
		return FIndex;
	}

	VUnicodeString VL_WinListViewItem::GetText()
	{
		return GetSubItem(-1);
	}

	void VL_WinListViewItem::SetText(VUnicodeString Value)
	{
		SetSubItem(-1,Value);
	}

	VInt VL_WinListViewItem::GetImageIndex()
	{
		LVITEM Item;
		FillListViewItem(&Item,FIndex,0,LVIF_IMAGE);
		ListView_GetItem(FHandle,&Item);
		return Item.iImage;
	}

	void VL_WinListViewItem::SetImageIndex(VInt Value)
	{
		LVITEM Item;
		FillListViewItem(&Item,FIndex,0,LVIF_IMAGE);
		Item.iImage=Value;
		ListView_SetItem(FHandle,&Item);
	}

	VUnicodeString VL_WinListViewItem::GetSubItem(VInt Index)
	{
		VWChar Buffer[LISTVIEW_TEXT_BUFFER_SIZE]={0};
		LVITEM Item;
		FillListViewItem(&Item,FIndex,Index+1,LVIF_TEXT);
		Item.pszText=Buffer;
		Item.cchTextMax=sizeof(Buffer)/sizeof(*Buffer);
		ListView_GetItem(FHandle,&Item);
		return Buffer;
	}

	void VL_WinListViewItem::SetSubItem(VInt Index , VUnicodeString Value)
	{
		LVITEM Item;
		FillListViewItem(&Item,FIndex,Index+1,LVIF_TEXT);
		Item.pszText=Value.Buffer();
		BOOL Result=ListView_SetItem(FHandle,&Item);
	}

	void VL_WinListViewItem::GetTiledSubItem(VL_List<VInt , true>& Columns)
	{
		UINT Buffer[20]={0};
		LVITEM Item;
		FillListViewItem(&Item,FIndex,0,LVIF_COLUMNS);
		Item.puColumns=Buffer;
		ListView_GetItem(FHandle,&Item);
		Columns.Clear();
		for(UINT i=0;i<Item.cColumns;i++)
		{
			Columns.Add(Buffer[i]-1);
		}
	}

	void VL_WinListViewItem::SetTiledSubItem(VL_List<VInt , true>& Columns)
	{
		UINT Buffer[20]={0};
		VInt Count=0;
		for(;Count<Columns.GetCount() && Count<20;Count++)
		{
			Buffer[Count]=Columns[Count]+1;
		}
		LVITEM Item;
		FillListViewItem(&Item,FIndex,0,LVIF_COLUMNS);
		Item.cColumns=Count;
		Item.puColumns=Buffer;
		ListView_SetItem(FHandle,&Item);
	}

	VBool VL_WinListViewItem::GetCutState()
	{
		return ListView_GetItemState(FHandle,FIndex,LVIS_CUT)!=0;
	}

	void VL_WinListViewItem::SetCutState(VBool Value)
	{
		ListView_SetItemState(FHandle,FIndex,(Value?LVIS_CUT:0),LVIS_CUT);
	}

	VBool VL_WinListViewItem::GetDropHilitedState()
	{
		return ListView_GetItemState(FHandle,FIndex,LVIS_DROPHILITED)!=0;
	}

	void VL_WinListViewItem::SetDropHilitedState(VBool Value)
	{
		ListView_SetItemState(FHandle,FIndex,(Value?LVIS_DROPHILITED:0),LVIS_DROPHILITED);
	}

	VBool VL_WinListViewItem::GetFocusedState()
	{
		return ListView_GetItemState(FHandle,FIndex,LVIS_FOCUSED)!=0;
	}

	void VL_WinListViewItem::SetFocusedState(VBool Value)
	{
		ListView_SetItemState(FHandle,FIndex,(Value?LVIS_FOCUSED:0),LVIS_FOCUSED);
	}

	VBool VL_WinListViewItem::GetSelectedState()
	{
		return ListView_GetItemState(FHandle,FIndex,LVIS_SELECTED)!=0;
	}

	void VL_WinListViewItem::SetSelectedState(VBool Value)
	{
		ListView_SetItemState(FHandle,FIndex,(Value?LVIS_SELECTED:0),LVIS_SELECTED);
	}

	VInt VL_WinListViewItem::GetIndent()
	{
		LVITEM Item;
		FillListViewItem(&Item,FIndex,0,LVIF_INDENT);
		ListView_GetItem(FHandle,&Item);
		return Item.iIndent;
	}

	void VL_WinListViewItem::SetIndent(VInt Value)
	{
		LVITEM Item;
		FillListViewItem(&Item,FIndex,0,LVIF_INDENT);
		Item.iIndent=Value;
		ListView_SetItem(FHandle,&Item);
	}

	VBool VL_WinListViewItem::GetChecked()
	{
		return ListView_GetCheckState(FHandle,FIndex)!=0;
	}

	void VL_WinListViewItem::SetChecked(VBool Value)
	{
		ListView_SetCheckState(FHandle,FIndex,(Value?TRUE:FALSE));
	}

	VPointer VL_WinListViewItem::GetCustomData()
	{
		LVITEM Item;
		FillListViewItem(&Item,FIndex,0,LVIF_PARAM);
		ListView_GetItem(FHandle,&Item);
		return (VPointer)Item.lParam;
	}

	void VL_WinListViewItem::SetCustomData(VPointer Data)
	{
		LVITEM Item;
		FillListViewItem(&Item,FIndex,0,LVIF_PARAM);
		Item.lParam=(LPARAM)Data;
		ListView_SetItem(FHandle,&Item);
	}

	void VL_WinListViewItem::EnterGroup(VInt GroupID)
	{
		LVITEM Item;
		FillListViewItem(&Item,FIndex,0,LVIF_GROUPID);
		Item.iGroupId=GroupID;
		ListView_SetItem(FHandle,&Item);
	}
	
	void VL_WinListViewItem::LeaveGroup()
	{
		EnterGroup(I_GROUPIDNONE);
	}

	VInt VL_WinListViewItem::GetOwnerGroupID()
	{
		LVITEM Item;
		FillListViewItem(&Item,FIndex,0,LVIF_GROUPID);
		ListView_GetItem(FHandle,&Item);
		return Item.iGroupId;
	}

	VBool VL_WinListViewItem::IsInGroup()
	{
		return GetOwnerGroupID()!=I_GROUPIDNONE;
	}

/*********************************************************************************************************
VL_WinListViewColumn
*********************************************************************************************************/

	void FillListViewColumn(LVCOLUMN* Column , UINT Mask)
	{
		memset(Column,0,sizeof(*Column));
		Column->mask=Mask;
	}

	VL_WinListViewColumn::VL_WinListViewColumn(HWND Handle , VInt Index)
	{
		FHandle=Handle;
		FIndex=Index;
	}

	VL_WinListViewColumn::VL_WinListViewColumn()
	{
		FHandle=0;
		FIndex=0;
	}

	VL_WinListViewColumn::VL_WinListViewColumn(const VL_WinListViewColumn& Item)
	{
		FHandle=Item.FHandle;
		FIndex=Item.FIndex;
	}

	VL_WinListViewColumn& VL_WinListViewColumn::operator=(const VL_WinListViewColumn& Item)
	{
		FHandle=Item.FHandle;
		FIndex=Item.FIndex;
		return *this;
	}

	VL_WinListViewColumn::operator VBool()
	{
		return FHandle!=0;
	}

	VInt VL_WinListViewColumn::GetIndex()
	{
		return FIndex;
	}
	
	VUnicodeString VL_WinListViewColumn::GetText()
	{
		VWChar Buffer[LISTVIEW_TEXT_BUFFER_SIZE]={0};
		LVCOLUMN Column;
		FillListViewColumn(&Column,LVCF_TEXT);
		Column.pszText=Buffer;
		Column.cchTextMax=sizeof(Buffer)/sizeof(*Buffer);
		ListView_GetColumn(FHandle,FIndex,&Column);
		return Buffer;
	}

	void VL_WinListViewColumn::SetText(VUnicodeString Value)
	{
		LVCOLUMN Column;
		FillListViewColumn(&Column,LVCF_TEXT);
		Column.pszText=Value.Buffer();
		ListView_SetColumn(FHandle,FIndex,&Column);
	}

	VInt VL_WinListViewColumn::GetSubItemIndex()
	{
		LVCOLUMN Column;
		FillListViewColumn(&Column,LVCF_SUBITEM);
		ListView_GetColumn(FHandle,FIndex,&Column);
		return Column.iSubItem-1;
	}

	void VL_WinListViewColumn::SetSubItemIndex(VInt Value)
	{
		LVCOLUMN Column;
		FillListViewColumn(&Column,LVCF_SUBITEM);
		Column.iSubItem=Value+1;
		ListView_SetColumn(FHandle,FIndex,&Column);
	}

	VInt VL_WinListViewColumn::GetWidth()
	{
		LVCOLUMN Column;
		FillListViewColumn(&Column,LVCF_WIDTH);
		ListView_GetColumn(FHandle,FIndex,&Column);
		return Column.cx;
	}

	void VL_WinListViewColumn::SetWidth(VInt Value)
	{
		LVCOLUMN Column;
		FillListViewColumn(&Column,LVCF_WIDTH | LVCF_DEFAULTWIDTH);
		Column.cx=Value;
		Column.cxDefault=Value;
		ListView_SetColumn(FHandle,FIndex,&Column);
	}

	VInt VL_WinListViewColumn::GetOrder()
	{
		LVCOLUMN Column;
		FillListViewColumn(&Column,LVCF_ORDER);
		ListView_GetColumn(FHandle,FIndex,&Column);
		return Column.iOrder;
	}

	void VL_WinListViewColumn::SetOrder(VInt Value)
	{
		LVCOLUMN Column;
		FillListViewColumn(&Column,LVCF_ORDER);
		Column.iOrder=Value;
		ListView_SetColumn(FHandle,FIndex,&Column);
	}

	VInt VL_WinListViewColumn::GetImageIndex()
	{
		LVCOLUMN Column;
		FillListViewColumn(&Column,LVCF_IMAGE);
		ListView_GetColumn(FHandle,FIndex,&Column);
		return Column.iImage;
	}

	void VL_WinListViewColumn::SetImageIndex(VInt Value)
	{
		LVCOLUMN Column;
		FillListViewColumn(&Column,LVCF_IMAGE | LVCF_FMT);
		ListView_GetColumn(FHandle,FIndex,&Column);
		if(Value>=0)
		{
			Column.fmt|=LVCFMT_COL_HAS_IMAGES;
		}
		else
		{
			Column.fmt&=~LVCFMT_COL_HAS_IMAGES;
		}
		Column.iImage=Value;
		ListView_SetColumn(FHandle,FIndex,&Column);
	}

	VLE_WinHAlign VL_WinListViewColumn::GetTextAlign()
	{
		LVCOLUMN Column;
		FillListViewColumn(&Column,LVCF_FMT);
		ListView_GetColumn(FHandle,FIndex,&Column);
		if(Column.fmt & LVCFMT_RIGHT)
		{
			return vwhaRight;
		}
		else if(Column.fmt & LVCFMT_CENTER)
		{
			return vwhaCenter;
		}
		else
		{
			return vwhaLeft;
		}
	}

	void VL_WinListViewColumn::SetTextAlign(VLE_WinHAlign Value)
	{
		LVCOLUMN Column;
		FillListViewColumn(&Column,LVCF_FMT);
		ListView_GetColumn(FHandle,FIndex,&Column);
		Column.fmt&=~LVCFMT_RIGHT;
		Column.fmt&=~LVCFMT_CENTER;
		Column.fmt&=~LVCFMT_LEFT;
		switch(Value)
		{
		case vwhaRight:
			Column.fmt|=LVCFMT_RIGHT;
			break;
		case vwhaCenter:
			Column.fmt|=LVCFMT_CENTER;
			break;
		case vwhaLeft:
			Column.fmt|=LVCFMT_LEFT;
			break;
		}
		ListView_SetColumn(FHandle,FIndex,&Column);
	}

	VBool VL_WinListViewColumn::GetImageOnRight()
	{
		LVCOLUMN Column;
		FillListViewColumn(&Column,LVCF_FMT);
		ListView_GetColumn(FHandle,FIndex,&Column);
		return (Column.fmt & LVCFMT_BITMAP_ON_RIGHT)!=0;
	}

	void VL_WinListViewColumn::SetImageOnRight(VBool Value)
	{
		LVCOLUMN Column;
		FillListViewColumn(&Column,LVCF_FMT);
		ListView_GetColumn(FHandle,FIndex,&Column);
		if(Value)
		{
			Column.fmt|=LVCFMT_BITMAP_ON_RIGHT;
		}
		else
		{
			Column.fmt&=~LVCFMT_BITMAP_ON_RIGHT;
		}
		ListView_SetColumn(FHandle,FIndex,&Column);
	}

	VBool VL_WinListViewColumn::GetFixedWidth()
	{
		LVCOLUMN Column;
		FillListViewColumn(&Column,LVCF_FMT);
		ListView_GetColumn(FHandle,FIndex,&Column);
		return (Column.fmt & LVCFMT_FIXED_WIDTH)!=0;
	}

	void VL_WinListViewColumn::SetFixedWidth(VBool Value)
	{
		LVCOLUMN Column;
		FillListViewColumn(&Column,LVCF_FMT);
		ListView_GetColumn(FHandle,FIndex,&Column);
		if(Value)
		{
			Column.fmt|=LVCFMT_FIXED_WIDTH;
		}
		else
		{
			Column.fmt&=~LVCFMT_FIXED_WIDTH;
		}
		ListView_SetColumn(FHandle,FIndex,&Column);
	}

	VBool VL_WinListViewColumn::GetShowSplitButton()
	{
		LVCOLUMN Column;
		FillListViewColumn(&Column,LVCF_FMT);
		ListView_GetColumn(FHandle,FIndex,&Column);
		return (Column.fmt & LVCFMT_SPLITBUTTON)!=0;
	}

	void VL_WinListViewColumn::SetShowSplitButton(VBool Value)
	{
		LVCOLUMN Column;
		FillListViewColumn(&Column,LVCF_FMT);
		ListView_GetColumn(FHandle,FIndex,&Column);
		if(Value)
		{
			Column.fmt|=LVCFMT_SPLITBUTTON;
		}
		else
		{
			Column.fmt&=~LVCFMT_SPLITBUTTON;
		}
		ListView_SetColumn(FHandle,FIndex,&Column);
	}

/*********************************************************************************************************
VL_WinListViewGroup
*********************************************************************************************************/

	void FillListViewGroup(LVGROUP* Group , UINT Mask)
	{
		memset(Group,0,sizeof(*Group));
		Group->cbSize=sizeof(*Group);
		Group->mask=Mask;
	}

	VUnicodeString VL_WinListViewGroup::GetListViewGroupText(UINT Mask , LPWSTR LVGROUP::* Item , UINT LVGROUP::* Count)
	{
		VWChar Buffer[LISTVIEW_TEXT_BUFFER_SIZE];
		LVGROUP Group;
		FillListViewGroup(&Group,Mask);
		Group.*Item=Buffer;
		Group.*Count=LISTVIEW_TEXT_BUFFER_SIZE;
		ListView_GetGroupInfo(FHandle,FID,&Group);
		return Buffer;
	}

	VUnicodeString VL_WinListViewGroup::GetListViewGroupText(UINT Mask , LPWSTR LVGROUP::* Item , int LVGROUP::* Count)
	{
		VWChar Buffer[LISTVIEW_TEXT_BUFFER_SIZE];
		LVGROUP Group;
		FillListViewGroup(&Group,Mask);
		Group.*Item=Buffer;
		Group.*Count=LISTVIEW_TEXT_BUFFER_SIZE;
		ListView_GetGroupInfo(FHandle,FID,&Group);
		return Buffer;
	}

	void VL_WinListViewGroup::SetListViewGroupText(UINT Mask , LPWSTR LVGROUP::* Item , VUnicodeString Text)
	{
		LVGROUP Group;
		FillListViewGroup(&Group,Mask);
		Group.*Item=Text.Length()?Text.Buffer():NULL;
		ListView_SetGroupInfo(FHandle,FID,&Group);
	}

	VL_WinListViewGroup::VL_WinListViewGroup(HWND Handle , VInt ID)
	{
		FHandle=Handle;
		FID=ID;
	}

	VL_WinListViewGroup::VL_WinListViewGroup()
	{
		FHandle=0;
		FID=0;
	}

	VL_WinListViewGroup::VL_WinListViewGroup(const VL_WinListViewGroup& Item)
	{
		FHandle=Item.FHandle;
		FID=Item.FID;
	}

	VL_WinListViewGroup& VL_WinListViewGroup::operator=(const VL_WinListViewGroup& Item)
	{
		FHandle=Item.FHandle;
		FID=Item.FID;
		return *this;
	}

	VL_WinListViewGroup::operator VBool()
	{
		return FHandle!=0;
	}

	VInt VL_WinListViewGroup::GetGroupID()
	{
		return FID;
	}

	VUnicodeString VL_WinListViewGroup::GetHeader()
	{
		return GetListViewGroupText(LVGF_HEADER,&LVGROUP::pszHeader,&LVGROUP::cchHeader);
	}

	void VL_WinListViewGroup::SetHeader(VUnicodeString Value)
	{
		SetListViewGroupText(LVGF_HEADER,&LVGROUP::pszHeader,Value);
	}

	VUnicodeString VL_WinListViewGroup::GetFooter()
	{
		return GetListViewGroupText(LVGF_FOOTER,&LVGROUP::pszFooter,&LVGROUP::cchFooter);
	}

	void VL_WinListViewGroup::SetFooter(VUnicodeString Value)
	{
		SetListViewGroupText(LVGF_FOOTER,&LVGROUP::pszFooter,Value);
	}

	VUnicodeString VL_WinListViewGroup::GetSubTitle()
	{
		return GetListViewGroupText(LVGF_SUBTITLE,&LVGROUP::pszSubtitle,&LVGROUP::cchSubtitle);
	}

	void VL_WinListViewGroup::SetSubTitle(VUnicodeString Value)
	{
		SetListViewGroupText(LVGF_SUBTITLE,&LVGROUP::pszSubtitle,Value);
	}

	VUnicodeString VL_WinListViewGroup::GetTask()
	{
		return GetListViewGroupText(LVGF_TASK,&LVGROUP::pszTask,&LVGROUP::cchTask);
	}

	void VL_WinListViewGroup::SetTask(VUnicodeString Value)
	{
		SetListViewGroupText(LVGF_TASK,&LVGROUP::pszTask,Value);
	}

	VUnicodeString VL_WinListViewGroup::GetDescriptionTop()
	{
		return GetListViewGroupText(LVGF_DESCRIPTIONTOP,&LVGROUP::pszDescriptionTop,&LVGROUP::cchDescriptionTop);
	}

	void VL_WinListViewGroup::SetDescriptionTop(VUnicodeString Value)
	{
		SetListViewGroupText(LVGF_DESCRIPTIONTOP,&LVGROUP::pszDescriptionTop,Value);
	}

	VUnicodeString VL_WinListViewGroup::GetDescriptionBottom()
	{
		return GetListViewGroupText(LVGF_DESCRIPTIONBOTTOM,&LVGROUP::pszDescriptionBottom,&LVGROUP::cchDescriptionBottom);
	}

	void VL_WinListViewGroup::SetDescriptionBottom(VUnicodeString Value)
	{
		SetListViewGroupText(LVGF_DESCRIPTIONBOTTOM,&LVGROUP::pszDescriptionBottom,Value);
	}

	VInt VL_WinListViewGroup::GetImageIndex()
	{
		LVGROUP Group;
		FillListViewGroup(&Group,LVGF_TITLEIMAGE);
		ListView_GetGroupInfo(FHandle,FID,&Group);
		return Group.iTitleImage;
	}

	void VL_WinListViewGroup::SetImageIndex(VInt Value)
	{
		LVGROUP Group;
		FillListViewGroup(&Group,LVGF_TITLEIMAGE);
		Group.iTitleImage=Value;
		ListView_SetGroupInfo(FHandle,FID,&Group);
	}

	VInt VL_WinListViewGroup::GetExtendedImageIndex()
	{
		LVGROUP Group;
		FillListViewGroup(&Group,LVGF_EXTENDEDIMAGE);
		ListView_GetGroupInfo(FHandle,FID,&Group);
		return Group.iExtendedImage;
	}

	void VL_WinListViewGroup::SetExtendedImageIndex(VInt Value)
	{
		LVGROUP Group;
		FillListViewGroup(&Group,LVGF_EXTENDEDIMAGE);
		Group.iExtendedImage=Value;
		ListView_SetGroupInfo(FHandle,FID,&Group);
	}

	VLE_WinHAlign VL_WinListViewGroup::GetHeaderAlign()
	{
		LVGROUP Group;
		FillListViewGroup(&Group,LVGF_ALIGN);
		ListView_GetGroupInfo(FHandle,FID,&Group);
		if(Group.uAlign==LVGA_HEADER_LEFT)
		{
			return vwhaLeft;
		}
		else if(Group.uAlign==LVGA_HEADER_CENTER)
		{
			return vwhaCenter;
		}
		else
		{
			return vwhaRight;
		}
	}

	void VL_WinListViewGroup::SetHeaderAlign(VLE_WinHAlign Value)
	{
		LVGROUP Group;
		FillListViewGroup(&Group,LVGF_ALIGN);
		switch(Value)
		{
		case vwhaLeft:
			Group.uAlign=LVGA_HEADER_LEFT;
			break;
		case vwhaCenter:
			Group.uAlign=LVGA_HEADER_CENTER;
			break;
		case vwhaRight:
			Group.uAlign=LVGA_HEADER_RIGHT;
			break;
		}
		ListView_SetGroupInfo(FHandle,FID,&Group);
	}

/*********************************************************************************************************
VL_WinListView
*********************************************************************************************************/

	class ListViewSortParam
	{
	public:
		VPointer Param;
		VL_WinListView* ListView;
	};

	int CALLBACK ListViewCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		VLE_ListViewSortResult Result=vlvsrEqual;
		ListViewSortParam* P=(ListViewSortParam*)lParamSort;
		P->ListView->OnCompareItems(P->ListView,lParam1,lParam2,P->Param,Result);
		switch(Result)
		{
		case vlvsrLarger:
			return 1;
		case vlvsrSmaller:
			return -1;
		default:
			return 0;
		}
	}

	DWORD VL_WinListView::InternalGetExStyle()
	{
		return ListView_GetExtendedListViewStyle(FHandle);
	}

	void VL_WinListView::InternalSetExStyle(DWORD ExStyle)
	{
		ListView_SetExtendedListViewStyle(FHandle,ExStyle);
	}

	VL_WinListView::VL_WinListView(VL_WinContainer* Parent)
	{
		FSmallImageList=0;
		FLargeImageList=0;
		FStateImageList=0;
		FGroupImageList=0;
		FColumnCount=0;
		FUsedGroupIDs=1;

		_CreateWindow(
			0,
			WS_VSCROLL | WS_HSCROLL | WS_BORDER | WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | LVS_ALIGNTOP | LVS_SHAREIMAGELISTS | LVS_ICON,
			WC_LISTVIEW,
			Parent);

		ListView_SetUnicodeFormat(FHandle,TRUE);
		SetExStyle(LVS_EX_DOUBLEBUFFER,true);
		SetExStyle(LVS_EX_FULLROWSELECT,true);
		SetExStyle(LVS_EX_TRACKSELECT,false);
	}

	VL_WinListView::~VL_WinListView()
	{
	}

	LRESULT VL_WinListView::ProcessMessage(UINT Message , WPARAM& wParam , LPARAM& lParam , VBool& CallDefaultProcedure)
	{
		LRESULT Result=0;
		switch(Message)
		{
		case WM_NOTIFY_DISPATCHED:
			switch(((NMHDR*)lParam)->code)
			{
			case LVN_BEGINDRAG:
				{
					NMLISTVIEW* Param=(NMLISTVIEW*)lParam;
					OnBeginDrag(this,Param->iItem,-1);
				}
				break;
			case LVN_BEGINRDRAG:
				{
					NMLISTVIEW* Param=(NMLISTVIEW*)lParam;
					OnBeginRightDrag(this,Param->iItem,-1);
				}
				break;
			case LVN_BEGINLABELEDIT:
				{
					CallDefaultProcedure=false;
					NMLVDISPINFO* Param=(NMLVDISPINFO*)lParam;
					VUnicodeString Text=Param->item.pszText?Param->item.pszText:L"";
					VBool Accept=true;
					OnBeginLabelEdit(this,Param->item.iItem,Accept,Text);
					return Accept?FALSE:TRUE;
				}
				break;
			case LVN_ENDLABELEDIT:
				{
					CallDefaultProcedure=false;
					NMLVDISPINFO* Param=(NMLVDISPINFO*)lParam;
					VUnicodeString Text=Param->item.pszText?Param->item.pszText:L"";
					VBool Accept=true;
					OnEndLabelEdit(this,Param->item.iItem,Accept,Text);
					return Accept?TRUE:FALSE;
				}
			case LVN_COLUMNCLICK:
				{
					NMLISTVIEW* Param=(NMLISTVIEW*)lParam;
					OnColumnClick(this,Param->iSubItem,-1);
				}
				break;
			case LVN_COLUMNDROPDOWN:
				{
					NMLISTVIEW* Param=(NMLISTVIEW*)lParam;
					OnColumnDropDown(this,Param->iSubItem,-1);
				}
				break;
			case LVN_HOTTRACK:
				{
					NMLISTVIEW* Param=(NMLISTVIEW*)lParam;
					OnItemHotTrack(this,Param->iItem,Param->iSubItem-1);
				}
				break;
			case LVN_ITEMACTIVATE:
				{
					NMITEMACTIVATE* Param=(NMITEMACTIVATE*)lParam;
					OnItemActive(this,Param->iItem,-1);
				}
				break;
			case LVN_ITEMCHANGING:
				{
					NMLISTVIEW* Param=(NMLISTVIEW*)lParam;
					OnItemChanging(this,Param->iItem,-1);
				}
				break;
			case LVN_ITEMCHANGED:
				{
					NMLISTVIEW* Param=(NMLISTVIEW*)lParam;
					OnItemChanged(this,Param->iItem,-1);
				}
				break;
			case LVN_LINKCLICK:
				{
					NMLVLINK* Param=(NMLVLINK*)lParam;
					if(ListView_HasGroup(FHandle,Param->iSubItem))
					{
						OnGroupTaskClick(this,Param->iSubItem,-1);
					}
				}
				break;
			case NM_CLICK:
				{
					NMITEMACTIVATE* Param=(NMITEMACTIVATE*)lParam;
					OnItemClick(this,Param->iItem,Param->iSubItem-1);
				}
				break;
			case NM_DBLCLK:
				{
					NMITEMACTIVATE* Param=(NMITEMACTIVATE*)lParam;
					OnItemDbClick(this,Param->iItem,Param->iSubItem-1);
				}
				break;
			case NM_RCLICK:
				{
					NMITEMACTIVATE* Param=(NMITEMACTIVATE*)lParam;
					OnItemRightClick(this,Param->iItem,Param->iSubItem-1);
				}
				break;
			case NM_RDBLCLK:
				{
					NMITEMACTIVATE* Param=(NMITEMACTIVATE*)lParam;
					OnItemRightDbClick(this,Param->iItem,Param->iSubItem-1);
				}
				break;
			}
			Result=VL_WinControl::ProcessMessage(Message,wParam,lParam,CallDefaultProcedure);
			break;
		default:
			Result=VL_WinControl::ProcessMessage(Message,wParam,lParam,CallDefaultProcedure);
		}
		return Result;
	}

	VL_WinImageList* VL_WinListView::GetSmallImageList()
	{
		return FSmallImageList;
	}

	void VL_WinListView::SetSmallImageList(VL_WinImageList* ImageList)
	{
		ListView_SetImageList(FHandle,ImageList->GetHandle(),LVSIL_SMALL);
		FSmallImageList=ImageList;
	}

	VL_WinImageList* VL_WinListView::GetLargeImageList()
	{
		return FLargeImageList;
	}

	void VL_WinListView::SetLargeImageList(VL_WinImageList* ImageList)
	{
		ListView_SetImageList(FHandle,ImageList->GetHandle(),LVSIL_NORMAL);
		FLargeImageList=ImageList;
	}

	VL_WinImageList* VL_WinListView::GetStateImageList()
	{
		return FStateImageList;
	}

	void VL_WinListView::SetStateImageList(VL_WinImageList* ImageList)
	{
		ListView_SetImageList(FHandle,ImageList->GetHandle(),LVSIL_STATE);
		FStateImageList=ImageList;
	}

	VL_WinImageList* VL_WinListView::GetGroupImageList()
	{
		return FGroupImageList;
	}

	void VL_WinListView::SetGroupImageList(VL_WinImageList* ImageList)
	{
		ListView_SetImageList(FHandle,ImageList->GetHandle(),LVSIL_GROUPHEADER);
		FGroupImageList=ImageList;
	}

	VLE_ListViewStyle VL_WinListView::GetViewStyle()
	{
		switch(ListView_GetView(FHandle))
		{
		case LV_VIEW_ICON:
			return vlvsLarge;
		case LV_VIEW_SMALLICON:
			return vlvsSmall;
		case LV_VIEW_LIST:
			return vlvsList;
		case LV_VIEW_DETAILS:
			return vlvsReport;
		default:
			return vlvsTile;
		}
	}

	void VL_WinListView::SetViewStyle(VLE_ListViewStyle Style)
	{
		switch(Style)
		{
		case vlvsLarge:
			ListView_SetView(FHandle,LV_VIEW_ICON);
			break;
		case vlvsSmall:
			ListView_SetView(FHandle,LV_VIEW_SMALLICON);
			break;
		case vlvsList:
			ListView_SetView(FHandle,LV_VIEW_LIST);
			break;
		case vlvsReport:
			ListView_SetView(FHandle,LV_VIEW_DETAILS);
			break;
		case vlvsTile:
			ListView_SetView(FHandle,LV_VIEW_TILE);
			break;
		}
	}

	COLORREF VL_WinListView::GetOutlineColor()
	{
		return ListView_GetOutlineColor(FHandle);
	}

	void VL_WinListView::SetOutlineColor(COLORREF Value)
	{
		ListView_SetOutlineColor(FHandle,Value);
	}

	COLORREF VL_WinListView::GetTextColor()
	{
		return ListView_GetTextColor(FHandle);
	}
	
	void VL_WinListView::SetTextColor(COLORREF Value)
	{
		ListView_SetTextColor(FHandle,Value);
	}

	COLORREF VL_WinListView::GetTextBackColor()
	{
		return ListView_GetTextBkColor(FHandle);
	}

	void VL_WinListView::SetTextBackColor(COLORREF Value)
	{
		ListView_SetTextBkColor(FHandle,Value);
	}

	COLORREF VL_WinListView::GetBackColor()
	{
		return ListView_GetBkColor(FHandle);
	}

	void VL_WinListView::SetBackColor(COLORREF Value)
	{
		ListView_SetBkColor(FHandle,Value);
	}

	VBool VL_WinListView::GetAutoArrange()
	{
		return GetStyle(LVS_AUTOARRANGE);
	}

	void VL_WinListView::SetAutoArrange(VBool Value)
	{
		SetStyle(LVS_AUTOARRANGE,Value);
	}

	VBool VL_WinListView::GetEditable()
	{
		return GetStyle(LVS_EDITLABELS);
	}

	void VL_WinListView::SetEditable(VBool Value)
	{
		SetStyle(LVS_EDITLABELS,Value);
	}

	VBool VL_WinListView::GetColumnPushable()
	{
		return !GetStyle(LVS_NOSORTHEADER);
	}

	void VL_WinListView::SetColumnPushable(VBool Value)
	{
		SetStyle(LVS_NOSORTHEADER,!Value);
	}

	VBool VL_WinListView::GetAlwaysShowSelection()
	{
		return GetStyle(LVS_SHOWSELALWAYS);
	}

	void VL_WinListView::SetAlwaysShowSelection(VBool Value)
	{
		SetStyle(LVS_SHOWSELALWAYS,Value);
	}

	VBool VL_WinListView::GetSingleSelection()
	{
		return GetStyle(LVS_SINGLESEL);
	}

	void VL_WinListView::SetSingleSelection(VBool Value)
	{
		SetStyle(LVS_SINGLESEL,Value);
	}

	VBool VL_WinListView::GetShowCheckBoxes()
	{
		return GetExStyle(LVS_EX_CHECKBOXES);
	}

	void VL_WinListView::SetShowCheckBoxes(VBool Value)
	{
		SetExStyle(LVS_EX_CHECKBOXES,Value);
	}

	VBool VL_WinListView::GetAutoCheckSelect()
	{
		return GetExStyle(LVS_EX_AUTOCHECKSELECT);
	}

	void VL_WinListView::SetAutoCheckSelect(VBool Value)
	{
		SetExStyle(LVS_EX_AUTOCHECKSELECT,Value);
	}

	VBool VL_WinListView::GetBorderSelect()
	{
		return GetExStyle(LVS_EX_BORDERSELECT);
	}

	void VL_WinListView::SetBorderSelect(VBool Value)
	{
		SetExStyle(LVS_EX_BORDERSELECT,Value);
	}

	VBool VL_WinListView::GetShowGridLines()
	{
		return GetExStyle(LVS_EX_GRIDLINES);
	}

	void VL_WinListView::SetShowGridLines(VBool Value)
	{
		SetExStyle(LVS_EX_GRIDLINES,Value);
	}

	VBool VL_WinListView::GetHeaderDragable()
	{
		return GetExStyle(LVS_EX_HEADERDRAGDROP);
	}

	void VL_WinListView::SetHeaderDragable(VBool Value)
	{
		SetExStyle(LVS_EX_HEADERDRAGDROP,Value);
	}

	VBool VL_WinListView::GetOneClickActive()
	{
		return GetExStyle(LVS_EX_ONECLICKACTIVATE);
	}

	void VL_WinListView::SetOneClickActive(VBool Value)
	{
		SetExStyle(LVS_EX_ONECLICKACTIVATE,Value);
	}

	VBool VL_WinListView::GetTwoClickActive()
	{
		return GetExStyle(LVS_EX_TWOCLICKACTIVATE);
	}

	void VL_WinListView::SetTwoClickActive(VBool Value)
	{
		SetExStyle(LVS_EX_TWOCLICKACTIVATE,Value);
	}

	VBool VL_WinListView::GetUnderlineHot()
	{
		return GetExStyle(LVS_EX_UNDERLINEHOT);
	}

	void VL_WinListView::SetUnderlineHot(VBool Value)
	{
		SetExStyle(LVS_EX_UNDERLINEHOT,Value);
	}

	VBool VL_WinListView::GetUnderlineCold()
	{
		return GetExStyle(LVS_EX_UNDERLINECOLD);
	}

	void VL_WinListView::SetUnderlineCold(VBool Value)
	{
		SetExStyle(LVS_EX_UNDERLINECOLD,Value);
	}

	VBool VL_WinListView::GetAutoAutoArrange()
	{
		return GetExStyle(LVS_EX_AUTOAUTOARRANGE);
	}

	void VL_WinListView::SetAutoAutoArrange(VBool Value)
	{
		SetExStyle(LVS_EX_AUTOAUTOARRANGE,Value);
	}

	VBool VL_WinListView::GetSnapToGrid()
	{
		return GetExStyle(LVS_EX_SNAPTOGRID);
	}

	void VL_WinListView::SetSnapToGrid(VBool Value)
	{
		SetExStyle(LVS_EX_SNAPTOGRID,Value);
	}

	void VL_WinListView::EditLabel(VInt Index)
	{
		ListView_EditLabel(FHandle,Index);
	}

	void VL_WinListView::CancelEditLabel()
	{
		ListView_CancelEditLabel(FHandle);
	}

	void VL_WinListView::Arrange()
	{
		ListView_Arrange(FHandle,LVA_DEFAULT);
	}

	void VL_WinListView::SnapToGrid()
	{
		ListView_Arrange(FHandle,LVA_SNAPTOGRID);
	}

	void VL_WinListView::EnsureItemVisible(VInt Index , VBool AcceptPartialVisible)
	{
		ListView_EnsureVisible(FHandle,Index,(AcceptPartialVisible?TRUE:FALSE));
	}

	void VL_WinListView::Sort(VPointer Param)
	{
		ListViewSortParam P;
		P.Param=Param;
		P.ListView=this;
		ListView_SortItemsEx(FHandle,ListViewCompareFunc,&P);
	}

	VInt VL_WinListView::GetItemCount()
	{
		return ListView_GetItemCount(FHandle);
	}

	void VL_WinListView::AddItem(VUnicodeString Text , VInt ImageIndex)
	{
		InsertItem(GetItemCount(),Text,ImageIndex);
	}

	void VL_WinListView::InsertItem(VInt Index , VUnicodeString Text , VInt ImageIndex)
	{
		LVITEM Item;
		memset(&Item,0,sizeof(Item));
		Item.mask=LVIF_TEXT | (ImageIndex==-1?0:LVIF_IMAGE) | LVIF_PARAM;
		Item.pszText=Text.Buffer();
		Item.cchTextMax=Text.Length()+1;
		Item.iItem=Index;
		Item.iSubItem=0;
		Item.iImage=ImageIndex;
		Item.lParam=0;
		ListView_InsertItem(FHandle,&Item);
	}

	void VL_WinListView::DeleteItem(VInt Index)
	{
		ListView_DeleteItem(FHandle,Index);
	}

	void VL_WinListView::Clear()
	{
		ListView_DeleteAllItems(FHandle);
	}

	VL_WinListViewItem VL_WinListView::GetItem(VInt Index)
	{
		if(Index>=0 && Index<GetItemCount())
		{
			return VL_WinListViewItem(FHandle,Index);
		}
		else
		{
			return VL_WinListViewItem();
		}
	}

	VInt VL_WinListView::GetSelectedItemCount()
	{
		return ListView_GetSelectedCount(FHandle);
	}

	VInt VL_WinListView::GetSelectionItemMark()
	{
		return ListView_GetSelectionMark(FHandle);
	}

	void VL_WinListView::SetSelectionItemMark(VInt Value)
	{
		ListView_SetSelectionMark(FHandle,Value);
	}

	VInt VL_WinListView::GetHotItemIndex()
	{
		return ListView_GetHotItem(FHandle);
	}

	void VL_WinListView::SetHotItemIndex(VInt Value)
	{
		ListView_SetHotItem(FHandle,Value);
	}

	VInt VL_WinListView::GetColumnCount()
	{
		return FColumnCount;
	}

	void VL_WinListView::AddColumn(VUnicodeString Text , VInt SubItemIndex)
	{
		InsertColumn(GetColumnCount(),Text,SubItemIndex);
	}

	void VL_WinListView::InsertColumn(VInt Index , VUnicodeString Text , VInt SubItemIndex)
	{
		LVCOLUMN Column;
		memset(&Column,0,sizeof(Column));
		Column.mask=LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
		Column.pszText=Text.Buffer();
		Column.cchTextMax=Text.Length()+1;
		Column.iSubItem=SubItemIndex+1;
		Column.cx=100;
		if(ListView_InsertColumn(FHandle,Index,&Column)!=-1)
		{
			FColumnCount++;
		}
	}

	void VL_WinListView::DeleteColumn(VInt Index)
	{
		if(ListView_DeleteColumn(FHandle,Index)==TRUE)
		{
			FColumnCount--;
		}
	}

	VL_WinListViewColumn VL_WinListView::GetColumn(VInt Index)
	{
		if(Index>=0 && Index<GetColumnCount())
		{
			return VL_WinListViewColumn(FHandle,Index);
		}
		else
		{
			return VL_WinListViewColumn();
		}
	}

	VInt VL_WinListView::GetSelectedColumnIndex()
	{
		return ListView_GetSelectedColumn(FHandle);
	}
	
	void VL_WinListView::SetSelectedColumnIndex(VInt Value)
	{
		ListView_SetSelectedColumn(FHandle,Value);
	}

	void VL_WinListView::EnableGrouping(VBool Value)
	{
		ListView_EnableGroupView(FHandle,(Value?TRUE:FALSE));
	}

	VInt VL_WinListView::GetGroupCount()
	{
		return ListView_GetGroupCount(FHandle);
	}

	VInt VL_WinListView::AddGroup(VUnicodeString Text)
	{
		return InsertGroup(-1,Text);
	}

	VInt VL_WinListView::InsertGroup(VInt Index , VUnicodeString Text)
	{
		LVGROUP Group;
		memset(&Group,0,sizeof(Group));
		Group.cbSize=sizeof(Group);
		Group.mask=LVGF_HEADER | LVGF_GROUPID;
		Group.pszHeader=Text.Buffer();
		Group.iGroupId=FUsedGroupIDs;
		ListView_InsertGroup(FHandle,Index,&Group);
		return FUsedGroupIDs++;
	}

	void VL_WinListView::DeleteGroup(VInt Index)
	{
		VL_WinListViewGroup Group=GetGroup(Index);
		if(Group)
		{
			ListView_RemoveGroup(FHandle,Group.GetGroupID());
		}
	}

	void VL_WinListView::DeleteGroupByID(VInt ID)
	{
		ListView_RemoveGroup(FHandle,ID);
	}

	VL_WinListViewGroup VL_WinListView::GetGroup(VInt Index)
	{
		if(Index>=0 && Index<GetGroupCount())
		{
			LVGROUP Group;
			FillListViewGroup(&Group,LVGF_GROUPID);
			ListView_GetGroupInfoByIndex(FHandle,Index,&Group);
			return GetGroupByID(Group.iGroupId);
		}
		else
		{
			return VL_WinListViewGroup();
		}
	}

	VL_WinListViewGroup VL_WinListView::GetGroupByID(VInt ID)
	{
		if(ListView_HasGroup(FHandle,ID)==TRUE)
		{
			return VL_WinListViewGroup(FHandle,ID);
		}
		else
		{
			return VL_WinListViewGroup();
		}
	}

/*********************************************************************************************************
VL_WinTreeViewItem
*********************************************************************************************************/

	void FillTreeViewInsertItem(TVINSERTSTRUCT* Item , VUnicodeString Text , VInt ImageIndex , VInt SelectedImageIndex)
	{
		if(SelectedImageIndex==-1)
		{
			SelectedImageIndex=ImageIndex;
		}
		memset(Item,0,sizeof(*Item));
		Item->itemex.mask=TVIF_TEXT | (ImageIndex==-1?0:TVIF_IMAGE | TVIF_SELECTEDIMAGE);
		if(Text.Length())
		{
			Item->itemex.pszText=Text.Buffer();
		}
		if(ImageIndex!=-1)
		{
			Item->itemex.iImage=ImageIndex;
			Item->itemex.iSelectedImage=SelectedImageIndex;
		}
	}

	void FillTreeViewItem(TVITEMEX* Item , HTREEITEM Handle , UINT Mask)
	{
		memset(Item,0,sizeof(*Item));
		Item->hItem=Handle;
		Item->mask=Mask;
	}

	VL_WinTreeViewItem::VL_WinTreeViewItem(HWND Handle , HTREEITEM Item)
	{
		FHandle=Handle;
		FItem=Item;
	}

	VL_WinTreeViewItem::VL_WinTreeViewItem(const VL_WinTreeViewItem& Item)
	{
		FHandle=Item.FHandle;
		FItem=Item.FItem;
	}

	VL_WinTreeViewItem::VL_WinTreeViewItem()
	{
		FHandle=0;
		FItem=0;
	}

	VL_WinTreeViewItem& VL_WinTreeViewItem::operator=(const VL_WinTreeViewItem& Item)
	{
		FHandle=Item.FHandle;
		FItem=Item.FItem;
		return *this;
	}

	VL_WinTreeViewItem::operator VBool()
	{
		return FItem!=0;
	}

	VL_WinTreeViewItem VL_WinTreeViewItem::InsertAtLast(VUnicodeString Text , VInt ImageIndex , VInt SelectedImageIndex)
	{
		TVINSERTSTRUCT Item;
		FillTreeViewInsertItem(&Item,Text,ImageIndex,SelectedImageIndex);
		Item.hParent=FItem;
		Item.hInsertAfter=TVI_LAST;
		return VL_WinTreeViewItem(FHandle,TreeView_InsertItem(FHandle,&Item));
	}

	VL_WinTreeViewItem VL_WinTreeViewItem::InsertAtFirst(VUnicodeString Text , VInt ImageIndex , VInt SelectedImageIndex)
	{
		TVINSERTSTRUCT Item;
		FillTreeViewInsertItem(&Item,Text,ImageIndex,SelectedImageIndex);
		Item.hParent=FItem;
		Item.hInsertAfter=TVI_FIRST;
		return VL_WinTreeViewItem(FHandle,TreeView_InsertItem(FHandle,&Item));
	}

	VL_WinTreeViewItem VL_WinTreeViewItem::InsertAfter(VUnicodeString Text , VInt ImageIndex , VInt SelectedImageIndex)
	{
		TVINSERTSTRUCT Item;
		FillTreeViewInsertItem(&Item,Text,ImageIndex,SelectedImageIndex);
		Item.hParent=GetParent().FItem;
		Item.hInsertAfter=FItem;
		return VL_WinTreeViewItem(FHandle,TreeView_InsertItem(FHandle,&Item));
	}

	void VL_WinTreeViewItem::DeleteSelf()
	{
		TreeView_DeleteItem(FHandle,FItem);
		FHandle=0;
		FItem=0;
	}

	VL_WinTreeViewItem VL_WinTreeViewItem::GetParent()
	{
		return VL_WinTreeViewItem(FHandle,TreeView_GetParent(FHandle,FItem));
	}

	VL_WinTreeViewItem VL_WinTreeViewItem::GetFirstChild()
	{
		return VL_WinTreeViewItem(FHandle,TreeView_GetChild(FHandle,FItem));
	}

	VL_WinTreeViewItem VL_WinTreeViewItem::GetPrevious()
	{
		return VL_WinTreeViewItem(FHandle,TreeView_GetPrevSibling(FHandle,FItem));
	}

	VL_WinTreeViewItem VL_WinTreeViewItem::GetNext()
	{
		return VL_WinTreeViewItem(FHandle,TreeView_GetNextSibling(FHandle,FItem));
	}

	VL_WinTreeViewItem VL_WinTreeViewItem::GetNextVisible()
	{
		return VL_WinTreeViewItem(FHandle,TreeView_GetNextVisible(FHandle,FItem));
	}

	VL_WinTreeViewItem VL_WinTreeViewItem::GetPrevVisible()
	{
		return VL_WinTreeViewItem(FHandle,TreeView_GetPrevVisible(FHandle,FItem));
	}

	VL_WinTreeViewItem VL_WinTreeViewItem::GetNextSelected()
	{
		return VL_WinTreeViewItem(FHandle,TreeView_GetNextSelected(FHandle,FItem));
	}

	HTREEITEM VL_WinTreeViewItem::GetHandle()
	{
		return FItem;
	}

	void VL_WinTreeViewItem::SetDropTarget()
	{
		TreeView_SelectDropTarget(FHandle,FItem);
	}

	void VL_WinTreeViewItem::SetFirstVisible()
	{
		TreeView_SelectSetFirstVisible(FHandle,FItem);
	}

	VUnicodeString VL_WinTreeViewItem::GetText()
	{
		VWChar Buffer[LISTVIEW_TEXT_BUFFER_SIZE]={0};
		TVITEMEX Item;
		FillTreeViewItem(&Item,FItem,TVIF_TEXT);
		Item.pszText=Buffer;
		Item.cchTextMax=sizeof(Buffer)/sizeof(*Buffer);
		TreeView_GetItem(FHandle,&Item);
		return Buffer;
	}

	void VL_WinTreeViewItem::SetText(VUnicodeString Value)
	{
		TVITEMEX Item;
		FillTreeViewItem(&Item,FItem,TVIF_TEXT);
		if(Value.Length())
		{
			Item.pszText=Value.Buffer();
		}
		TreeView_SetItem(FHandle,&Item);
	}

	VInt VL_WinTreeViewItem::GetImageIndex()
	{
		TVITEMEX Item;
		FillTreeViewItem(&Item,FItem,TVIF_IMAGE);
		TreeView_GetItem(FHandle,&Item);
		return Item.iImage;
	}

	void VL_WinTreeViewItem::SetImageIndex(VInt Value)
	{
		TVITEMEX Item;
		FillTreeViewItem(&Item,FItem,TVIF_IMAGE);
		Item.iImage=Value;
		TreeView_SetItem(FHandle,&Item);
	}

	VInt VL_WinTreeViewItem::GetSelectedImageIndex()
	{
		TVITEMEX Item;
		FillTreeViewItem(&Item,FItem,TVIF_SELECTEDIMAGE);
		TreeView_GetItem(FHandle,&Item);
		return Item.iSelectedImage;
	}

	void VL_WinTreeViewItem::SetSelectedImageIndex(VInt Value)
	{
		TVITEMEX Item;
		FillTreeViewItem(&Item,FItem,TVIF_SELECTEDIMAGE);
		Item.iSelectedImage=Value;
		TreeView_SetItem(FHandle,&Item);
	}

	VPointer VL_WinTreeViewItem::GetCustomData()
	{
		TVITEMEX Item;
		FillTreeViewItem(&Item,FItem,TVIF_PARAM);
		TreeView_GetItem(FHandle,&Item);
		return (VPointer)Item.lParam;
	}

	void VL_WinTreeViewItem::SetCustomData(VPointer Value)
	{
		TVITEMEX Item;
		FillTreeViewItem(&Item,FItem,TVIF_PARAM);
		Item.lParam=(LPARAM)Value;
		TreeView_SetItem(FHandle,&Item);
	}

	VBool VL_WinTreeViewItem::GetBoldState()
	{
		return TreeView_GetItemState(FHandle,FItem,TVIS_BOLD)!=0;
	}

	void VL_WinTreeViewItem::SetBoldState(VBool Value)
	{
		TreeView_SetItemState(FHandle,FItem,(Value?TVIS_BOLD:0),TVIS_BOLD);
	}

	VBool VL_WinTreeViewItem::GetCutState()
	{
		return TreeView_GetItemState(FHandle,FItem,TVIS_CUT)!=0;
	}
	
	void VL_WinTreeViewItem::SetCutState(VBool Value)
	{
		TreeView_SetItemState(FHandle,FItem,(Value?TVIS_CUT:0),TVIS_CUT);
	}

	VBool VL_WinTreeViewItem::GetDropHilitedState()
	{
		return TreeView_GetItemState(FHandle,FItem,TVIS_DROPHILITED)!=0;
	}

	void VL_WinTreeViewItem::SetDropHilitedState(VBool Value)
	{
		TreeView_SetItemState(FHandle,FItem,(Value?TVIS_DROPHILITED:0),TVIS_DROPHILITED);
	}

	VBool VL_WinTreeViewItem::GetExpendedState()
	{
		return TreeView_GetItemState(FHandle,FItem,TVIS_EXPANDED)!=0;
	}

	void VL_WinTreeViewItem::SetExpendedState(VBool Value)
	{
		TreeView_SetItemState(FHandle,FItem,(Value?TVIS_EXPANDED:0),TVIS_EXPANDED);
	}

	VBool VL_WinTreeViewItem::GetSelectedState()
	{
		return TreeView_GetItemState(FHandle,FItem,TVIS_SELECTED)!=0;
	}

	void VL_WinTreeViewItem::SetSelectedState(VBool Value)
	{
		TreeView_SetItemState(FHandle,FItem,(Value?TVIS_SELECTED:0),TVIS_SELECTED);
	}

	VBool VL_WinTreeViewItem::GetChecked()
	{
		return TreeView_GetCheckState(FHandle,FItem)==TRUE;
	}

	void VL_WinTreeViewItem::SetChecked(VBool Value)
	{
		TreeView_SetCheckState(FHandle,FItem,(Value?TRUE:FALSE));
	}

	void VL_WinTreeViewItem::EnsureVisible()
	{
		TreeView_EnsureVisible(FHandle,FItem);
	}

	void VL_WinTreeViewItem::Expand()
	{
		TreeView_Expand(FHandle,FItem,TVE_EXPAND);
	}

	void VL_WinTreeViewItem::Collapse()
	{
		TreeView_Expand(FHandle,FItem,TVE_COLLAPSE);
	}

	void VL_WinTreeViewItem::Toggle()
	{
		TreeView_Expand(FHandle,FItem,TVE_TOGGLE);
	}

/*********************************************************************************************************
VL_WinTreeView
*********************************************************************************************************/

	VL_WinTreeView::VL_WinTreeView(VL_WinContainer* Parent)
	{
		FNormalImageList=0;
		FStateImageList=0;

		_CreateWindow(
			TVS_EX_DOUBLEBUFFER ,
			WS_VSCROLL | WS_HSCROLL | WS_BORDER | WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | TVS_DISABLEDRAGDROP | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT,
			WC_TREEVIEW,
			Parent);
		TreeView_SetUnicodeFormat(FHandle,TRUE);
		//SetExStyle(TVS_EX_DOUBLEBUFFER,true);
	}

	VL_WinTreeView::~VL_WinTreeView()
	{
	}

	LRESULT VL_WinTreeView::ProcessMessage(UINT Message , WPARAM& wParam , LPARAM& lParam , VBool& CallDefaultProcedure)
	{
		LRESULT Result=0;
		switch(Message)
		{
		case WM_NOTIFY_DISPATCHED:
			switch(((NMHDR*)lParam)->code)
			{
			case NM_CLICK:
				{
					OnClick(this);
				}
				break;
			case NM_DBLCLK:
				{
					OnDbClick(this);
				}
				break;
			case NM_RCLICK:
				{
					OnRightClick(this);
				}
				break;
			case NM_RDBLCLK:
				{
					OnRightDbClick(this);
				}
				break;
			case TVN_BEGINDRAG:
				{
					NMTREEVIEW* Param=(NMTREEVIEW*)lParam;
					OnBeginDrag(this,VL_WinTreeViewItem(FHandle,Param->itemNew.hItem));
				}
				break;
			case TVN_BEGINRDRAG:
				{
					NMTREEVIEW* Param=(NMTREEVIEW*)lParam;
					OnBeginRightDrag(this,VL_WinTreeViewItem(FHandle,Param->itemNew.hItem));
				}
				break;
			case TVN_BEGINLABELEDIT:
				{
					CallDefaultProcedure=false;
					NMTVDISPINFO* Param=(NMTVDISPINFO*)lParam;
					VBool Accept=true;
					OnBeginLabelEdit(this,VL_WinTreeViewItem(FHandle,Param->item.hItem),Accept,(Param->item.pszText?Param->item.pszText:L""));
					return Accept?FALSE:TRUE;
				}
				break;
			case TVN_ENDLABELEDIT:
				{
					CallDefaultProcedure=false;
					NMTVDISPINFO* Param=(NMTVDISPINFO*)lParam;
					VBool Accept=true;
					OnEndLabelEdit(this,VL_WinTreeViewItem(FHandle,Param->item.hItem),Accept,(Param->item.pszText?Param->item.pszText:L""));
					return Accept?TRUE:FALSE;
				}
				break;
			case TVN_ITEMEXPANDING:
				{
					NMTREEVIEW* Param=(NMTREEVIEW*)lParam;
					OnItemExpanding(this,VL_WinTreeViewItem(FHandle,Param->itemNew.hItem));
				}
				break;
			case TVN_ITEMEXPANDED:
				{
					NMTREEVIEW* Param=(NMTREEVIEW*)lParam;
					OnItemExpanded(this,VL_WinTreeViewItem(FHandle,Param->itemNew.hItem));
				}
				break;
			case TVN_SELCHANGING:
				{
					CallDefaultProcedure=false;
					NMTREEVIEW* Param=(NMTREEVIEW*)lParam;
					VBool Accept=true;
					OnItemSelecting(this,VL_WinTreeViewItem(FHandle,Param->itemNew.hItem),Accept);
					return Accept?FALSE:TRUE;
				}
				break;
			case TVN_SELCHANGED:
				{
					NMTREEVIEW* Param=(NMTREEVIEW*)lParam;
					OnItemSelected(this,VL_WinTreeViewItem(FHandle,Param->itemNew.hItem));
				}
				break;
			}
			Result=VL_WinControl::ProcessMessage(Message,wParam,lParam,CallDefaultProcedure);
			break;
		default:
			Result=VL_WinControl::ProcessMessage(Message,wParam,lParam,CallDefaultProcedure);
		}
		return Result;
	}

	VL_WinImageList* VL_WinTreeView::GetNormalImageList()
	{
		return FNormalImageList;
	}

	void VL_WinTreeView::SetNormalImageList(VL_WinImageList* ImageList)
	{
		TreeView_SetImageList(FHandle,ImageList->GetHandle(),TVSIL_NORMAL);
		FNormalImageList=ImageList;
	}

	VL_WinImageList* VL_WinTreeView::GetStateImageList()
	{
		return FStateImageList;
	}

	void VL_WinTreeView::SetStateImageList(VL_WinImageList* ImageList)
	{
		TreeView_SetImageList(FHandle,ImageList->GetHandle(),TVSIL_STATE);
		FStateImageList=ImageList;
	}

	COLORREF VL_WinTreeView::GetLineColor()
	{
		return TreeView_GetLineColor(FHandle);
	}

	void VL_WinTreeView::SetLineColor(COLORREF Value)
	{
		TreeView_SetLineColor(FHandle,Value);
	}

	COLORREF VL_WinTreeView::GetTextColor()
	{
		return TreeView_GetTextColor(FHandle);
	}

	void VL_WinTreeView::SetTextColor(COLORREF Value)
	{
		TreeView_SetTextColor(FHandle,Value);
	}

	COLORREF VL_WinTreeView::GetBackColor()
	{
		return TreeView_GetBkColor(FHandle);
	}

	void VL_WinTreeView::SetBackColor(COLORREF Value)
	{
		TreeView_SetBkColor(FHandle,Value);
	}

	VBool VL_WinTreeView::GetShowCheckBoxes()
	{
		return GetStyle(TVS_CHECKBOXES);
	}

	void VL_WinTreeView::SetShowCheckBoxes(VBool Value)
	{
		SetStyle(TVS_CHECKBOXES,Value);
	}

	VBool VL_WinTreeView::GetEnableDragDrop()
	{
		return !GetStyle(TVS_DISABLEDRAGDROP);
	}

	void VL_WinTreeView::SetEnableDragDrop(VBool Value)
	{
		SetStyle(TVS_DISABLEDRAGDROP,!Value);
	}

	VBool VL_WinTreeView::GetEditable()
	{
		return GetStyle(TVS_EDITLABELS);
	}

	void VL_WinTreeView::SetEditable(VBool Value)
	{
		SetStyle(TVS_EDITLABELS,Value);
	}

	VBool VL_WinTreeView::GetFullRowSelect()
	{
		return GetStyle(TVS_FULLROWSELECT);
	}

	void VL_WinTreeView::SetFullRowSelect(VBool Value)
	{
		SetStyle(TVS_FULLROWSELECT,Value);
	}

	VBool VL_WinTreeView::GetHasButtons()
	{
		return GetStyle(TVS_HASBUTTONS);
	}

	void VL_WinTreeView::SetHasButtons(VBool Value)
	{
		SetStyle(TVS_HASBUTTONS,Value);
	}

	VBool VL_WinTreeView::GetHasLines()
	{
		return GetStyle(TVS_HASLINES);
	}

	void VL_WinTreeView::SetHasLines(VBool Value)
	{
		SetStyle(TVS_HASLINES,Value);
	}

	VBool VL_WinTreeView::GetAlwaysShowSelection()
	{
		return GetStyle(TVS_SHOWSELALWAYS);
	}

	void VL_WinTreeView::SetAlwaysShowSelection(VBool Value)
	{
		SetStyle(TVS_SHOWSELALWAYS,Value);
	}

	VBool VL_WinTreeView::GetDimmedCheckBoxes()
	{
		return GetExStyle(TVS_EX_DIMMEDCHECKBOXES);
	}

	void VL_WinTreeView::SetDimmedCheckBoxes(VBool Value)
	{
		SetExStyle(TVS_EX_DIMMEDCHECKBOXES,Value);
	}

	VBool VL_WinTreeView::GetExclusionCheckBoxes()
	{
		return GetExStyle(TVS_EX_EXCLUSIONCHECKBOXES);
	}

	void VL_WinTreeView::SetExclusionCheckBoxes(VBool Value)
	{
		SetExStyle(TVS_EX_EXCLUSIONCHECKBOXES,Value);
	}

	VBool VL_WinTreeView::GetFadeButton()
	{
		return GetExStyle(TVS_EX_FADEINOUTEXPANDOS);
	}

	void VL_WinTreeView::SetFadeButton(VBool Value)
	{
		SetExStyle(TVS_EX_FADEINOUTEXPANDOS,Value);
	}

	void VL_WinTreeView::EditLabel(VL_WinTreeViewItem Item)
	{
		TreeView_EditLabel(FHandle,Item.FItem);
	}

	void VL_WinTreeView::FinishEditLabel()
	{
		TreeView_EndEditLabelNow(FHandle,FALSE);
	}

	void VL_WinTreeView::CancelEditLabel()
	{
		TreeView_EndEditLabelNow(FHandle,TRUE);
	}

	VL_WinTreeViewItem VL_WinTreeView::AddRootItem(VUnicodeString Text , VInt ImageIndex , VInt SelectedImageIndex)
	{
		TVINSERTSTRUCT Item;
		FillTreeViewInsertItem(&Item,Text,ImageIndex,SelectedImageIndex);
		Item.hParent=NULL;
		Item.hInsertAfter=TVI_ROOT;
		return VL_WinTreeViewItem(FHandle,TreeView_InsertItem(FHandle,&Item));
	}

	VL_WinTreeViewItem VL_WinTreeView::GetFirstRoot()
	{
		return VL_WinTreeViewItem(FHandle,TreeView_GetRoot(FHandle));
	}

	VL_WinTreeViewItem VL_WinTreeView::GetCurrentSelectedItem()
	{
		return VL_WinTreeViewItem(FHandle,TreeView_GetSelection(FHandle));
	}

	VL_WinTreeViewItem VL_WinTreeView::GetFirstVisible()
	{
		return VL_WinTreeViewItem(FHandle,TreeView_GetFirstVisible(FHandle));
	}

	VL_WinTreeViewItem VL_WinTreeView::GetLastVisible()
	{
		return VL_WinTreeViewItem(FHandle,TreeView_GetLastVisible(FHandle));
	}

	VL_WinTreeViewItem VL_WinTreeView::GetDropHighlighted()
	{
		return VL_WinTreeViewItem(FHandle,TreeView_GetDropHilight(FHandle));
	}

	void VL_WinTreeView::Clear()
	{
		TreeView_DeleteAllItems(FHandle);
	}

	}
}