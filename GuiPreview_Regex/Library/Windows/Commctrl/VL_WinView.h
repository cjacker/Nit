/*******************************************************************************
Vczh Library++ 2.0
Windows界面::数据
开发者：陈梓瀚

接口：
类：
  VL_WinListViewItem			：ListView项目
  VL_WinListViewColumn			：ListView列头
  VL_WinListViewGroup			：ListView组
  VL_WinListView				：ListView

  VL_WinTreeViewItem			：TreeView项目
  VL_WinTreeView				：TreeView
函数：
*******************************************************************************/

#ifndef VL_WINVIEW
#define VL_WINVIEW

#include "..\VL_WinClass.h"
#include "commctrl.h"

namespace vl
{
	namespace windows
	{

/*********************************************************************************************************
List View
*********************************************************************************************************/

		class VL_WinListViewItem : public VL_Base
		{
			friend class VL_WinListView;
		protected:
			HWND					FHandle;
			VInt					FIndex;

			VL_WinListViewItem(HWND Handle , VInt Index);
		public:
			VL_WinListViewItem();
			VL_WinListViewItem(const VL_WinListViewItem& Item);
			VL_WinListViewItem& operator=(const VL_WinListViewItem& Item);
			operator VBool();

			VInt					GetIndex();

			VUnicodeString			GetText();
			void					SetText(VUnicodeString Value);

			VInt					GetImageIndex();
			void					SetImageIndex(VInt Value);

			VUnicodeString			GetSubItem(VInt Index);
			void					SetSubItem(VInt Index , VUnicodeString Value);

			void					GetTiledSubItem(VL_List<VInt , true>& Columns);
			void					SetTiledSubItem(VL_List<VInt , true>& Columns);

			VBool					GetCutState();
			void					SetCutState(VBool Value);

			VBool					GetDropHilitedState();
			void					SetDropHilitedState(VBool Value);

			VBool					GetFocusedState();
			void					SetFocusedState(VBool Value);

			VBool					GetSelectedState();
			void					SetSelectedState(VBool Value);

			VInt					GetIndent();
			void					SetIndent(VInt Value);

			VBool					GetChecked();
			void					SetChecked(VBool Value);

			VPointer				GetCustomData();
			void					SetCustomData(VPointer Data);

			void					EnterGroup(VInt GroupID);
			void					LeaveGroup();
			VInt					GetOwnerGroupID();
			VBool					IsInGroup();
		};

		class VL_WinListViewColumn : public VL_Base
		{
			friend class VL_WinListView;
		protected:
			HWND					FHandle;
			VInt					FIndex;

			VL_WinListViewColumn(HWND Handle , VInt Index);
		public:
			VL_WinListViewColumn();
			VL_WinListViewColumn(const VL_WinListViewColumn& Item);
			VL_WinListViewColumn& operator=(const VL_WinListViewColumn& Item);
			operator VBool();

			VInt					GetIndex();

			VUnicodeString			GetText();
			void					SetText(VUnicodeString Value);

			VInt					GetSubItemIndex();
			void					SetSubItemIndex(VInt Value);

			VInt					GetWidth();
			void					SetWidth(VInt Value);

			VInt					GetOrder();
			void					SetOrder(VInt Value);

			VInt					GetImageIndex();
			void					SetImageIndex(VInt Value);

			VLE_WinHAlign			GetTextAlign();
			void					SetTextAlign(VLE_WinHAlign Value);

			VBool					GetImageOnRight();
			void					SetImageOnRight(VBool Value);

			VBool					GetFixedWidth();
			void					SetFixedWidth(VBool Value);

			VBool					GetShowSplitButton();
			void					SetShowSplitButton(VBool Value);
		};

		class VL_WinListViewGroup : public VL_Base
		{
			friend class VL_WinListView;
		protected:
			HWND					FHandle;
			VInt					FID;

			VL_WinListViewGroup(HWND Handle , VInt ID);

			VUnicodeString			GetListViewGroupText(UINT Mask , LPWSTR LVGROUP::* Item , UINT LVGROUP::* Count);
			VUnicodeString			GetListViewGroupText(UINT Mask , LPWSTR LVGROUP::* Item , int LVGROUP::* Count);
			void					SetListViewGroupText(UINT Mask , LPWSTR LVGROUP::* Item , VUnicodeString Text);
		public:
			VL_WinListViewGroup();
			VL_WinListViewGroup(const VL_WinListViewGroup& Item);
			VL_WinListViewGroup& operator=(const VL_WinListViewGroup& Item);
			operator VBool();

			VInt					GetGroupID();

			VUnicodeString			GetHeader();
			void					SetHeader(VUnicodeString Value);

			VUnicodeString			GetFooter();
			void					SetFooter(VUnicodeString Value);

