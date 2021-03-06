#include "VL_WinMain.h"

namespace vl
{
	namespace windows
	{

	UINT_PTR CALLBACK Service_CCHookProc(HWND hdlg , UINT uiMsg , WPARAM wParam , LPARAM lParam);
	UINT_PTR CALLBACK Service_PFNHookProc(HWND hdlg , UINT uiMsg , WPARAM wParam , LPARAM lParam);
	UINT_PTR CALLBACK Service_CFHookProc(HWND hdlg , UINT uiMsg , WPARAM wParam , LPARAM lParam);

/*********************************************************************************************************
事件
*********************************************************************************************************/

/*********************************************************************************************************
VL_WinColorDialog
*********************************************************************************************************/

	VL_WinColorDialog::VL_WinColorDialog()
	{
		FHandle=0;
		FOpening=false;

		Color=RGB(0,0,0);
		for(VInt i=0;i<sizeof(CustomColors)/sizeof(COLORREF);i++)
		{
			CustomColors[i]=RGB(0,0,0);
		}
	}

	VBool VL_WinColorDialog::IsOpening()
	{
		return FOpening;
	}

	VBool VL_WinColorDialog::Execute(VL_WinForm* Form)
	{
		if(FOpening)
		{
			return false;
		}
		else
		{
			FOpening=true;

			CHOOSECOLOR cc;
			cc.lStructSize=sizeof(cc);
			cc.hwndOwner=Form->GetHandle();
			cc.hInstance=NULL;
			cc.rgbResult=Color;
			cc.lpCustColors=CustomColors;
			cc.Flags=CC_ANYCOLOR | CC_ENABLEHOOK;
			cc.lCustData=(LPARAM)this;
			cc.lpfnHook=Service_CCHookProc;
			cc.lpTemplateName=NULL;

			VBool OK=ChooseColor(&cc)!=0;
			Color=cc.rgbResult;
			GetCommdlgService()->UnregisterColorDialog(FHandle);
			FOpening=false;
			FHandle=0;

			return OK;
		}
	}

/*********************************************************************************************************
VL_WinFileDialog
*********************************************************************************************************/

	void VL_WinFileDialog::Initialize()
	{
		FFilterBuffer=new VWChar[FFilter.Length()+2];
		{
			FFilterBuffer[FFilter.Length()+1]=L'\0';
			PWChar Buffer=FFilter.Buffer();
			VInt Length=FFilter.Length();
			for(VInt i=0;i<=Length;i++)
			{
				if(Buffer[i]==L'|')
				{
					FFilterBuffer[i]=L'\0';
				}
				else
				{
					FFilterBuffer[i]=Buffer[i];
				}
			}
		}

		FFileBuffer=new VWChar[65536];
		{
			PWChar Buffer=FFileBuffer;
			for(VInt i=0;i<FileNames.GetCount();i++)
			{
				VUnicodeString File=FileNames[i];
				wcscpy(Buffer,File.Buffer());
				Buffer+=File.Length()+1;
			}
			*Buffer=L'\0';
		}

		FTitleBuffer=new VWChar[FTitle.Length()+1];
		wcscpy(FTitleBuffer,FTitle.Buffer());

		FInitDirBuffer=new VWChar[FInitDir.Length()+1];
		wcscpy(FInitDirBuffer,FInitDir.Buffer());

		FDefExtBuffer=new VWChar[FDefaultExt.Length()+1];
		wcscpy(FDefExtBuffer,FDefaultExt.Buffer());

		FHandle=0;
		FOpening=true;
	}

	void VL_WinFileDialog::Finalize()
	{
		FHandle=0;
		FOpening=false;

		delete[] FFilterBuffer;
		delete[] FFileBuffer;
		delete[] FTitleBuffer;
		delete[] FInitDirBuffer;
		delete[] FDefExtBuffer;
	}

