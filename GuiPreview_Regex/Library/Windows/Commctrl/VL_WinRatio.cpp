#include "VL_WinRatio.h"

namespace vl
{
	namespace windows
	{

/*********************************************************************************************************
VL_WinScroll
*********************************************************************************************************/

	void VL_WinScroll::GetInfo(SCROLLINFO& Info)
	{
		memset(&Info,0,sizeof(Info));
		Info.cbSize=sizeof(Info);
		Info.fMask=SIF_ALL;
		SendMessage(FHandle,SBM_GETSCROLLINFO,0,(LPARAM)&Info);
	}

	void VL_WinScroll::SetInfo(SCROLLINFO& Info)
	{
		Info.fMask=SIF_PAGE | SIF_POS | SIF_RANGE;
		SendMessage(FHandle,SBM_SETSCROLLINFO,TRUE,(LPARAM)&Info);
	}

	VL_WinScroll::VL_WinScroll(VL_WinContainer* Parent , VBool Horizontal)
	{
		_CreateWindow(
			0,
			WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | (Horizontal?SBS_HORZ:SBS_VERT),
			WC_SCROLLBAR,
			Parent);
		SetMax(99);
		SetLineSize(1);
		SetPageSize(10);
	}

	VL_WinScroll::~VL_WinScroll()
	{
	}

