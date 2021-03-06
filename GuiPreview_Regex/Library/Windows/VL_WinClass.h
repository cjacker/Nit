/*******************************************************************************
Vczh Library++ 2.0
Windows界面::WinClass
开发者：陈梓瀚

接口：
类：
  IVL_AcceleratorReceiver		：加速键接收器
  VL_AcceleratorItem			：加速键对象
  VL_AcceleratorManager			：加速键管理器

  VL_WinClass					：窗口类
  VL_WinControl					：控件基类
  VL_WinContainer				：控件容器
  VL_WinForm					：窗口

  VL_WinTimer					：定时器
  VL_WinImageList				：图像列表
	Draw的Style使用ILD_FOCUS , ILD_SELECTED , ILD_NORMAL
  VL_WinMenu					：菜单
  VL_WinFormMenu				：窗口菜单
  VL_WinPopupMenu				：弹出式菜单
  VL_WinMenuItem				：菜单项

  VL_Placement					：布局
    pBlank
    pControl
	pVertFix1
	pVertFix2
	pVertScale
	pHorzFix1
	pHorzFix2
	pHorzScale
函数：
  Accel							：获得加速键，从而可以将菜单项或其他加速键接收器绑定到某个加速键上
    参数：
	  SysKeys					：VLE_AcceleratorKey成员的组合
	  Key						：按键名，参见VL_WinClass.cpp中的InitAccelerator函数
宏：
  WM_USER_VLGUI					：私有消息起始号码
*******************************************************************************/

#ifndef VL_WINCLASS
#define VL_WINCLASS

#include "..\Data\Data\VL_Data_Basic.h"
#include "..\Data\Data\VL_Data_String.h"
#include "..\Data\Data\VL_Data_List.h"
#include "..\Data\Data\VL_Data_Map.h"
#include "..\Data\Data\VL_Data_Event.h"
#include "VL_WinGDI.h"

#include <windows.h>
#include <CommCtrl.h>

#define WM_GETPOINTER_DISPATCHED	(WM_USER+1)
#define WM_COMMAND_DISPATCHED		(WM_USER+2)
#define WM_NOTIFY_DISPATCHED		(WM_USER+3)
#define WM_VSCROLL_DISPATCHED		(WM_USER+4)
#define WM_HSCROLL_DISPATCHED		(WM_USER+5)

#define WM_USER_VLGUI				(WM_USER+6)

namespace vl
{
	namespace windows
	{
		using namespace collection;

		class VL_WinClass : public VL_Base
		{
		protected:
			VUnicodeString			FName;
			WNDCLASSEX				FWindowClass;
			ATOM					FClassAtom;
		public:
			VL_WinClass(VUnicodeString Name , VBool Shadow , VBool OwnDC , WNDPROC Procedure , HINSTANCE hInstance);

			VBool					IsAvailable();
			VUnicodeString			GetName();
			ATOM					GetClassAtom();
		};

/*********************************************************************************************************
公共枚举
*********************************************************************************************************/

		enum VLE_WinHAlign
		{
			vwhaLeft,
			vwhaCenter,
			vwhaRight
		};

		enum VLE_WinVAlign
		{
			vwvaTop,
			vwvaCenter,
			vwvaBottom
		};

/*********************************************************************************************************
事件
*********************************************************************************************************/

		struct VLS_MouseStruct
		{
			VBool	Ctrl;
			VBool	Shift;
			VBool	LeftButton;
			VBool	MiddleButton;
			VBool	RightButton;
			VInt	X;
			VInt	Y;
			VInt	Wheel;

			VLS_MouseStruct(WPARAM wParam , LPARAM lParam , VBool WheelMessage);
		};

		struct VLS_KeyStruct
		{
			VInt	KeyCode;		/*char事件为字符的Utf-16代码，其余为Virtual Key Code*/
			VInt	RepeatCount;
			VBool	PreviousDown;	/*前一个相同的按键事件按键是否处于按下状态，Down事件有效*/
			VBool	Alt;			/*Alt是否处于按下状态，Sys事件有效*/

			VLS_KeyStruct(WPARAM wParam , LPARAM lParam);
		};