	void VL_WinFileDialog::FillStruct(OPENFILENAME& Struct , DWORD ExtraFlag)
	{
		Struct.lStructSize=sizeof(Struct);
		Struct.hwndOwner=NULL;
		Struct.hInstance=NULL;
		Struct.lpstrFilter=FFilterBuffer;
		Struct.lpstrCustomFilter=NULL;
		Struct.nMaxCustFilter=0;
		Struct.nFilterIndex=1;
		Struct.lpstrFile=FFileBuffer;
		Struct.nMaxFile=65536;
		Struct.lpstrFileTitle=NULL;
		Struct.nMaxFileTitle=0;
		Struct.lpstrInitialDir=FInitDirBuffer;
		Struct.lpstrTitle=FTitleBuffer;
		Struct.nFileOffset=0;
		Struct.nFileExtension=0;
		Struct.lpstrDefExt=FDefExtBuffer;
		Struct.lCustData=(LPARAM)(this);
		Struct.lpfnHook=Service_PFNHookProc;
		Struct.lpTemplateName=NULL;
		Struct.pvReserved=NULL;
		Struct.dwReserved=0;
		Struct.FlagsEx=0;

		Struct.Flags=OFN_ENABLEHOOK | OFN_ENABLEINCLUDENOTIFY | OFN_EXPLORER | ExtraFlag;
		if(FMultiSelection)		Struct.Flags|=OFN_ALLOWMULTISELECT;
		if(!FAddToRecent)		Struct.Flags|=OFN_DONTADDTORECENT;
		if(FEnableSizing)		Struct.Flags|=OFN_ENABLESIZING;
		if(FShowHidden)			Struct.Flags|=OFN_FORCESHOWHIDDEN;
		if(!FDereferenceLink)	Struct.Flags|=OFN_NODEREFERENCELINKS;
		if(!FNetworkButton)		Struct.Flags|=OFN_NONETWORKBUTTON;
	}

	void VL_WinFileDialog::FillResult(OPENFILENAME& Struct)
	{
		if(Struct.Flags & OFN_ALLOWMULTISELECT)
		{
			FileNames.Clear();
			PWChar Buffer=Struct.lpstrFile;
			while(*Buffer)
			{
				VInt Length=(VInt)wcslen(Buffer);
				FileNames.Add(Buffer);
				Buffer+=Length+1;
			}
		}
		else
		{
			FileNames.Clear();
			FileNames.Add(Struct.lpstrFile);
		}
	}

	VL_WinFileDialog::VL_WinFileDialog()
	{
		FHandle=0;
		FOpening=false;

		FMultiSelection=false;
		FAddToRecent=false;
		FEnableSizing=true;
		FShowHidden=false;
		FDereferenceLink=true;
		FNetworkButton=true;
		FFilter=L"所有文件(*.*)|*.*";
	}

	VL_WinFileDialog::~VL_WinFileDialog()
	{
	}

	VBool VL_WinFileDialog::GetMultiSelection()
	{
		return FMultiSelection;
	}

	void VL_WinFileDialog::SetMultiSelection(VBool Value)
	{
		FMultiSelection=true;
	}

	VBool VL_WinFileDialog::GetAddToRecent()
	{
		return FAddToRecent;
	}

	void VL_WinFileDialog::SetAddToRecent(VBool Value)
	{
		FAddToRecent=Value;
	}

	VBool VL_WinFileDialog::GetEnableSizing()
	{
		return FEnableSizing;
	}

	void VL_WinFileDialog::SetEnableSizing(VBool Value)
	{
		FEnableSizing=Value;
	}

	VBool VL_WinFileDialog::GetShowHidden()
	{
		return FShowHidden;
	}

	void VL_WinFileDialog::SetShowHidden(VBool Value)
	{
		FShowHidden=Value;
	}

	VBool VL_WinFileDialog::GetDereferenceLink()
	{
		return FDereferenceLink;
	}

	void VL_WinFileDialog::SetDereferenceLink(VBool Value)
	{
		FDereferenceLink=Value;
	}

	VBool VL_WinFileDialog::GetNetworkButton()
	{
		return FNetworkButton;
	}