	LRESULT VL_WinScroll::ProcessMessage(UINT Message , WPARAM& wParam , LPARAM& lParam , VBool& CallDefaultProcedure)
	{
		LRESULT Result=0;
		switch(Message)
		{
		case WM_VSCROLL_DISPATCHED:
		case WM_HSCROLL_DISPATCHED:
			switch(LOWORD(wParam))
			{
			case SB_ENDSCROLL:
				OnEndScroll(this);
				break;
			case SB_LEFT:
				SetPosition(GetMin());
				OnScrollMin(this);
				OnPositionChanged(this);
				break;
			case SB_RIGHT:
				SetPosition(GetMax());
				OnScrollMax(this);
				OnPositionChanged(this);
				break;
			case SB_LINELEFT:
				SetPosition(GetPosition()-GetLineSize());
				OnArrowDec(this);
				OnPositionChanged(this);
				break;
			case SB_LINERIGHT:
				SetPosition(GetPosition()+GetLineSize());
				OnArrowInc(this);
				OnPositionChanged(this);
				break;
			case SB_PAGELEFT:
				SetPosition(GetPosition()-GetPageSize());
				OnPageDec(this);
				OnPositionChanged(this);
				break;
			case SB_PAGERIGHT:
				SetPosition(GetPosition()+GetPageSize());
				OnPageInc(this);
				OnPositionChanged(this);
				break;
			case SB_THUMBPOSITION:
				{
					SCROLLINFO Info;
					GetInfo(Info);
					SetPosition(Info.nTrackPos);
					OnThumbUp(this);
					OnPositionChanged(this);
				}
				break;
			case SB_THUMBTRACK:
				{
					SCROLLINFO Info;
					GetInfo(Info);
					SetPosition(Info.nTrackPos);
					OnTracking(this);
					OnPositionChanged(this);
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

	VInt VL_WinScroll::GetMin()
	{
		SCROLLINFO Info;
		GetInfo(Info);
		return Info.nMin;
	}

	void VL_WinScroll::SetMin(VInt Value)
	{
		SCROLLINFO Info;
		GetInfo(Info);
		Info.nMin=Value;
		SetInfo(Info);
	}

	VInt VL_WinScroll::GetMax()
	{
		SCROLLINFO Info;
		GetInfo(Info);
		return Info.nMax;
	}

	void VL_WinScroll::SetMax(VInt Value)
	{
		SCROLLINFO Info;
		GetInfo(Info);
		Info.nMax=Value;
		SetInfo(Info);
	}

	VInt VL_WinScroll::GetLineSize()
	{
		return FLineSize;
	}

	void VL_WinScroll::SetLineSize(VInt Value)
	{
		FLineSize=Value;
	}

	VInt VL_WinScroll::GetPageSize()
	{
		SCROLLINFO Info;
		GetInfo(Info);
		return Info.nPage;
	}

	void VL_WinScroll::SetPageSize(VInt Value)
	{
		SCROLLINFO Info;
		GetInfo(Info);
		Info.nPage=Value;
		SetInfo(Info);
	}

	VInt VL_WinScroll::GetPosition()
	{
		SCROLLINFO Info;
		GetInfo(Info);
		return Info.nPos;
	}

	void VL_WinScroll::SetPosition(VInt Value)
	{
		SCROLLINFO Info;
		GetInfo(Info);
		Info.nPos=Value;
		SetInfo(Info);
	}

/*********************************************************************************************************
VL_WinTrack
*********************************************************************************************************/

	void VL_WinTrack::GetTickVisible(VBool& Top , VBool& Bottom)
	{
		if(GetStyle(TBS_NOTICKS))
		{
			Top=false;
			Bottom=false;
		}
		else if(GetStyle(TBS_BOTH))
		{
			Top=true;
			Bottom=true;
		}
		else if(GetStyle(TBS_LEFT) || GetStyle(TBS_TOP))
		{
			Top=true;
			Bottom=false;
		}
		else
		{
			Top=false;
			Bottom=true;
		}
	}

	void VL_WinTrack::SetTickVisible(VBool Top , VBool Bottom)
	{
		SetStyle(TBS_LEFT | TBS_TOP,false);
		SetStyle(TBS_RIGHT | TBS_BOTTOM,false);
		SetStyle(TBS_NOTICKS,false);
		SetStyle(TBS_BOTH,false);
		if(Top)
		{
			if(Bottom)
			{
				SetStyle(TBS_BOTH,true);
			}
			else
			{
				SetStyle(TBS_LEFT | TBS_TOP,true);
			}
		}
		else
		{
			if(Bottom)
			{
				SetStyle(TBS_RIGHT | TBS_BOTTOM,true);
			}
			else
			{
				SetStyle(TBS_NOTICKS,true);
			}
		}
	}
	
	VL_WinTrack::VL_WinTrack(VL_WinContainer* Parent , VBool Horizontal)
	{
		_CreateWindow(
			0,
			WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | (Horizontal?TBS_HORZ:TBS_VERT) | TBS_BOTTOM | TBS_RIGHT,
			TRACKBAR_CLASS,
			Parent);
		SendMessage(FHandle,TBM_SETUNICODEFORMAT,TRUE,0);
		SetMax(10);
	}

	VL_WinTrack::~VL_WinTrack()
	{
	}

	LRESULT VL_WinTrack::ProcessMessage(UINT Message , WPARAM& wParam , LPARAM& lParam , VBool& CallDefaultProcedure)
	{
		LRESULT Result=0;
		switch(Message)
		{
		case WM_VSCROLL_DISPATCHED:
		case WM_HSCROLL_DISPATCHED:
			switch(LOWORD(wParam))
			{
			case TB_ENDTRACK:
				OnEndScroll(this);
				break;
			case TB_TOP:
				OnScrollMin(this);
				OnPositionChanged(this);
				break;
			case TB_BOTTOM:
				OnScrollMax(this);
				OnPositionChanged(this);
				break;
			case TB_LINEUP:
				OnArrowDec(this);
				OnPositionChanged(this);
				break;
			case TB_LINEDOWN:
				OnArrowInc(this);
				OnPositionChanged(this);
				break;
			case TB_PAGEUP:
				OnPageDec(this);
				OnPositionChanged(this);
				break;
			case TB_PAGEDOWN:
				OnPageInc(this);
				OnPositionChanged(this);
				break;
			case TB_THUMBPOSITION:
				OnThumbUp(this);
				OnPositionChanged(this);
				break;
			case TB_THUMBTRACK:
				OnTracking(this);
				OnPositionChanged(this);
				break;
			}
			Result=VL_WinControl::ProcessMessage(Message,wParam,lParam,CallDefaultProcedure);
			break;
		default:
			Result=VL_WinControl::ProcessMessage(Message,wParam,lParam,CallDefaultProcedure);
		}
		return Result;
	}

	VBool VL_WinTrack::GetShowLeftTopTick()
	{
		VBool Top,Bottom;
		GetTickVisible(Top,Bottom);
		return Top;
	}

	void VL_WinTrack::SetShowLeftTopTick(VBool Value)
	{
		VBool Top,Bottom;
		GetTickVisible(Top,Bottom);
		Top=Value;
		SetTickVisible(Top,Bottom);
	}

	VBool VL_WinTrack::GetShowRightBottomTick()
	{
		VBool Top,Bottom;
		GetTickVisible(Top,Bottom);
		return Bottom;
	}

	void VL_WinTrack::SetShowRightBottomTick(VBool Value)
	{
		VBool Top,Bottom;
		GetTickVisible(Top,Bottom);
		Bottom=Value;
		SetTickVisible(Top,Bottom);
	}

	VBool VL_WinTrack::GetEnableSelection()
	{
		return GetStyle(TBS_ENABLESELRANGE);
	}

	void VL_WinTrack::SetEnableSelection(VBool Value)
	{
		SetStyle(TBS_ENABLESELRANGE,Value);
	}

	VBool VL_WinTrack::GetAutoTicks()
	{
		return GetStyle(TBS_AUTOTICKS);
	}

	void VL_WinTrack::SetAutoTicks(VBool Value)
	{
		SetStyle(TBS_AUTOTICKS,Value);
	}

	VInt VL_WinTrack::GetMin()
	{
		return SendMessage(FHandle,TBM_GETRANGEMIN,0,0);
	}

	void VL_WinTrack::SetMin(VInt Value)
	{
		SendMessage(FHandle,TBM_SETRANGEMIN,TRUE,Value);
	}

	VInt VL_WinTrack::GetMax()
	{
		return SendMessage(FHandle,TBM_GETRANGEMAX,0,0);
	}

	void VL_WinTrack::SetMax(VInt Value)
	{
		SendMessage(FHandle,TBM_SETRANGEMAX,TRUE,Value);
	}

	VInt VL_WinTrack::GetLineSize()
	{
		return SendMessage(FHandle,TBM_GETLINESIZE,0,0);
	}

	void VL_WinTrack::SetLineSize(VInt Value)
	{
		SendMessage(FHandle,TBM_SETLINESIZE,0,Value);
	}

	VInt VL_WinTrack::GetPageSize()
	{
		return SendMessage(FHandle,TBM_GETPAGESIZE,0,0);
	}

	void VL_WinTrack::SetPageSize(VInt Value)
	{
		SendMessage(FHandle,TBM_SETPAGESIZE,0,Value);
	}

	VInt VL_WinTrack::GetPosition()
	{
		return SendMessage(FHandle,TBM_GETPOS,0,0);
	}

	void VL_WinTrack::SetPosition(VInt Value)
	{
		SendMessage(FHandle,TBM_SETPOS,TRUE,Value);
	}

	VInt VL_WinTrack::GetSelStart()
	{
		return SendMessage(FHandle,TBM_GETSELSTART,0,0);
	}

	void VL_WinTrack::SetSelStart(VInt Value)
	{
		SendMessage(FHandle,TBM_SETSELSTART,TRUE,Value);
	}

	VInt VL_WinTrack::GetSelEnd()
	{
		return SendMessage(FHandle,TBM_GETSELEND,0,0);
	}

	void VL_WinTrack::SetSelEnd(VInt Value)
	{
		SendMessage(FHandle,TBM_SETSELEND,TRUE,Value);
	}

	void VL_WinTrack::ClearSelection()
	{
		SendMessage(FHandle,TBM_CLEARSEL,TRUE,0);
	}

	void VL_WinTrack::ClearTicks()
	{
		SendMessage(FHandle,TBM_CLEARTICS,TRUE,0);
	}

	VInt VL_WinTrack::GetTicksCount()
	{
		return SendMessage(FHandle,TBM_GETNUMTICS,0,0);
	}

	void VL_WinTrack::GetAdditionalTicks(VL_List<VInt , true>& TickPos)
	{
		VInt Count=GetTicksCount()-2;
		if(Count<=0)
		{
			TickPos.Clear();
		}
		else
		{
			TickPos.SetCapacity(Count);
			DWORD* Ticks=(DWORD*)SendMessage(FHandle,TBM_GETPTICS,0,0);
			for(VInt i=0;i<Count;i++)
			{
				TickPos.Add(Ticks[i]);
			}
		}
	}

	VInt VL_WinTrack::GetTickPos(VInt Index)
	{
		return SendMessage(FHandle,TBM_GETTIC,Index,0);
	}

	void VL_WinTrack::SetTick(VInt Position)
	{
		SendMessage(FHandle,TBM_SETTIC,0,Position);
	}

	void VL_WinTrack::BuildAutoTicks(VInt Value)
	{
		SendMessage(FHandle,TBM_SETTICFREQ,Value,0);
	}

/*********************************************************************************************************
VL_WinProgress
*********************************************************************************************************/

	void VL_WinProgress::InternalSetMarquee()
	{
		SendMessage(FHandle,PBM_SETMARQUEE,(FMarqueeRunning?TRUE:FALSE),FMarqueeInterval/100);
	}

	VL_WinProgress::VL_WinProgress(VL_WinContainer* Parent , VBool Horizontal)
	{
		_CreateWindow(
			0,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | (Horizontal?0:PBS_VERTICAL),
			PROGRESS_CLASS,
			Parent);
		FMarqueeRunning=true;
		FMarqueeInterval=5000;
		SetMax(100);
		SetStep(5);
		InternalSetMarquee();
	}

	VL_WinProgress::~VL_WinProgress()
	{
	}

	VBool VL_WinProgress::GetMarquee()
	{
		return GetStyle(PBS_MARQUEE);
	}

	void VL_WinProgress::SetMarquee(VBool Value)
	{
		SetStyle(PBS_MARQUEE,Value);
	}

	VBool VL_WinProgress::GetSmooth()
	{
		return GetStyle(PBS_SMOOTH);
	}

	void VL_WinProgress::SetSmooth(VBool Value)
	{
		SetStyle(PBS_SMOOTH,Value);
	}

	VBool VL_WinProgress::GetSmoothReverse()
	{
		return GetStyle(PBS_SMOOTHREVERSE);
	}

	void VL_WinProgress::SetSmoothReverse(VBool Value)
	{
		SetStyle(PBS_SMOOTHREVERSE,Value);
	}

	COLORREF VL_WinProgress::GetBarColor()
	{
		return SendMessage(FHandle,PBM_GETBARCOLOR,0,0);
	}

	void VL_WinProgress::SetBarColor(COLORREF Value)
	{
		SendMessage(FHandle,PBM_SETBARCOLOR,0,Value);
	}

	COLORREF VL_WinProgress::GetBackColor()
	{
		return SendMessage(FHandle,PBM_GETBKCOLOR,0,0);
	}

	void VL_WinProgress::SetBackColor(COLORREF Value)
	{
		SendMessage(FHandle,PBM_SETBKCOLOR,0,Value);
	}

	VInt VL_WinProgress::GetPosition()
	{
		return SendMessage(FHandle,PBM_GETPOS,0,0);
	}

	void VL_WinProgress::SetPosition(VInt Value)
	{
		SendMessage(FHandle,PBM_SETPOS,Value,0);
	}

	VInt VL_WinProgress::GetMin()
	{
		return SendMessage(FHandle,PBM_GETRANGE,TRUE,0);
	}

	void VL_WinProgress::SetMin(VInt Value)
	{
		SendMessage(FHandle,PBM_SETRANGE32,Value,GetMax());
	}

	VInt VL_WinProgress::GetMax()
	{
		return SendMessage(FHandle,PBM_GETRANGE,FALSE,0);
	}

	void VL_WinProgress::SetMax(VInt Value)
	{
		SendMessage(FHandle,PBM_SETRANGE32,GetMin(),Value);
	}

	VInt VL_WinProgress::GetStep()
	{
		return SendMessage(FHandle,PBM_GETSTEP,0,0);
	}

	void VL_WinProgress::SetStep(VInt Value)
	{
		SendMessage(FHandle,PBM_SETSTEP,Value,0);
	}

	VBool VL_WinProgress::GetMarqueeRunning()
	{
		return FMarqueeRunning;
	}

	void VL_WinProgress::SetMarqueeRunning(VBool Value)
	{
		FMarqueeRunning=Value;
		InternalSetMarquee();
	}

	VInt VL_WinProgress::GetMarqueeInterval()
	{
		return FMarqueeInterval;
	}

	void VL_WinProgress::SetMarqueeInterval(VInt Value)
	{
		FMarqueeInterval=Value;
		InternalSetMarquee();
	}

	}
}