		VL_DEFINE_VOID_EVENT	(VE_NotifyEvent		,(Sender)									,(VL_Base* Sender));
		VL_DEFINE_VOID_EVENT	(VE_MovingEvent		,(Sender,Area)								,(VL_Base* Sender,LPRECT Area));
		VL_DEFINE_VOID_EVENT	(VE_SizingEvent		,(Sender,Left,Right,Top,Bottom,Area)		,(VL_Base* Sender,VBool Left,VBool Right,VBool Top,VBool Bottom,LPRECT Area));
		VL_DEFINE_VOID_EVENT	(VE_QueryEvent		,(Sender,Cancel)							,(VL_Base* Sender,VBool& Cancel));
		VL_DEFINE_VOID_EVENT	(VE_MouseEvent		,(Sender,MouseStruct)						,(VL_Base* Sender,VLS_MouseStruct MouseStruct));
		VL_DEFINE_VOID_EVENT	(VE_KeyEvent		,(Sender,KeyStruct)							,(VL_Base* Sender,VLS_KeyStruct KeyStruct));
		VL_DEFINE_VOID_EVENT	(VE_CharEvent		,(Sender,Char)								,(VL_Base* Sender,VWChar& Char));
		VL_DEFINE_VOID_EVENT	(VE_HotKeyEvent		,(Sender,ID)								,(VL_Base* Sender,VInt ID));
		VL_DEFINE_VOID_EVENT	(VE_DropdownEvent	,(Sender,Rect)								,(VL_Base* Sender,RECT Rect));

/*********************************************************************************************************
加速键
*********************************************************************************************************/

		extern VInt					KeyCodeFromName(VUnicodeString Name);

		class IVL_AcceleratorReceiver : public IVL_Interface
		{
			friend class VL_AcceleratorItem;
		private:
			VL_AcceleratorItem*		FItem;
		protected:
			virtual void			OnAttachAcceleratorItem()=0;
			virtual void			OnDetachAcceleratorItem()=0;
			virtual void			OnExecuteAcceleratorItem()=0;
			VL_AcceleratorItem*		GetAcceleratorItem();
		public:
			IVL_AcceleratorReceiver();
			~IVL_AcceleratorReceiver();

			VBool					Bind(VL_AcceleratorItem* Item);
			void					Unbind();
		};

		class VL_AcceleratorItem : public VL_Base
		{
		protected:
			IVL_AcceleratorReceiver*	FReceiver;
			VUnicodeString				FName;
		public:
			VL_AcceleratorItem(VUnicodeString Name);
			~VL_AcceleratorItem();

			VBool						Bind(IVL_AcceleratorReceiver* Receiver);
			void						Unbind();
			void						Execute();
			VUnicodeString				GetName();
		};
		typedef VL_List<VL_AutoPtr<VL_AcceleratorItem> , false , VL_AcceleratorItem*>	VL_AcceleratorItemList;

		enum VLE_AcceleratorKey
		{
			vakNone=0,
			vakCtrl=1,
			vakShift=2,
			vakAlt=4
		};
		class VL_AcceleratorManager : public VL_Base
		{
		protected:
			VL_List<ACCEL , true>		FAcceleratorList;
			VL_AcceleratorItemList		FAcceleratorItems;
			HACCEL						FHandle;
		public:
			VL_AcceleratorManager();
			~VL_AcceleratorManager();

			HACCEL						GetHandle();
			VL_AcceleratorItem*			GetAcceleratorItem(VLE_AcceleratorKey SysKeys , VUnicodeString Key);
			VL_AcceleratorItem*			GetAcceleratorItem(VInt ID);
		};

		extern VL_AcceleratorManager*	GetAcceleratorManager();
		extern VL_AcceleratorItem*		Accel(VLE_AcceleratorKey SysKeys , VUnicodeString Key);

/*********************************************************************************************************
布局
*********************************************************************************************************/

		class VL_WinControl;
		class VL_WinContainer;

		enum VLE_PlacementDirection
		{
			vpdVertical,
			vpdHorizontal
		};
		enum VLE_PlacementBehavior
		{
			vpbFixPlacement1,
			vpbFixPlacement2,
			vpbFixScale
		};
		class VL_Placement : public VL_Base
		{
			friend class VL_WinContainer;
		protected:
			VL_WinContainer*		FContainer;
			VL_Placement*			FParent;
			VL_WinControl*			FControl;
			VL_Placement*			FPlacement1;
			VL_Placement*			FPlacement2;
			VInt					FSpliterPosition;
			VLE_PlacementDirection	FSpliterDirection;
			VInt					FSpliterSize;
			VDouble					FSpliterScale;
			VLE_PlacementBehavior	FBehavior;
			VInt					FBorderSize;
			VInt					FMinClientWidth;
			VInt					FMinClientHeight;
			VBool					FDisableResize;