	void VL_WinFileDialog::SetNetworkButton(VBool Value)
	{
		FNetworkButton=Value;
	}

	VUnicodeString VL_WinFileDialog::GetTitle()
	{
		return FTitle;
	}

	void VL_WinFileDialog::SetTitle(VUnicodeString Value)
	{
		FTitle=Value;
	}

	VUnicodeString VL_WinFileDialog::GetInitDir()
	{
		return FInitDir;
	}

	void VL_WinFileDialog::SetInitDir(VUnicodeString Value)
	{
		FInitDir=Value;
	}

	VUnicodeString VL_WinFileDialog::GetFilter()
	{
		return FFilter;
	}

	void VL_WinFileDialog::SetFilter(VUnicodeString Value)
	{
		FFilter=Value;
	}

	VUnicodeString VL_WinFileDialog::GetDefaultExt()
	{
		return FDefaultExt;
	}

	void VL_WinFileDialog::SetDefaultExt(VUnicodeString Value)
	{
		FDefaultExt=Value;
	}

/*********************************************************************************************************
VL_WinOpenFileDialog
*********************************************************************************************************/

	void VL_WinOpenFileDialog::FillResult(OPENFILENAME& Struct)
	{
		VL_WinFileDialog::FillResult(Struct);
		FReadonly=(Struct.Flags & OFN_READONLY)!=0;;
	}

	VL_WinOpenFileDialog::VL_WinOpenFileDialog()
	{
		FTitle=L"打开";
		FHideReadonly=false;
		FReadonly=false;
		FFileMustExist=true;
	}

	VL_WinOpenFileDialog::~VL_WinOpenFileDialog()
	{
	}

	VBool VL_WinOpenFileDialog::GetHideReadonly()
	{
		return FHideReadonly;
	}

	void VL_WinOpenFileDialog::SetHideReadonly(VBool Value)
	{
		FHideReadonly=Value;
	}

	VBool VL_WinOpenFileDialog::GetReadonly()
	{
		return FReadonly;
	}

	void VL_WinOpenFileDialog::SetReadonly(VBool Value)
	{
		FReadonly=Value;
	}

	VBool VL_WinOpenFileDialog::GetFileMustExist()
	{
		return FFileMustExist;
	}

	void VL_WinOpenFileDialog::SetFileMustExist(VBool Value)
	{
		FFileMustExist=Value;
	}
	
	VBool VL_WinOpenFileDialog::IsOpening()
	{
		return FOpening;
	}

	VBool VL_WinOpenFileDialog::Execute(VL_WinForm* Form)
	{
		if(FOpening)
		{
			return false;
		}
		else
		{
			DWORD Flags=0;
			if(FHideReadonly)		Flags|=OFN_HIDEREADONLY;
			if(FReadonly)			Flags|=OFN_READONLY;
			if(FFileMustExist)		Flags|=OFN_FILEMUSTEXIST;
			Initialize();

			OPENFILENAME Struct;
			FillStruct(Struct,Flags);
			Struct.hwndOwner=Form->GetHandle();
			VBool Result=GetOpenFileName(&Struct)!=0;
			GetCommdlgService()->UnregisterFileDialog(FHandle);

			Finalize();
			return Result;
		}
	}

/*********************************************************************************************************
VL_WinSaveFileDialog
*********************************************************************************************************/

	VL_WinSaveFileDialog::VL_WinSaveFileDialog()
	{
		FTitle=L"保存";
		FOverridePrompt=true;
	}

	VL_WinSaveFileDialog::~VL_WinSaveFileDialog()
	{
	}

	VBool VL_WinSaveFileDialog::GetOverridePrompt()
	{
		return FOverridePrompt;
	}

	void VL_WinSaveFileDialog::SetOverridePrompt(VBool Value)
	{
		FOverridePrompt=Value;
	}
	
	VBool VL_WinSaveFileDialog::IsOpening()
	{
		return FOpening;
	}

