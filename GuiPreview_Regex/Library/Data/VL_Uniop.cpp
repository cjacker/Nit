#include "VL_Uniop.h"

namespace vl
{
	namespace uniop
	{

/*********************************************************************************************************
VL_UniBuilder
*********************************************************************************************************/

	void VL_UniBuilder::RequireSpace(VInt Count)
	{
		if(FLength+Count>FSize)
		{
			FSize+=(Count%FBlockSize)?(Count/FBlockSize+1)*FBlockSize:Count;
			PWChar NewBuffer=new VWChar[FSize];
			wcsncpy(NewBuffer,FBuffer,FLength);
			delete[] FBuffer;
			FBuffer=NewBuffer;
		}
	}

	VL_UniBuilder::VL_UniBuilder(VInt BlockSize)
	{
		FBuffer=new VWChar[BlockSize];
		FBlockSize=BlockSize;
		FLength=0;
		FSize=BlockSize;
	}

	VL_UniBuilder::~VL_UniBuilder()
	{
		delete[] FBuffer;
	}

	VInt VL_UniBuilder::Length()
	{
		return FLength;
	}

	VUnicodeString VL_UniBuilder::GetString()
	{
		return VUnicodeString(FBuffer,FLength);
	}

	VUnicodeString VL_UniBuilder::GetString(VInt Index , VInt Count)
	{
		return VUnicodeString(FBuffer+Index,Count);
	}

	void VL_UniBuilder::Append(VUnicodeString String)
	{
		Append(String.Buffer(),String.Length());
	}

	void VL_UniBuilder::Append(PWChar String , VInt Count)
	{
		if(Count==-1)
		{
			Count=(VInt)wcslen(String);
		}
		RequireSpace(Count);
		wcsncpy(FBuffer+FLength,String,Count);
		FLength+=Count;
	}

	void VL_UniBuilder::Append(VWChar Char)
	{
		RequireSpace(1);
		FBuffer[FLength++]=Char;
	}

	void VL_UniBuilder::Insert(VInt Index , VUnicodeString String)
	{
		Insert(Index,String.Buffer(),String.Length());
	}

	void VL_UniBuilder::Insert(VInt Index , PWChar String , VInt Count)
	{
		if(Count==-1)
		{
			Count=(VInt)wcslen(String);
		}
		if(Index<0)
		{
			return;
		}
		else if(Index==FLength)
		{
			Append(String,Count);
		}
		else
		{
			RequireSpace(Count);
			VInt TempSize=FLength-Index;
			PWChar Temp=new VWChar[TempSize];
			wcsncpy(Temp,FBuffer+Index,TempSize);
			wcsncpy(FBuffer+Index,String,Count);
			wcsncpy(FBuffer+Index+Count,Temp,TempSize);
			delete[] Temp;
			FLength+=Count;
		}
	}

	void VL_UniBuilder::Delete(VInt Index , VInt Count)
	{
		if(Index<0)
		{
			Count+=Index;
			Index=0;
		}
		else if(Index>=FLength)
		{
			return;
		}
		if(Index+Count>FLength)
		{
			Count=FLength-Index;
		}
		wcsncpy(FBuffer+Index,FBuffer+Index+Count,FLength-Count);
		FLength-=Count;
	}

	void VL_UniBuilder::Clear()
	{
		FLength=0;
	}

	void VL_UniBuilder::operator +=(VUnicodeString String)
	{
		Append(String);
	}

	void VL_UniBuilder::operator +=(PWChar String)
	{
		Append(String);
	}

	void VL_UniBuilder::operator +=(VWChar Char)
	{
		Append(Char);
	}

	VWChar& VL_UniBuilder::operator [](VInt Index)
	{
		return FBuffer[Index];
	}

/*********************************************************************************************************
VL_UniReplacer
*********************************************************************************************************/

	void VL_UniReplacer::DestroyCache()
	{
		if(FStrFroms)delete[] FStrFroms;
		if(FStrTos)delete[] FStrTos;
		FStrFroms=0;
		FStrTos=0;
		FCacheBuilt=false;
	}