			void					Initialize();
			VL_Placement(VL_Placement* Parent);
			VL_Placement(VL_WinContainer* Container);
			~VL_Placement();

			void					OnSizing(VL_Base* Sender , VBool Left , VBool Right , VBool Top , VBool Bottom , LPRECT Area);
			void					OnSized(VL_Base* Sender);
			void					ModifySize(RECT Area);
			void					Resize();
			void					UnregisterControl(VL_WinControl* Control);
		public:

			VInt					GetSpliterPosition();
			void					SetSpliterPosition(VInt Position);

			VLE_PlacementDirection	GetSpliterDirection();
			void					SetSpliterDirection(VLE_PlacementDirection Direction);

			VLE_PlacementBehavior	GetBehavior();
			void					SetBehavior(VLE_PlacementBehavior Behavior);

			VInt					GetBorderSize();
			void					SetBorderSize(VInt Value);

			VInt					GetSpliterSize();
			void					SetSpliterSize(VInt Size);

			VDouble					GetSpliterScale();
			void					SetSpliterScale(VDouble Scale);

			VL_WinControl*			GetControl();
			void					SetControl(VL_WinControl* Control);

			VInt					GetMinClientWidth();
			void					SetMinClientWidth(VInt Value);

			VInt					GetMinClientHeight();
			void					SetMinClientHeight(VInt Value);

			VL_Placement*			GetPlacement1();
			VL_Placement*			GetPlacement2();
			RECT					GetArea();
			RECT					GetMinSize();

			VInt					GetPlacement1Position();
			void					SetPlacement1Position(VInt Value);

			VInt					GetPlacement2Position();
			void					SetPlacement2Position(VInt Value);
		};

		namespace placement
		{
			class pBase : IVL_Interface
			{
			public:
				typedef VL_AutoPtr<pBase>		Ptr;

				virtual void		Apply(VL_Placement* Placement)=0;
			};

			class pBlank
			{
			protected:
				VInt				FMinWidth;
				VInt				FMinHeight;
			public:
				pBlank(VInt MinWidth , VInt MinHeight);

				operator pBase::Ptr();
			};

			class pControl
			{
			protected:
				VL_WinControl*		FControl;
				VInt				FBorder;
				VInt				FMinWidth;
				VInt				FMinHeight;
			public:
				pControl(VL_WinControl* Control);
				pControl(VL_WinControl* Control , VInt Border);
				pControl(VL_WinControl* Control , VInt MinWidth , VInt MinHeight);
				pControl(VL_WinControl* Control , VInt Border , VInt MinWidth , VInt MinHeight);

				operator pBase::Ptr();
			};

			class pVertFix1
			{
			protected:
				VInt				FBorder;
				VInt				FSpliterSize;
				pBase::Ptr			FPlacement1;
				pBase::Ptr			FPlacement2;
			public:
				pVertFix1(VInt Border , VInt SpliterSize , pBase::Ptr Placement1 , pBase::Ptr Placement2);

				operator pBase::Ptr();
			};

			class pVertFix2
			{
			protected:
				VInt				FBorder;
				VInt				FSpliterSize;
				pBase::Ptr			FPlacement1;
				pBase::Ptr			FPlacement2;
			public:
				pVertFix2(VInt Border , VInt SpliterSize , pBase::Ptr Placement1 , pBase::Ptr Placement2);

				operator pBase::Ptr();
			};

			class pVertScale
			{
			protected:
				VInt				FBorder;
				VInt				FSpliterSize;
				VDouble				FScale;
				pBase::Ptr			FPlacement1;
				pBase::Ptr			FPlacement2;
			public:
				pVertScale(VInt Border , VInt SpliterSize , VDouble Scale , pBase::Ptr Placement1 , pBase::Ptr Placement2);

				operator pBase::Ptr();
			};

			class pHorzFix1
			{
			protected:
				VInt				FBorder;
				VInt				FSpliterSize;
				pBase::Ptr			FPlacement1;
				pBase::Ptr			FPlacement2;
			public:
				pHorzFix1(VInt Border , VInt SpliterSize , pBase::Ptr Placement1 , pBase::Ptr Placement2);

				operator pBase::Ptr();
			};