			VUnicodeString			GetSubTitle();
			void					SetSubTitle(VUnicodeString Value);

			VUnicodeString			GetTask();
			void					SetTask(VUnicodeString Value);

			VUnicodeString			GetDescriptionTop();
			void					SetDescriptionTop(VUnicodeString Value);

			VUnicodeString			GetDescriptionBottom();
			void					SetDescriptionBottom(VUnicodeString Value);

			VInt					GetImageIndex();
			void					SetImageIndex(VInt Value);

			VInt					GetExtendedImageIndex();
			void					SetExtendedImageIndex(VInt Value);

			VLE_WinHAlign			GetHeaderAlign();
			void					SetHeaderAlign(VLE_WinHAlign Value);
		};

		enum VLE_ListViewStyle
		{
			vlvsLarge,
			vlvsSmall,
			vlvsList,
			vlvsReport,
			vlvsTile
		};

		enum VLE_ListViewSortResult
		{
			vlvsrLarger,
			vlvsrEqual,
			vlvsrSmaller
		};
		VL_DEFINE_VOID_EVENT	(VE_ListViewItemEvent	,(Sender , ItemIndex , SubItemIndex)		,(VL_Base* Sender , VInt ItemIndex , VInt SubItemIndex));
		VL_DEFINE_VOID_EVENT	(VE_ListViewEditEvent	,(Sender , ItemIndex , Accept , Text)		,(VL_Base* Sender , VInt ItemIndex , VBool& Accept , VUnicodeString Text));
		VL_DEFINE_VOID_EVENT	(VE_ListViewSortEvent	,(Sender , Index1 , Index2 , Param , Result),(VL_Base* Sender , VInt Index1 , VInt Index2 , VPointer Param , VLE_ListViewSortResult& Result));

		class VL_WinListView : public VL_WinControl
		{
		protected:
			VL_WinImageList*		FSmallImageList;
			VL_WinImageList*		FLargeImageList;
			VL_WinImageList*		FStateImageList;
			VL_WinImageList*		FGroupImageList;

			VInt					FColumnCount;
			VInt					FUsedGroupIDs;

			DWORD					InternalGetExStyle();
			void					InternalSetExStyle(DWORD ExStyle);
		public:
			VL_WinListView(VL_WinContainer* Parent);
			~VL_WinListView();
			LRESULT					ProcessMessage(UINT Message , WPARAM& wParam , LPARAM& lParam , VBool& CallDefaultProcedure);

			VE_ListViewItemEvent	OnBeginDrag;
			VE_ListViewItemEvent	OnBeginRightDrag;
			VE_ListViewEditEvent	OnBeginLabelEdit;
			VE_ListViewEditEvent	OnEndLabelEdit;
			VE_ListViewItemEvent	OnColumnClick;
			VE_ListViewItemEvent	OnColumnDropDown;
			VE_ListViewItemEvent	OnItemHotTrack;
			VE_ListViewItemEvent	OnItemActive;
			VE_ListViewItemEvent	OnItemChanging;
			VE_ListViewItemEvent	OnItemChanged;
			VE_ListViewItemEvent	OnGroupTaskClick;
			VE_ListViewItemEvent	OnItemClick;
			VE_ListViewItemEvent	OnItemDbClick;
			VE_ListViewItemEvent	OnItemRightClick;
			VE_ListViewItemEvent	OnItemRightDbClick;
			VE_ListViewSortEvent	OnCompareItems;

			VL_WinImageList*		GetSmallImageList();
			void					SetSmallImageList(VL_WinImageList* ImageList);

			VL_WinImageList*		GetLargeImageList();
			void					SetLargeImageList(VL_WinImageList* ImageList);

			VL_WinImageList*		GetStateImageList();
			void					SetStateImageList(VL_WinImageList* ImageList);

			VL_WinImageList*		GetGroupImageList();
			void					SetGroupImageList(VL_WinImageList* ImageList);

			VLE_ListViewStyle		GetViewStyle();
			void					SetViewStyle(VLE_ListViewStyle Style);

			COLORREF				GetOutlineColor();
			void					SetOutlineColor(COLORREF Value);

			COLORREF				GetTextColor();
			void					SetTextColor(COLORREF Value);

			COLORREF				GetTextBackColor();
			void					SetTextBackColor(COLORREF Value);

			COLORREF				GetBackColor();
			void					SetBackColor(COLORREF Value);

			/**********************************************************
			Styles
			**********************************************************/

			VBool					GetAutoArrange();
			void					SetAutoArrange(VBool Value);

			VBool					GetEditable();
			void					SetEditable(VBool Value);

			VBool					GetColumnPushable();
			void					SetColumnPushable(VBool Value);

			VBool					GetAlwaysShowSelection();
			void					SetAlwaysShowSelection(VBool Value);