	void VL_UniReplacer::BuildCache()
	{
		if(!FCacheBuilt)
		{
			DestroyCache();
			FStrFroms=new VUnicodeString[FStrCount];
			FStrTos=new VUnicodeString[FStrCount];
			for(VInt Index=0;Index<FRepList.KeyCount();Index++)
			{
				FStrFroms[Index]=FRepList.KeyOfIndex(Index);
				FStrTos[Index]=FRepList.ValueOfIndex(Index);
			}
			FCacheBuilt=true;
		}
	}

	VL_UniReplacer::VL_UniReplacer()
	{
		FCacheBuilt=false;
		FStrFroms=0;
		FStrTos=0;
		FStrCount=0;
		FMaxRatio=1;
	}

	VL_UniReplacer::~VL_UniReplacer()
	{
		DestroyCache();
	}
	
	void VL_UniReplacer::SetItem(VUnicodeString StrFrom , VUnicodeString StrTo)
	{
		if(FRepList.Add(StrFrom,StrTo))
		{
			VInt Ratio=(VInt)ceil((VDouble)StrTo.Length()/StrFrom.Length());
			if(Ratio>FMaxRatio)
			{
				FMaxRatio=Ratio;
			}
			FStrCount++;
			FCacheBuilt=false;
		}
	}

	VUnicodeString VL_UniReplacer::Replace(VUnicodeString Str , VInt& ReplaceCount)
	{
		BuildCache();
		ReplaceCount=0;
		PWChar Buffer=new VWChar[Str.Length()*FMaxRatio+1];
		PWChar WritePoint=Buffer;
		PWChar ReadPoint=Str.Buffer();
		while(*ReadPoint)
		{
			VBool Replaced=false;
			VInt MatchedIndex=-1;
			VInt MatchedLength=0;
			for(VInt i=FStrCount-1;i>=0;i--)
			{
				if(wcsncmp(ReadPoint,FStrFroms[i].Buffer(),FStrFroms[i].Length())==0)
				{
					if(MatchedLength<FStrFroms[i].Length())
					{
						MatchedIndex=i;
						MatchedLength=FStrFroms[i].Length();
					}
				}
			}
			if(MatchedIndex>-1)
			{
				wcsncpy(WritePoint,FStrTos[MatchedIndex].Buffer(),FStrTos[MatchedIndex].Length());
				ReadPoint+=FStrFroms[MatchedIndex].Length();
				WritePoint+=FStrTos[MatchedIndex].Length();
				Replaced=true;
				ReplaceCount++;
			}
			if(Replaced)
			{
				continue;
			}
			*WritePoint++=*ReadPoint++;
		}
		*WritePoint=L'\0';
		VUnicodeString Result=Buffer;
		delete[] Buffer;
		return Result;
	}

/*********************************************************************************************************
VL_UniSpliter
*********************************************************************************************************/

	void VL_UniSpliter::DestroyCache()
	{
		if(FSpliters)delete[] FSpliters;
		if(FReserveds)delete[] FReserveds;
		FSpliters=0;
		FReserveds=0;
		FCacheBuilt=false;
	}

	void VL_UniSpliter::BuildCache()
	{
		if(!FCacheBuilt)
		{
			DestroyCache();
			FSpliters=new VUnicodeString[FSpliterCount];
			FReserveds=new VBool[FSpliterCount];
			VInt Index=0;
			for(VInt Index=0;Index<FSpliterList.KeyCount();Index++)
			{
				FSpliters[Index]=FSpliterList.KeyOfIndex(Index);
				FReserveds[Index]=FSpliterList.ValueOfIndex(Index);
			}
			FCacheBuilt=true;
		}
	}

	VL_UniSpliter::VL_UniSpliter()
	{
		FCacheBuilt=false;
		FSpliters=0;
		FReserveds=0;
		FSpliterCount=0;
	}

	VL_UniSpliter::~VL_UniSpliter()
	{
		DestroyCache();
	}

	void VL_UniSpliter::SetItem(VUnicodeString Spliter , VBool Reserved)
	{
		if(FSpliterList.Add(Spliter,Reserved))
		{
			FSpliterList.Add(Spliter,Reserved);
			FSpliterCount++;
			FCacheBuilt=false;
		}
	}

