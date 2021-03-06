#include "VL_WinButton.h"

namespace vl
{
	namespace windows
	{

/*********************************************************************************************************
VL_WinCustomButton
*********************************************************************************************************/

	VL_WinCustomButton::VL_WinCustomButton(VL_WinContainer* Parent , DWORD Style)
	{
		_CreateWindow(
			0,
			Style | BS_NOTIFY | BS_BITMAP | WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
			WC_BUTTON,
			Parent);
	}

	LRESULT VL_WinCustomButton::ProcessMessage(UINT Message , WPARAM& wParam , LPARAM& lParam , VBool& CallDefaultProcedure)
	{
		LRESULT Result=0;
		switch(Message)
		{
		case WM_COMMAND_DISPATCHED:
			switch(HIWORD(wParam))
			{
			case BN_CLICKED:
				OnClick(this);
				break;
			}
			Result=VL_WinControl::ProcessMessage(Message,wParam,lParam,CallDefaultProcedure);
			break;
		default:
			Result=VL_WinControl::ProcessMessage(Message,wParam,lParam,CallDefaultProcedure);
		}
		return Result;
	}

	VLE_Align VL_WinCustomButton::GetTextVAlign()
	{
		if(GetStyle(BS_TOP))			return vaLeftTop;
		else if(GetStyle(BS_BOTTOM))	return vaRightBottom;
		else							return vaCenter;
	}

	void VL_WinCustomButton::SetTextVAlign(VLE_Align Value)
	{
		switch(Value)
		{
		case vaLeftTop:
			SetStyle(BS_TOP,true);
			SetStyle(BS_VCENTER,false);
			SetStyle(BS_BOTTOM,false);
			break;
		case vaCenter:
			SetStyle(BS_TOP,false);
			SetStyle(BS_VCENTER,true);
			SetStyle(BS_BOTTOM,false);
			break;
		case vaRightBottom:
			SetStyle(BS_TOP,false);
			SetStyle(BS_VCENTER,false);
			SetStyle(BS_BOTTOM,true);
			break;
		}
	}

	VLE_Align VL_WinCustomButton::GetTextHAlign()
	{
		if(GetStyle(BS_LEFT))			return vaLeftTop;
		else if(GetStyle(BS_RIGHT))		return vaRightBottom;
		else							return vaCenter;
	}

	void VL_WinCustomButton::SetTextHAlign(VLE_Align Value)
	{
		switch(Value)
		{
		case vaLeftTop:
			SetStyle(BS_LEFT,true);
			SetStyle(BS_CENTER,false);
			SetStyle(BS_RIGHT,false);
			break;
		case vaCenter:
			SetStyle(BS_LEFT,false);
			SetStyle(BS_CENTER,true);
			SetStyle(BS_RIGHT,false);
			break;
		case vaRightBottom:
			SetStyle(BS_LEFT,false);
			SetStyle(BS_CENTER,false);
			SetStyle(BS_RIGHT,true);
			break;
		}
	}

/*********************************************************************************************************
VL_WinCustomButton
*********************************************************************************************************/

	VL_WinCustomChecked::VL_WinCustomChecked(VL_WinContainer* Parent , DWORD Style):VL_WinCustomButton(Parent,Style)
	{
	}

	VBool VL_WinCustomChecked::GetChecked()
	{
		return SendMessage(FHandle,BM_GETCHECK,0,0)==BST_CHECKED;
	}

	void VL_WinCustomChecked::SetChecked(VBool Value)
	{
		SendMessage(FHandle,BM_SETCHECK,(Value?BST_CHECKED:BST_UNCHECKED),0);
	}

	VBool VL_WinCustomChecked::GetLeftText()
	{
		return GetStyle(BS_LEFTTEXT);
	}

	void VL_WinCustomChecked::SetLeftText(VBool Value)
	{
		SetStyle(BS_LEFTTEXT,Value);
	}

	VBool VL_WinCustomChecked::GetPushLike()
	{
		return GetStyle(BS_PUSHLIKE);
	}

	void VL_WinCustomChecked::SetPushLike(VBool Value)
	{
		SetStyle(BS_PUSHLIKE,Value);
	}

/*********************************************************************************************************
VL_WinButton
*********************************************************************************************************/
	