			VBool					GetSingleSelection();
			void					SetSingleSelection(VBool Value);

			VBool					GetShowCheckBoxes();
			void					SetShowCheckBoxes(VBool Value);

			VBool					GetAutoCheckSelect();
			void					SetAutoCheckSelect(VBool Value);

			VBool					GetBorderSelect();
			void					SetBorderSelect(VBool Value);

			VBool					GetShowGridLines();
			void					SetShowGridLines(VBool Value);

			VBool					GetHeaderDragable();
			void					SetHeaderDragable(VBool Value);

			VBool					GetOneClickActive();
			void					SetOneClickActive(VBool Value);

			VBool					GetTwoClickActive();
			void					SetTwoClickActive(VBool Value);

			VBool					GetUnderlineHot();
			void					SetUnderlineHot(VBool Value);

			VBool					GetUnderlineCold();
			void					SetUnderlineCold(VBool Value);

			VBool					GetAutoAutoArrange();
			void					SetAutoAutoArrange(VBool Value);

			VBool					GetSnapToGrid();
			void					SetSnapToGrid(VBool Value);

			/**********************************************************
			Actions
			**********************************************************/

			void					EditLabel(VInt Index);
			void					CancelEditLabel();
			void					Arrange();
			void					SnapToGrid();
			void					EnsureItemVisible(VInt Index , VBool AcceptPartialVisible);
			void					Sort(VPointer Param=0);

			/**********************************************************
			Items
			**********************************************************/

			VInt					GetItemCount();
			void					AddItem(VUnicodeString Text , VInt ImageIndex=-1);
			void					InsertItem(VInt Index , VUnicodeString Text , VInt ImageIndex=-1);
			void					DeleteItem(VInt Index);
			void					Clear();
			VL_WinListViewItem		GetItem(VInt Index);
			VInt					GetSelectedItemCount();
			VInt					GetSelectionItemMark();
			void					SetSelectionItemMark(VInt Value);
			VInt					GetHotItemIndex();
			void					SetHotItemIndex(VInt Value);

			/**********************************************************
			Columns
			**********************************************************/

			VInt					GetColumnCount();
			void					AddColumn(VUnicodeString Text , VInt SubItemIndex=-1);
			void					InsertColumn(VInt Index , VUnicodeString Text , VInt SubItemIndex=-1);
			void					DeleteColumn(VInt Index);
			VL_WinListViewColumn	GetColumn(VInt Index);
			VInt					GetSelectedColumnIndex();
			void					SetSelectedColumnIndex(VInt Value);

			/**********************************************************
			Groups
			**********************************************************/

			void					EnableGrouping(VBool Value);
			VInt					GetGroupCount();
			VInt					AddGroup(VUnicodeString Text);
			VInt					InsertGroup(VInt Index , VUnicodeString Text);
			void					DeleteGroup(VInt Index);
			void					DeleteGroupByID(VInt ID);
			VL_WinListViewGroup		GetGroup(VInt Index);
			VL_WinListViewGroup		GetGroupByID(VInt ID);
		};
		
/*********************************************************************************************************
Tree View
*********************************************************************************************************/
		
		class VL_WinTreeViewItem : public VL_Base
		{
			friend class VL_WinTreeView;
		protected:
			HWND					FHandle;
			HTREEITEM				FItem;

			VL_WinTreeViewItem(HWND Handle , HTREEITEM Item);
		public:
			VL_WinTreeViewItem(const VL_WinTreeViewItem& Item);
			VL_WinTreeViewItem();
			VL_WinTreeViewItem& operator=(const VL_WinTreeViewItem& Item);
			operator VBool();

			VL_WinTreeViewItem		InsertAtLast(VUnicodeString Text , VInt ImageIndex=-1 , VInt SelectedImageIndex=-1);
			VL_WinTreeViewItem		InsertAtFirst(VUnicodeString Text , VInt ImageIndex=-1 , VInt SelectedImageIndex=-1);
			VL_WinTreeViewItem		InsertAfter(VUnicodeString Text , VInt ImageIndex=-1 , VInt SelectedImageIndex=-1);
			void					DeleteSelf();

			VL_WinTreeViewItem		GetParent();
			VL_WinTreeViewItem		GetFirstChild();
			VL_WinTreeViewItem		GetPrevious();
			VL_WinTreeViewItem		GetNext();
			VL_WinTreeViewItem		GetNextVisible();
			VL_WinTreeViewItem		GetPrevVisible();
			VL_WinTreeViewItem		GetNextSelected();

			HTREEITEM				GetHandle();
			void					SetDropTarget();
			void					SetFirstVisible();

			VUnicodeString			GetText();
			void					SetText(VUnicodeString Value);

			VInt					GetImageIndex();
			void					SetImageIndex(VInt Value);