			class pHorzFix2
			{
			protected:
				VInt				FBorder;
				VInt				FSpliterSize;
				pBase::Ptr			FPlacement1;
				pBase::Ptr			FPlacement2;
			public:
				pHorzFix2(VInt Border , VInt SpliterSize , pBase::Ptr Placement1 , pBase::Ptr Placement2);

				operator pBase::Ptr();
			};

			class pHorzScale
			{
			protected:
				VInt				FBorder;
				VInt				FSpliterSize;
				VDouble				FScale;
				pBase::Ptr			FPlacement1;
				pBase::Ptr			FPlacement2;
			public:
				pHorzScale(VInt Border , VInt SpliterSize , VDouble Scale , pBase::Ptr Placement1 , pBase::Ptr Placement2);

				operator pBase::Ptr();
			};
		}

/*********************************************************************************************************
基础类
*********************************************************************************************************/

		class VL_WinControl : public VL_Base
		{
		protected:
			HWND					FHandle;
			VBool					FMouseEntered;
			VBool					FEnableHover;
			VBool					FHoverOnce;
			VInt					FHoverTime;
			VInt					FLastX;
			VInt					FLastY;
			VL_WinContainer*		FParent;
			VL_WinFont::Ptr			FFont;
			VBool					FSubClassed;

			VBool					_CreateWindow(DWORD ExStyle , DWORD Style , PWChar Class , VL_WinContainer* Parent);

			virtual DWORD			InternalGetExStyle();
			virtual void			InternalSetExStyle(DWORD ExStyle);
			VBool					GetExStyle(DWORD ExStyle);
			void					SetExStyle(DWORD ExStyle , VBool Available);
			VBool					GetStyle(DWORD Style);
			void					SetStyle(DWORD Style , VBool Available);
			void					TrackMouse(VBool Enable , VInt HoverTime);
			void					Destroy();
			void					GetWindowPosSize(VInt& l , VInt& t , VInt& w , VInt & h);
			void					SetWindowPosSize(VInt& l , VInt& t , VInt& w , VInt & h);
		public:
			VE_NotifyEvent			OnEnabled;
			VE_NotifyEvent			OnDisabled;
			VE_NotifyEvent			OnMoved;
			VE_MovingEvent			OnMoving;
			VE_NotifyEvent			OnShow;
			VE_NotifyEvent			OnHide;
			VE_NotifyEvent			OnSized;
			VE_SizingEvent			OnSizing;
			VE_NotifyEvent			OnGotFocus;
			VE_NotifyEvent			OnLostFocus;
			VE_NotifyEvent			OnPaint;

			VE_MouseEvent			OnMouseMove;
			VE_MouseEvent			OnLeftButtonDown;
			VE_MouseEvent			OnLeftButtonUp;
			VE_MouseEvent			OnLeftButtonDbClick;
			VE_MouseEvent			OnRightButtonDown;
			VE_MouseEvent			OnRightButtonUp;
			VE_MouseEvent			OnRightButtonDbClick;
			VE_MouseEvent			OnMiddleButtonDown;
			VE_MouseEvent			OnMiddleButtonUp;
			VE_MouseEvent			OnMiddleButtonDbClick;
			VE_MouseEvent			OnMouseHWheel;
			VE_MouseEvent			OnMouseVWheel;
			VE_MouseEvent			OnMouseHover;
			VE_NotifyEvent			OnMouseEnter;
			VE_NotifyEvent			OnMouseLeave;

			VE_KeyEvent				OnKeyDown;
			VE_KeyEvent				OnKeyUp;
			VE_KeyEvent				OnSysKeyDown;
			VE_KeyEvent				OnSysKeyUp;
			VE_CharEvent			OnChar;

			VL_WinControl();
			~VL_WinControl();

			virtual LRESULT			ProcessMessage(UINT Message , WPARAM& wParam , LPARAM& lParam , VBool& CallDefaultProcedure);

			HWND					GetHandle();

			VL_WinContainer*		GetParent();
			void					SetParent(VL_WinContainer* Parent);

			VBool					GetEnableHover();
			void					SetEnableHover(VBool Value);

			VBool					GetHoverOnce();
			void					SetHoverOnce(VBool Value);

			VInt					GetHoverTime();
			void					SetHoverTime(VInt Value);

			VInt					GetLeft();
			void					SetLeft(VInt Value);

			VInt					GetTop();
			void					SetTop(VInt Value);