	VBool VL_WinSaveFileDialog::Execute(VL_WinForm* Form)
	{
		if(FOpening)
		{
			return false;
		}
		else
		{
			DWORD Flags=0;
			if(FOverridePrompt)		Flags|=OFN_OVERWRITEPROMPT;
			Initialize();

			OPENFILENAME Struct;
			FillStruct(Struct,Flags);
			Struct.hwndOwner=Form->GetHandle();
			VBool Result=GetSaveFileName(&Struct)!=0;
			GetCommdlgService()->UnregisterFileDialog(FHandle);

			Finalize();
			return Result;
		}
	}

/*********************************************************************************************************
VL_WinFontDialog
*********************************************************************************************************/

	VL_WinFontDialog::VL_WinFontDialog()
	{
		FHandle=0;
		FOpening=false;

		FTrueTypeOnly=false;
		FExtended=true;
		FFontMustExist=true;

		Font=GetApplication()->GetDefaultFont();
		FFontInfo=*Font->GetInfo();
		Color=RGB(0,0,0);
	}

	VL_WinFontDialog::~VL_WinFontDialog()
	{
	}

	VBool VL_WinFontDialog::GetTrueTypeOnly()
	{
		return FTrueTypeOnly;
	}

	void VL_WinFontDialog::SetTrueTypeOnly(VBool Value)
	{
		FTrueTypeOnly=Value;
	}

	VBool VL_WinFontDialog::GetExtended()
	{
		return FExtended;
	}

	void VL_WinFontDialog::SetExtended(VBool Value)
	{
		FExtended=Value;
	}

	VBool VL_WinFontDialog::GetFontMustExist()
	{
		return FFontMustExist;
	}

	void VL_WinFontDialog::SetFontMustExist(VBool Value)
	{
		FFontMustExist=Value;
	}

	VBool VL_WinFontDialog::IsOpening()
	{
		return FOpening;
	}

	VBool VL_WinFontDialog::Execute(VL_WinForm* Form)
	{
		if(FOpening)
		{
			return false;
		}
		else
		{
			CHOOSEFONT Struct;
			Struct.lStructSize=sizeof(Struct);
			Struct.hwndOwner=Form->GetHandle();
			Struct.hDC=NULL;
			Struct.lpLogFont=&FFontInfo;
			Struct.iPointSize=0;
			Struct.Flags=0;
			Struct.rgbColors=Color;
			Struct.lCustData=(LPARAM)this;
			Struct.lpfnHook=Service_CFHookProc;
			Struct.lpTemplateName=NULL;
			Struct.hInstance=NULL;
			Struct.lpszStyle=NULL;
			Struct.nFontType=0;
			Struct.nSizeMin=0;
			Struct.nSizeMax=0;

			Struct.Flags|=CF_ENABLEHOOK;
			Struct.Flags|=CF_INITTOLOGFONTSTRUCT;
			Struct.Flags|=CF_SCREENFONTS;

			if(FTrueTypeOnly)		Struct.Flags|=CF_TTONLY;
			if(FExtended)			Struct.Flags|=CF_EFFECTS;
			if(FFontMustExist)		Struct.Flags|=CF_FORCEFONTEXIST;

			VBool Result=ChooseFont(&Struct)!=0;
			if(Result)
			{
				Color=Struct.rgbColors;
				Font=new VL_WinFont(&FFontInfo);
			}
			GetCommdlgService()->UnregisterFontDialog(FHandle);
			return Result;
		}
	}

/*********************************************************************************************************
VL_WinCommonDialogService
*********************************************************************************************************/

	VL_WinCommonDialogService::VL_WinCommonDialogService()
	{
		FMessage_ColorOnOK=RegisterWindowMessage(COLOROKSTRING);
	}

	void VL_WinCommonDialogService::RegisterColorDialog(HWND Handle , VL_WinColorDialog* Dialog)
	{
		FColorDialogs.Add(Handle,Dialog);
		Dialog->FHandle=Handle;
	}