			VInt					GetSelectedImageIndex();
			void					SetSelectedImageIndex(VInt Value);

			VPointer				GetCustomData();
			void					SetCustomData(VPointer Value);

			VBool					GetBoldState();
			void					SetBoldState(VBool Value);

			VBool					GetCutState();
			void					SetCutState(VBool Value);

			VBool					GetDropHilitedState();
			void					SetDropHilitedState(VBool Value);

			VBool					GetExpendedState();
			void					SetExpendedState(VBool Value);

			VBool					GetSelectedState();
			void					SetSelectedState(VBool Value);

			VBool					GetChecked();
			void					SetChecked(VBool Value);

			void					EnsureVisible();
			void					Expand();
			void					Collapse();
			void					Toggle();
		};

		VL_DEFINE_VOID_EVENT	(VE_TreeViewItemEvent	,(Sender , Item)					,(VL_Base* Sender , VL_WinTreeViewItem Item));
		VL_DEFINE_VOID_EVENT	(VE_TreeViewVerifyEvent	,(Sender , Item , Accept)			,(VL_Base* Sender , VL_WinTreeViewItem Item , VBool& Accept));
		VL_DEFINE_VOID_EVENT	(VE_TreeViewEditEvent	,(Sender , Item , Accept , Text)	,(VL_Base* Sender , VL_WinTreeViewItem Item , VBool& Accept , VUnicodeString Text));

		class VL_WinTreeView : public VL_WinControl
		{
		protected:
			VL_WinImageList*		FNormalImageList;
			VL_WinImageList*		FStateImageList;
		public:
			VL_WinTreeView(VL_WinContainer* Parent);
			~VL_WinTreeView();
			LRESULT					ProcessMessage(UINT Message , WPARAM& wParam , LPARAM& lParam , VBool& CallDefaultProcedure);

			VE_NotifyEvent			OnClick;
			VE_NotifyEvent			OnDbClick;
			VE_NotifyEvent			OnRightClick;
			VE_NotifyEvent			OnRightDbClick;
			VE_TreeViewItemEvent	OnBeginDrag;
			VE_TreeViewItemEvent	OnBeginRightDrag;
			VE_TreeViewEditEvent	OnBeginLabelEdit;
			VE_TreeViewEditEvent	OnEndLabelEdit;
			VE_TreeViewItemEvent	OnItemExpanding;
			VE_TreeViewItemEvent	OnItemExpanded;
			VE_TreeViewVerifyEvent	OnItemSelecting;
			VE_TreeViewItemEvent	OnItemSelected;

			VL_WinImageList*		GetNormalImageList();
			void					SetNormalImageList(VL_WinImageList* ImageList);

			VL_WinImageList*		GetStateImageList();
			void					SetStateImageList(VL_WinImageList* ImageList);

			COLORREF				GetLineColor();
			void					SetLineColor(COLORREF Value);

			COLORREF				GetTextColor();
			void					SetTextColor(COLORREF Value);

			COLORREF				GetBackColor();
			void					SetBackColor(COLORREF Value);

			/**********************************************************
			Styles
			**********************************************************/

			VBool					GetShowCheckBoxes();
			void					SetShowCheckBoxes(VBool Value);

			VBool					GetEnableDragDrop();
			void					SetEnableDragDrop(VBool Value);

			VBool					GetEditable();
			void					SetEditable(VBool Value);

			VBool					GetFullRowSelect();
			void					SetFullRowSelect(VBool Value);

			VBool					GetHasButtons();
			void					SetHasButtons(VBool Value);

			VBool					GetHasLines();
			void					SetHasLines(VBool Value);

			VBool					GetAlwaysShowSelection();
			void					SetAlwaysShowSelection(VBool Value);

			VBool					GetDimmedCheckBoxes();
			void					SetDimmedCheckBoxes(VBool Value);

			VBool					GetExclusionCheckBoxes();
			void					SetExclusionCheckBoxes(VBool Value);

			VBool					GetFadeButton();
			void					SetFadeButton(VBool Value);

			/**********************************************************
			Actions
			**********************************************************/

			void					EditLabel(VL_WinTreeViewItem Item);
			void					FinishEditLabel();
			void					CancelEditLabel();

			/**********************************************************
			Items
			**********************************************************/

			VL_WinTreeViewItem		AddRootItem(VUnicodeString Text , VInt ImageIndex=-1 , VInt SelectedImageIndex=-1);
			VL_WinTreeViewItem		GetFirstRoot();
			VL_WinTreeViewItem		GetCurrentSelectedItem();
			VL_WinTreeViewItem		GetFirstVisible();
			VL_WinTreeViewItem		GetLastVisible();
			VL_WinTreeViewItem		GetDropHighlighted();
			void					Clear();
		};

	}
}

#endif