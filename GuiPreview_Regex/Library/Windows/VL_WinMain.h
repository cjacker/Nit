/*******************************************************************************
Vczh Library++ 2.0
Windows界面::WinMain
开发者：陈梓瀚

接口：
类：
  VL_WinApplication					：程序框架
函数：
*******************************************************************************/

#ifndef VL_WINMAIN
#define VL_WINMAIN

#include "VL_WinClass.h"
#include "VL_WinCommDlg.h"
#include "..\Data\Data\VL_Data_Map.h"

namespace vl
{
	namespace windows
	{
		using namespace collection;

		class VL_WinApplication : public VL_Base
		{
			friend class VL_WinControl;
			friend class VL_WinForm;

			typedef VL_ListedMap<HWND,VL_WinControl*>	ControlMap;
			typedef VL_List<VL_WinForm* , true>			FormList;
		protected:
			HINSTANCE				FInstance;
			ControlMap				FControls;
			VL_WinForm*				FMainForm;
			FormList				FForms;

			VL_WinPen::Ptr			FPen;
			VL_WinBrush::Ptr		FBrush;
			VL_WinFont::Ptr			FFont;

			VBool					ProcessMessage(VBool InMessageLoop);
			VBool					RegisterForm(VL_WinForm* Form);
			void					UnregisterForm(VL_WinForm* Form);
			void					RunModal(VL_WinForm* Form);
		public:
			VL_WinApplication(HINSTANCE hInstance);

			HINSTANCE				GetInstance();
			VUnicodeString			GetAppName();
			void					Run();
			void					DoEvents();
			VL_WinControl*			GetControl(HWND Handle);
			void					Terminate();
			VL_WinForm*				GetMainForm();

			VL_WinPen::Ptr			GetDefaultPen();
			VL_WinBrush::Ptr		GetDefaultBrush();
			VL_WinFont::Ptr			GetDefaultFont();
		};

		extern VL_WinClass*			GetDefaultClass();
		extern VL_WinApplication*	GetApplication();
	}
}
extern LRESULT CALLBACK				SubclassProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,UINT_PTR uIdSubclass,DWORD_PTR dwRefData);
extern LRESULT CALLBACK				WindowProc(HWND hwnd , UINT uMsg , WPARAM wParam , LPARAM lParam);

#endif