	void VL_UniSpliter::Split(VUnicodeString Str , VL_List<VUnicodeString , false>& Result , VBool DiscardEmptyPart)
	{
		BuildCache();
		Result.Clear();
		PWChar ReadPoint=Str.Buffer();
		PWChar WordBegin=ReadPoint;
		while(*ReadPoint)
		{
			VBool Splited=false;
			for(VInt i=FSpliterCount-1;i>=0;i--)
			{
				if(wcsncmp(ReadPoint,FSpliters[i].Buffer(),FSpliters[i].Length())==0)
				{
					if(WordBegin==ReadPoint)
					{
						if(!DiscardEmptyPart)
						{
							Result.Add(L"");
						}
					}
					else
					{
						Result.Add(VUnicodeString(WordBegin,(VInt)(ReadPoint-WordBegin)));
					}
					if(FReserveds[i])
					{
						Result.Add(FSpliters[i]);
					}
					ReadPoint+=FSpliters[i].Length();
					WordBegin=ReadPoint;
					Splited=true;
				}
			}
			if(Splited)
			{
				continue;
			}
			ReadPoint++;
		}
		if(WordBegin!=ReadPoint)
		{
			Result.Add(VUnicodeString(WordBegin,(VInt)(ReadPoint-WordBegin)));
		}
	}

/*********************************************************************************************************
VL_UniStrings
*********************************************************************************************************/

	void VL_UniStrings::Split(VUnicodeString Text , VL_List<VUnicodeString , false>& Result)
	{
		PWChar ReadPoint=Text.Buffer();
		PWChar WordStart=ReadPoint;
		VWChar Current;
		while(Current=*ReadPoint)
		{
			switch(Current)
			{
			case L'\r':
				if(ReadPoint[1]==L'\n')
				{
					Result.Add(VUnicodeString(WordStart,(VInt)(ReadPoint-WordStart)));
					ReadPoint+=2;
					WordStart=ReadPoint;
					break;
				}
			case L'\n':
				Result.Add(VUnicodeString(WordStart,(VInt)(ReadPoint-WordStart)));
				ReadPoint++;
				WordStart=ReadPoint;
				break;
			default:
				ReadPoint++;
			}
		}
		if(WordStart!=ReadPoint)
		{
			Result.Add(VUnicodeString(WordStart,(VInt)(ReadPoint-WordStart)));
		}
	}

	VL_UniStrings::VL_UniStrings()
	{
	}

	VL_UniStrings::~VL_UniStrings()
	{
	}

	VInt VL_UniStrings::GetCount()
	{
		return FStrings.GetCount();
	}

	VUnicodeString VL_UniStrings::Fetch(VInt Index)
	{
		return FStrings.Fetch(Index);
	}

	void VL_UniStrings::Delete(VInt Index)
	{
		FStrings.Delete(Index);
	}

	void VL_UniStrings::Clear()
	{
		FStrings.Clear();
	}

	VUnicodeString& VL_UniStrings::operator[](VInt Index)
	{
		return FStrings[Index];
	}

	void VL_UniStrings::Add(VUnicodeString String)
	{
		VL_List<VUnicodeString , false> List;
		Split(String,List);
		for(VInt i=0;i<List.GetCount();i++)
		{
			FStrings.Add(List[i]);
		}
	}

	void VL_UniStrings::Insert(VInt Index , VUnicodeString String)
	{
		VL_List<VUnicodeString , false> List;
		Split(String,List);
		for(VInt i=0;i<List.GetCount();i++)
		{
			FStrings.Insert(Index+i,List[i]);
		}
	}

	VInt VL_UniStrings::IndexOf(VUnicodeString String)
	{
		return FStrings.IndexOf(String);
	}

	VBool VL_UniStrings::Exists(VUnicodeString String)
	{
		return FStrings.Exists(String);
	}

	VUnicodeString VL_UniStrings::GetText()
	{
		VL_UniBuilder Builder(1024);
		for(VInt i=0;i<FStrings.GetCount();i++)
		{
			Builder.Append(FStrings[i]);
			Builder.Append(L"\r\n",2);
		}
		return Builder.GetString();
	}

	void VL_UniStrings::SetText(VUnicodeString Text)
	{
		Split(Text,FStrings);
	}

/*********************************************************************************************************
VL_UniSepstrBuilder
*********************************************************************************************************/

