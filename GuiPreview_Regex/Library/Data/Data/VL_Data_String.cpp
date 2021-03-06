#define _CRT_SECURE_NO_DEPRECATE
#include <mbstring.h>
#include <windows.h>
#include "VL_Data_String.h"
#pragma warning(disable:4996)

namespace vl
{
	VInt MbcsPoser(PCChar Sub , PChar Self)
	{
		PChar Result=(PChar)_mbsstr((unsigned char*)Self,(unsigned char*)Sub);
		if(Result)
		{
			return (VInt)(Result-Self);
		}
		else
		{
			return -1;
		}
	}

	void MbcsCaser(PChar Sub , VBool ToLower)
	{
		if(ToLower)
		{
			_mbslwr((unsigned char*)Sub);
		}
		else
		{
			_mbsupr((unsigned char*)Sub);
		}
	}

	VInt MbcsToInt(PCChar Buffer)
	{
		if(Buffer)
		{
			return atoi(Buffer);
		}
		else
		{
			return 0;
		}
	}

	VDouble MbcsToDouble(PCChar Buffer)
	{
		if(Buffer)
		{
			return atof(Buffer);
		}
		else
		{
			return 0;
		}
	}

	PChar MbcsFromInt(VInt Num , VInt& Len)
	{
		PChar Buffer=new VChar[100];
		_itoa(Num,Buffer,10);
		Len=(VInt)strlen(Buffer);
		return Buffer;
	}

	PChar MbcsFromDouble(VDouble* Num , VInt Digits , VInt& Len)
	{
		PChar Buffer=new VChar[100];
		_gcvt(*Num,Digits,Buffer);
		Len=(VInt)strlen(Buffer);
		if(Buffer[Len-1]=='.')
		{
			Buffer[Len]='0';
			Buffer[++Len]='\0';
		}
		return Buffer;
	}

	VInt UnicodePoser(PCWChar Sub , PWChar Self)
	{
		PWChar Result=wcsstr(Self,Sub);
		if(Result)
		{
			return (VInt)(Result-Self);
		}
		else
		{
			return -1;
		}
	}

	void UnicodeCaser(PWChar Sub , VBool ToLower)
	{
		if(ToLower)
		{
			wcslwr(Sub);
		}
		else
		{
			wcsupr(Sub);
		}
	}

	VInt  UnicodeToInt(PCWChar Buffer)
	{
		if(Buffer)
		{
			return _wtoi(Buffer);
		}
		else
		{
			return 0;
		}
	}

	VDouble  UnicodeToDouble(PCWChar Buffer)
	{
		if(Buffer)
		{
			return _wtof(Buffer);
		}
		else
		{
			return 0;
		}
	}

	PWChar UnicodeFromInt(VInt Num , VInt& Len)
	{
		PWChar Buffer=new VWChar[100];
		_itow(Num,Buffer,10);
		Len=(VInt)wcslen(Buffer);
		return Buffer;
	}

	PWChar UnicodeFromDouble(VDouble* Num , VInt Digits , VInt& Len)
	{
		PChar Buffer=MbcsFromDouble(Num,Digits,Len);
		VUnicodeString s=ToUnicode(Buffer);
		delete[] Buffer;
		PWChar WBuf=new VWChar[s.Length()+1];
		memcpy(WBuf,s.Buffer(),(s.Length()+1)*sizeof(VWChar));
		Len=s.Length();
		return WBuf;
	}

	VMbcsString ToMbcs(VUnicodeString String)
	{
		VInt Length=WideCharToMultiByte(CP_THREAD_ACP,0,String.Buffer(),String.Length(),0,0,0,0);
		PChar Temp=new VChar[Length+1];
		WideCharToMultiByte(CP_THREAD_ACP,0,String.Buffer(),String.Length(),Temp,Length,0,0);
		Temp[Length]='\0';
		VMbcsString s=Temp;
		delete[] Temp;
		return s;
	}

	VUnicodeString ToUnicode(VMbcsString String)
	{
		VInt Length=MultiByteToWideChar(CP_THREAD_ACP,0,String.Buffer(),String.Length(),0,0);
		PWChar Temp=new VWChar[Length+1];
		MultiByteToWideChar(CP_THREAD_ACP,0,String.Buffer(),String.Length(),Temp,Length);
		Temp[Length]=L'\0';
		VUnicodeString s=Temp;
		delete[] Temp;
		return s;
	}

	/*istream& operator>>(istream& i,VMbcsString& S)
	{
		VChar p[10000];
		i>>p;
		S=p;
		return i;
	}

	istream& operator>>(istream& i,VUnicodeString& S)
	{
		VMbcsString m;
		i>>m;
		S=ToUnicode(m);
		return i;
	}

	ostream& operator<<(ostream& o,VMbcsString S)
	{
		return o<<S.Buffer();
	}

	ostream& operator<<(ostream& o,VUnicodeString S)
	{
		return o<<ToMbcs(S).Buffer();
	}*/
}