	void VL_WinCommonDialogService::UnregisterColorDialog(HWND Handle)
	{
		FColorDialogs.Delete(Handle);
	}

	void VL_WinCommonDialogService::RegisterFileDialog(HWND Handle , VL_WinFileDialog* Dialog)
	{
		FFileDialogs.Add(Handle,Dialog);
		Dialog->FHandle=Handle;
	}

	void VL_WinCommonDialogService::UnregisterFileDialog(HWND Handle)
	{
		FFileDialogs.Delete(Handle);
	}

	void VL_WinCommonDialogService::RegisterFontDialog(HWND Handle , VL_WinFontDialog* Dialog)
	{
		FFontDialogs.Add(Handle,Dialog);
		Dialog->FHandle=Handle;
	}

	void VL_WinCommonDialogService::UnregisterFontDialog(HWND Handle)
	{
		FFontDialogs.Delete(Handle);
	}

/*********************************************************************************************************
对话框钩子
*********************************************************************************************************/

	VL_WinCommonDialogService* FCommonDialogService=0;

	UINT_PTR CALLBACK Service_CCHookProc(HWND hdlg , UINT uiMsg , WPARAM wParam , LPARAM lParam)
	{
		if(uiMsg==WM_INITDIALOG)
		{
			CHOOSECOLOR* pcc=(CHOOSECOLOR*)lParam;
			VL_WinColorDialog* Dialog=(VL_WinColorDialog*)(pcc->lCustData);
			FCommonDialogService->RegisterColorDialog(hdlg,Dialog);

			Dialog->OnInit(Dialog);
		}
		else if(uiMsg==FCommonDialogService->FMessage_ColorOnOK)
		{
			CHOOSECOLOR* pcc=(CHOOSECOLOR*)lParam;
			VBool Cancel=false;
			VL_WinColorDialog* Dialog=FCommonDialogService->FColorDialogs[hdlg];
			Dialog->Color=pcc->rgbResult;
			Dialog->OnOK(Dialog,Cancel);
			return Cancel?1:0;
		}
		return 0;
	}

	UINT_PTR CALLBACK Service_PFNHookProc(HWND hdlg , UINT uiMsg , WPARAM wParam , LPARAM lParam)
	{
		if(uiMsg==WM_INITDIALOG)
		{
			OPENFILENAME* pofn=(OPENFILENAME*)lParam;
			VL_WinFileDialog* Dialog=(VL_WinFileDialog*)(pofn->lCustData);
			FCommonDialogService->RegisterFileDialog(hdlg,Dialog);

			Dialog->OnInit(Dialog);
		}
		else if(uiMsg==WM_NOTIFY)
		{
			OFNOTIFY* pofn=(OFNOTIFY*)lParam;
			if(pofn->hdr.code==CDN_FILEOK)
			{
				VBool Cancel=false;
				VL_WinFileDialog* Dialog=(VL_WinFileDialog*)(pofn->lpOFN->lCustData);
				Dialog->FillResult(*pofn->lpOFN);
				Dialog->OnOK(Dialog,Cancel);
				if(Cancel)
				{
					SetWindowLongPtr(hdlg,DWL_MSGRESULT,1);
					return 1;
				}
			}
		}
		return 0;
	}

	UINT_PTR CALLBACK Service_CFHookProc(HWND hdlg , UINT uiMsg , WPARAM wParam , LPARAM lParam)
	{
		if(uiMsg==WM_INITDIALOG)
		{
			CHOOSEFONT* pcf=(CHOOSEFONT*)lParam;
			VL_WinFontDialog* Dialog=(VL_WinFontDialog*)(pcf->lCustData);
			FCommonDialogService->RegisterFontDialog(hdlg,Dialog);
		}
		return 0;
	}

	VL_WinCommonDialogService* GetCommdlgService()
	{
		return FCommonDialogService;
	}

	void SetCommdlgService(VL_WinCommonDialogService* Service)
	{
		FCommonDialogService=Service;
	}

	}
}