	VL_UniSepstrBuilder::VL_UniSepstrBuilder(VInt BlockSize):FBuilder(BlockSize)
	{
		FReplacer.SetItem(L"\"",L"\\\"");
		FReplacer.SetItem(L"\r",L"\\r");
		FReplacer.SetItem(L"\n",L"\\n");
		FReplacer.SetItem(L"\\",L"\\\\");
		FReplacer.SetItem(L" ",L" ");
		FReplacer.SetItem(L"\t",L"\t");
	}

	VInt VL_UniSepstrBuilder::Length()
	{
		return FBuilder.Length();
	}

	VUnicodeString VL_UniSepstrBuilder::GetString()
	{
		return FBuilder.GetString();
	}

	void VL_UniSepstrBuilder::Append(VUnicodeString String)
	{
		VInt rc=0;
		VUnicodeString Encoded=FReplacer.Replace(String,rc);
		if(rc)
		{
			FBuilder.Append(L"\"",1);
			FBuilder.Append(Encoded);
			FBuilder.Append(L"\"",1);
		}
		else
		{
			FBuilder.Append(Encoded);
		}
	}

	void VL_UniSepstrBuilder::Append(PWChar String , VInt Count)
	{
		if(Count==-1)
		{
			Count=(VInt)wcslen(String);
		}
		Append(VUnicodeString(String,Count));
	}

	void VL_UniSepstrBuilder::AppendUnencoded(VUnicodeString String)
	{
		FBuilder.Append(String);
	}

	void VL_UniSepstrBuilder::AppendUnencoded(PWChar String , VInt Count)
	{
		FBuilder.Append(String,Count);
	}

/*********************************************************************************************************
VL_UniSepstrParser
*********************************************************************************************************/

	VL_UniSepstrParser::VL_UniSepstrParser():FBuilder(1024)
	{
		FReadPoint=FStringBuffer.Buffer();
	}

	VL_UniSepstrParser::~VL_UniSepstrParser()
	{
	}

	void VL_UniSepstrParser::SetString(VUnicodeString String)
	{
		FStringBuffer=String;
		FReadPoint=FStringBuffer.Buffer();
	}

	VL_UniSepstrParser::_Token VL_UniSepstrParser::GetToken(VBool& Exists)
	{
		_Token Token;
		Token.Info=vsiNormal;
		VBool Parsing=true;
		Exists=true;

		while(Parsing)
		{
			VWChar Char=*FReadPoint++;
			switch(Char)
			{
			case L'\r':
				if(*FReadPoint=='\n')
				{
					FReadPoint++;
				}
				Token.Info=vsiEOL;
				Parsing=false;
				break;
			case L'\n':
				Token.Info=vsiEOL;
				Parsing=false;
				break;
			case L' ':case L'\t':
				break;
			case L'\0':
				Token.Info=vsiEOS;
				FReadPoint--;
				Exists=false;
				Parsing=false;
			default:
				{
					VBool Bounded=Char==L'\"';

					if(Bounded)
					{
						VBool Reading=true;
						FBuilder.Clear();
						VWChar TempChar;
						while(Reading)
						{
							switch(TempChar=*FReadPoint++)
							{
							case L'\r':case L'\n':case L'\t':case L'\0':
								Reading=false;
								break;
							case L'\"':
								Reading=false;
								break;
							case L'\\':
								switch(TempChar=*FReadPoint++)
								{
								case L'\"':case L'\\':
									FBuilder.Append(&TempChar,1);
									break;
								case L'r':
									FBuilder.Append(L"\r",1);
									break;
								case L'n':
									FBuilder.Append(L"\n",1);
									break;
								case '\0':
									FReadPoint--;
									Reading=false;
									break;
								default:
									FReadPoint--;
									FBuilder.Append(L"\\",1);
								}
								break;
							default:
								FBuilder.Append(&TempChar,1);
							}
						}
						Token.Token=FBuilder.GetString();
					}
					else
					{
						PWChar Start=FReadPoint-1;
						PWChar End=0;
						VBool Reading=true;
						while(Reading)
						{
							switch(*FReadPoint++)
							{
							case L'\r':case L'\n':case L' ':case L'\t':case L'\0':case L'\"':
								End=--FReadPoint;
								Reading=false;
								break;
							}
						}
						Token.Token=VUnicodeString(Start,(VInt)(End-Start));
					}
					Parsing=false;
				}
			}
		}
		return Token;
	}

	}
}