	VL_WinButton::VL_WinButton(VL_WinContainer* Parent):VL_WinCustomButton(Parent,BS_PUSHBUTTON | BS_CENTER | BS_VCENTER)
	{
	}

/*********************************************************************************************************
VL_WinCommandLink
*********************************************************************************************************/

	VL_WinCommandLink::VL_WinCommandLink(VL_WinContainer* Parent):VL_WinCustomButton(Parent,BS_COMMANDLINK | BS_CENTER | BS_VCENTER)
	{
	}

	VUnicodeString VL_WinCommandLink::GetNote()
	{
		DWORD Length=0;
		SendMessage(FHandle,BCM_GETNOTE,(WPARAM)&Length,(LPARAM)&Length);
		PWChar Buffer=new VWChar[Length+1];
		SendMessage(FHandle,BCM_GETNOTE,(WPARAM)&Length,(LPARAM)Buffer);
		VUnicodeString Note=Buffer;
		delete[] Buffer;
		return Note;
	}

	void VL_WinCommandLink::SetNote(VUnicodeString Note)
	{
		SendMessage(FHandle,BCM_SETNOTE,0,(LPARAM)Note.Buffer());
	}

/*********************************************************************************************************
VL_WinCheck
*********************************************************************************************************/

	VL_WinCheck::VL_WinCheck(VL_WinContainer* Parent):VL_WinCustomChecked(Parent,BS_AUTOCHECKBOX | BS_LEFT | BS_VCENTER)
	{
	}

/*********************************************************************************************************
VL_WinRadio
*********************************************************************************************************/

	VL_WinRadio::VL_WinRadio(VL_WinContainer* Parent):VL_WinCustomChecked(Parent,BS_RADIOBUTTON | BS_LEFT | BS_VCENTER)
	{
		FGroupNumber=-1;
	}

	LRESULT VL_WinRadio::ProcessMessage(UINT Message , WPARAM& wParam , LPARAM& lParam , VBool& CallDefaultProcedure)
	{
		LRESULT Result=0;
		switch(Message)
		{
		case WM_COMMAND_DISPATCHED:
			switch(HIWORD(wParam))
			{
			case BN_CLICKED:
				if(!GetChecked())
				{
					SetChecked(true);
				}
				break;
			}
			Result=VL_WinCustomButton::ProcessMessage(Message,wParam,lParam,CallDefaultProcedure);
			break;
		default:
			Result=VL_WinCustomButton::ProcessMessage(Message,wParam,lParam,CallDefaultProcedure);
		}
		return Result;
	}

	void VL_WinRadio::SetChecked(VBool Value)
	{
		if(Value)
		{
			for(VInt i=0;i<FParent->GetControlCount();i++)
			{
				VL_WinRadio* Radio=dynamic_cast<VL_WinRadio*>(FParent->GetControl(i));
				if(Radio && Radio!=this && Radio->FGroupNumber==FGroupNumber)
				{
					Radio->SetChecked(false);
				}
			}
		}
		VL_WinCustomChecked::SetChecked(Value);
	}

	VInt VL_WinRadio::GetGroupNumber()
	{
		return FGroupNumber;
	}

	void VL_WinRadio::SetGroupNumber(VInt Value)
	{
		FGroupNumber=Value;
	}

/*********************************************************************************************************
VL_WinSplitButton
*********************************************************************************************************/

	VL_WinSplitButton::VL_WinSplitButton(VL_WinContainer* Parent):VL_WinCustomButton(Parent,BS_SPLITBUTTON | BS_CENTER | BS_VCENTER)
	{
	}

	LRESULT VL_WinSplitButton::ProcessMessage(UINT Message , WPARAM& wParam , LPARAM& lParam , VBool& CallDefaultProcedure)
	{
		LRESULT Result=0;
		switch(Message)
		{
		case WM_NOTIFY_DISPATCHED:
			{
				NMHDR* hdr=(NMHDR*)lParam;
				if(hdr->code==BCN_DROPDOWN)
				{
					NMBCDROPDOWN* param=(NMBCDROPDOWN*)lParam;
					OnDropDown(this,param->rcButton);
				}
			}
			Result=VL_WinCustomButton::ProcessMessage(Message,wParam,lParam,CallDefaultProcedure);
			break;
		}
		return Result;
	}

	}
}