			VInt					GetWidth();
			void					SetWidth(VInt Value);

			VInt					GetHeight();
			void					SetHeight(VInt Value);

			void					Move(VInt Left , VInt Top , VInt Width , VInt Height);

			VUnicodeString			GetText();
			void					SetText(VUnicodeString Value);

			VBool					GetEnabled();
			void					SetEnabled(VBool Value);

			VBool					GetVisible();
			void					SetVisible(VBool Value);

			VBool					GetFocused();
			void					SetFocused();

			VBool					GetAcceptFiles();
			void					SetAcceptFiles(VBool Value);

			VL_WinFont::Ptr			GetFont();
			void					SetFont(VL_WinFont::Ptr Font);
		};
		typedef VL_List<VL_WinControl* , true>			VL_WinControlList;

		class VL_WinContainer : public VL_WinControl
		{
			friend class VL_WinControl;
		protected:
			VL_WinControlList		FControls;
			VL_Placement*			FPlacement;

			void					RegisterChild(VL_WinControl* Control);
			void					UnregisterChild(VL_WinControl* Control);
			void					Destroy();
			virtual void			GetClientArea(RECT* Rect);
		public:
			VL_WinContainer();
			~VL_WinContainer();

			VL_Placement*			GetPlacement();
			void					ApplyPlacement(placement::pBase::Ptr Placement);

			VInt					GetControlCount();
			VL_WinControl*			GetControl(VInt Index);

			VBool					GetClipChildren();
			void					SetClipChildren(VBool Value);

			VBool					GetClipSiblings();
			void					SetClipSiblings(VBool Value);

			VInt					GetClientWidth();
			void					SetClientWidth(VInt Value);

			VInt					GetClientHeight();
			void					SetClientHeight(VInt Value);
		};

		class VL_WinTimer;
		class VL_WinMenu;
		class VL_WinMenuItem;
		class VL_WinFormMenu;

		typedef VL_List<VL_WinTimer* , true>			VL_WinTimerList;
		typedef VL_List<VL_WinMenu* , true>				VL_WinMenuList;
		typedef VL_ListedMap<VInt , VL_WinMenuItem*>	VL_WinMenuItemMap;

		enum VLE_WinFormBorder
		{
			vwfbNone,
			vwfbBorder,
			vwfbSingle,
			vwfbSizable
		};
		class VL_WinForm : public VL_WinContainer
		{
			friend VL_WinTimer;
			friend VL_WinMenu;
			friend VL_WinMenuItem;
		protected:
			VBool					FIsMainForm;
			VBool					FCreated;

			VL_List<VInt , true>	FHotKeys;
			VL_WinTimerList			FTimerList;
			VL_WinMenuList			FMenuList;
			VL_WinFormMenu*			FFormMenu;
			VL_WinMenuItemMap		FMenuItemMap;
			VInt					FUsedMenuItemHandle;
			VL_List<VInt , true>	FFreeMenuItemHandles;

			VInt					RegisterOwned(VL_WinTimer* Timer);
			void					UnregisterOwned(VL_WinTimer* Timer);
			void					RegisterOwned(VL_WinMenu* Menu);
			void					UnregisterOwned(VL_WinMenu* Menu);

			VInt					GenerateMenuItemHandle();
			void					RegisterMenuItem(VInt Handle , VL_WinMenuItem* Item);
			void					UnregisterMenuItem(VInt Handle);

			virtual void			InitComponents();

			void					CreateForm();
		public:
			VE_NotifyEvent			OnActivateApp;
			VE_NotifyEvent			OnDeactivateApp;
			VE_NotifyEvent			OnActivate;
			VE_NotifyEvent			OnDeactivate;
			VE_QueryEvent			OnQueryClose;
			VE_NotifyEvent			OnClose;
			VE_NotifyEvent			OnCompacting;
			VE_NotifyEvent			OnCreate;
			VE_NotifyEvent			OnDestroy;
			VE_NotifyEvent			OnEnterSizeMove;
			VE_NotifyEvent			OnLeaveSizeMove;
			VE_HotKeyEvent			OnHotKey;

			VL_WinForm(VBool Create);
			~VL_WinForm();

			LRESULT					ProcessMessage(UINT Message , WPARAM& wParam , LPARAM& lParam , VBool& CallDefaultProcedure);
			VBool					RegisterGlobalHotKey(VInt ID , VBool Alt , VBool Ctrl , VBool Shift , VBool Win , VInt KeyCode);
			void					UnregisterGlobalHotKey(VInt ID);
			VL_WinMenuItem*			MenuItemOfHandle(VInt Handle);

			VLE_WinFormBorder		GetBorder();
			void					SetBorder(VLE_WinFormBorder Value);

			VBool					GetMaximizeBox();
			void					SetMaximizeBox(VBool Value);

			VBool					GetMinimizeBox();
			void					SetMinimizeBox(VBool Value);

			VBool					GetIconAvailable();
			void					SetIconAvailable(VBool Value);

			VBool					GetAppearInTaskBar();
			void					SetAppearInTaskBar(VBool Value);

			VBool					GetTopMost();
			void					SetTopMost(VBool Value);

			VBool					GetTransparent();
			void					SetTransparent(VBool Value);

			VBool					GetWindowEdge();
			void					SetWindowEdge(VBool Value);

			VBool					GetToolWindow();
			void					SetToolWindow(VBool Value);

			VBool					GetActived();
			void					SetActived();

			void					Show();
			void					ShowRestored();
			void					ShowMinimized();
			void					ShowMaximized();
			void					ShowModal();
			void					MoveCenter();
			void					Close();
			void					DestroyForm();

			VL_WinFormMenu*			GetFormMenu();
			void					SetFormMenu(VL_WinFormMenu* Value);
		};

/*********************************************************************************************************
定时器
*********************************************************************************************************/

		class VL_WinTimer : public VL_Base
		{
		protected:
			UINT_PTR				FHandle;
			VInt					FInterval;
			VBool					FEnabled;
			VL_WinForm*				FOwner;
		public:
			VE_NotifyEvent			OnTimer;

			VL_WinTimer(VL_WinForm* Owner);
			~VL_WinTimer();

			UINT_PTR				GetHandle();

			VL_WinForm*				GetOwner();

			VInt					GetInterval();
			void					SetInterval(VInt Value);

			VBool					GetEnabled();
			void					SetEnabled(VBool Value);
		};

/*********************************************************************************************************
图像列表
*********************************************************************************************************/

		class VL_WinImageList : public VL_Base
		{
		protected:
			HIMAGELIST				FHandle;

			VL_WinBitmap::Ptr		CheckBitmap(VL_WinBitmap::Ptr Bitmap);
			VL_WinBitmap::Ptr		CreateMask(VL_WinBitmap::Ptr Bitmap);
		public:
			VL_WinImageList(VInt Width , VInt Height);
			VL_WinImageList(VL_WinImageList* ImageList);
			~VL_WinImageList();

			HIMAGELIST				GetHandle();
			void					Add(VL_WinBitmap::Ptr Bitmap);
			void					Replace(VInt Index , VL_WinBitmap::Ptr Bitmap);
			void					Remove(VInt Index);
			VInt					GetCount();
			VInt					GetImageWidth();
			VInt					GetImageHeight();
			COLORREF				GetBackColor();
			void					SetBackColor(COLORREF Color);
			VBool					GetUseBackColor();
			void					SetUseBackColor(VBool Use);
			void					Draw(VL_WinDC* DC , VInt Index , VInt X , VInt Y , UINT Style);
		};

/*********************************************************************************************************
菜单
*********************************************************************************************************/

		class VL_WinPopupMenu;
		typedef VL_List<VL_WinMenuItem* , true>		VL_WinMenuItemList;

		class VL_WinMenu : public VL_Base
		{
			friend class VL_WinMenuItem;
		protected:
			HMENU					FHandle;
			VL_WinForm*				FOwner;
			VL_WinForm*				FMessageDispatcher;
			VBool					FAssociated;
			VL_WinMenuItemList		FMenuItems;
			VL_WinMenuItem*			FAssociatedMenuItem;

			virtual void			ItemChanged();
		public:
			VL_WinMenu(VL_WinForm* Owner);
			~VL_WinMenu();

			HMENU					GetHandle();
			VInt					GetCount();
			VL_WinMenuItem*			GetItem(VInt Index);
			VBool					GetAssociated();
			VL_WinMenuItem*			GetAssociatedMenuItem();

			VL_WinMenuItem*			Append();
			VL_WinMenuItem*			Insert(VInt Index);
			void					Delete(VInt Index);
			void					Delete(VL_WinMenuItem* MenuItem);
		};

		class VL_WinMenuItem : public VL_Base , public IVL_AcceleratorReceiver
		{
			friend VL_WinMenu;
		protected:
			VL_WinMenu*				FOwner;
			VInt					FHandle;
			VL_WinPopupMenu*		FSubMenu;
			VL_WinBitmap::Ptr		FCheckedBitmap;
			VL_WinBitmap::Ptr		FUncheckedBitmap;

			VBool					FChecked;
			VBool					FSeparator;
			VBool					FRadioCheck;
			VInt					FRadioGroup;
			VBool					FEnabled;
			VBool					FHighlighted;
			VUnicodeString			FText;
			VUnicodeString			FMenuText;

			void					OnAttachAcceleratorItem();
			void					OnDetachAcceleratorItem();
			void					OnExecuteAcceleratorItem();
			void					FillStruct(MENUITEMINFO* Info);
			void					RefreshProperties();
			VL_WinBitmap::Ptr		CheckBitmap(VL_WinBitmap::Ptr Bitmap);
		public:
			VE_NotifyEvent			OnClick;
			VE_NotifyEvent			OnRightClick;

			VL_WinMenuItem(VL_WinMenu* Owner , VInt Handle);
			~VL_WinMenuItem();

			VL_WinMenu*				GetOwner();

			VBool					GetChecked();
			void					SetChecked(VBool Value);

			VBool					GetSeparator();
			void					SetSeparator(VBool Value);

			VBool					GetRadioCheck();
			void					SetRadioCheck(VBool Value);

			VInt					GetRadioGroup();
			void					SetRadioGroup(VInt Value);

			VBool					GetEnabled();
			void					SetEnabled(VBool Value);

			VBool					GetHighlighted();
			void					SetHighlighted(VBool Value);

			VUnicodeString			GetText();
			void					SetText(VUnicodeString Text);

			VL_WinBitmap::Ptr		GetCheckedBitmap();
			void					SetCheckedBitmap(VL_WinBitmap::Ptr Bitmap);

			VL_WinBitmap::Ptr		GetUncheckedBitmap();
			void					SetUncheckedBitmap(VL_WinBitmap::Ptr Bitmap);

			VL_WinPopupMenu*		GetSubMenu();
			VL_WinPopupMenu*		CreateSubMenu();
			void					DestroySubMenu();
		};


		class VL_WinFormMenu : public VL_WinMenu
		{
			friend class VL_WinForm;
		protected:
			VL_WinForm*				FParent;

			void					ItemChanged();
		public:
			VL_WinFormMenu(VL_WinForm* Owner);
			~VL_WinFormMenu();
		};


		class VL_WinPopupMenu : public VL_WinMenu
		{
		protected:
		public:
			VL_WinPopupMenu(VL_WinForm* Owner);
			~VL_WinPopupMenu();

			VL_WinMenuItem*			Popup(VInt X , VInt Y , VBool KeepPrevMenu=false);
			VL_WinMenuItem*			Popup(VBool KeepPrevMenu=false);
		};

/*********************************************************************************************************
工具函数
*********************************************************************************************************/

		enum VLE_MsgboxButton
		{
			vmbAbortRetryIgnore,
			vmbOK,
			vmbOKCancel,
			vmbRetryCancel,
			vmbYesNo,
			vmbYesNoCancel
		};
		enum VLE_MsgboxIcon
		{
			vmiNone,
			vmiExclamation,
			vmiInformation,
			vmiQuestion,
			vmiError
		};
		enum VLE_MsgboxDefaultButton
		{
			vmdbButton1,
			vmdbButton2,
			vmdbButton3,
			vmdbButton4
		};
		enum VLE_MsgboxResult
		{
			vmrAbort,
			vmrRetry,
			vmrIgnore,
			vmrOK,
			vmrCancel,
			vmrYes,
			vmrNo
		};
		extern VLE_MsgboxResult VL_WinMsgbox(
			VL_WinForm* Caller ,
			VUnicodeString Text , 
			VUnicodeString Caption , 
			VLE_MsgboxButton Button , 
			VLE_MsgboxIcon Icon=vmiNone , 
			VLE_MsgboxDefaultButton DefaultButton=vmdbButton1
			);
		extern void ShowMessage(VL_WinForm* Caller , VUnicodeString Message);

/*********************************************************************************************************
通用类型
*********************************************************************************************************/

		enum VLE_Align
		{
			vaLeftTop,
			vaCenter,
			vaRightBottom
		};
	}
}

#endif