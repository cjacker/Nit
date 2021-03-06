#ifndef VL_DATA_STRING
#define VL_DATA_STRING

#include "VL_Data_Basic.h"

namespace vl
{
/*
	template<
		typename _Type ,
		_Type _Terminator ,
		_Type _TrimObject ,
		VInt(*_Poser)(const _Type* Sub , _Type* Self) ,
		void(*_Caser)(_Type* Buffer , VBool ToLower) ,
		VInt(*_ToInt)(const _Type* Buffer) ,
		VDouble(*_ToDouble)(const _Type* Buffer) ,
		_Type* (*_FromInt)(VInt Num , VInt& Len) ,
		_Type* (*_FromDouble)(VDouble* Num , VInt Digits , VInt& Len)
	>
	class VObjectString
	{
	public:
		typedef _Type _Element;
		typedef _Type*	_TypeBuffer;
		typedef const _Type* _ConstTypeBuffer;
		typedef VObjectString<_Type , _Terminator , _TrimObject , _Poser , _Caser , _ToInt , _ToDouble , _FromInt , _FromDouble> _String;
	public:
		VObjectString	();																构造空字符串
		VObjectString	(const _String& String);										从一个字符串构造新字符串
		VObjectString	(_ConstTypeBuffer String);										从0结束字符指针构造字符串
		VObjectString	(_ConstTypeBuffer String , VInt Length);						从指定长度的字符指针构造字符串，
		VObjectString	(_Type String);													从字符构造字符串
		VObjectString	(VInt Data);													从整数构造字符串
		VObjectString	(VDouble Data);													从浮点构造字符串
		~VObjectString	();
		_Type&			operator [] (VInt Index);										取出字符引用
		_Type			GetChar(VInt Index);											获得字符
		_String&		operator = (_String String);
		_String&		operator = (_ConstTypeBuffer String);
		void			operator += (_String String);
		void			operator += (_ConstTypeBuffer String);
		_String			operator + (_String String);
		_String			operator + (_ConstTypeBuffer String);
		friend _String	operator + (_ConstTypeBuffer String , _String String2);
		VBool			operator < (_ConstTypeBuffer String);
		VBool			operator > (_ConstTypeBuffer String);
		VBool			operator == (_ConstTypeBuffer String);
		VBool			operator <= (_ConstTypeBuffer String);
		VBool			operator >= (_ConstTypeBuffer String);
		VBool			operator != (_ConstTypeBuffer String);
		VBool			operator < (_String String);
		VBool			operator > (_String String);
		VBool			operator == (_String String);
		VBool			operator <= (_String String);
		VBool			operator >= (_String String);
		VBool			operator != (_String String);
		friend VBool	operator < (_ConstTypeBuffer String , const _String String2);
		friend VBool	operator > (_ConstTypeBuffer String , const _String String2);
		friend VBool	operator == (_ConstTypeBuffer String , const _String String2);
		friend VBool	operator <= (_ConstTypeBuffer String , const _String String2);
		friend VBool	operator >= (_ConstTypeBuffer String , const _String String2);
		friend VBool	operator != (_ConstTypeBuffer String , const _String String2);
		VInt			Length();														获得字符串长度
		_TypeBuffer		Buffer();														获得字符指针
		_String			SubString(VInt Index,VInt Count);								取其中的一段字符串
		_String			Left(VInt Count);												取左边的字符串
		_String			Right(VInt Count);												取右边的字符串
		_String			TrimLeft(_Type Char=_TrimObject);								返回删除左边连续指定字符的结果
		_String			TrimRight(_Type Char=_TrimObject);								返回删除右边连续指定字符的结果
		_String			Trim(_Type Char=_TrimObject);									返回删除两边连续指定字符串的结果
		_String			UpperCase();													返回所有字符转大写的结果
		_String			LowerCase();													返回所有字符转小写的结果
		void			Delete(VInt Index,VInt Count);									删除某段字符串
		void			Insert(VInt Index,_String S);									从指定的位置插入字符串
		VInt			Pos(const _String S);											寻找子字符串
		VInt			ToInt();														把字符串转变为整数
		VDouble			ToDouble();														把字符串转变为浮点
		void			FromInt(VInt Num);												把整数赋值给字符串
		void			FromDouble(VDouble Num , VInt Digits=50);						把浮点赋值给字符串
	};
*/
	template<
		typename _Type ,
		_Type _Terminator ,
		_Type _TrimObject ,
		VInt(*_Poser)(const _Type* Sub , _Type* Self) ,
		void(*_Caser)(_Type* Buffer , VBool ToLower) ,
		VInt(*_ToInt)(const _Type* Buffer) ,
		VDouble(*_ToDouble)(const _Type* Buffer) ,
		_Type* (*_FromInt)(VInt Num , VInt& Len) ,
		_Type* (*_FromDouble)(VDouble* Num , VInt Digits , VInt& Len)
	>
	class VObjectString
	{
	public:
		typedef _Type _Element;
		typedef _Type*	_TypeBuffer;
		typedef const _Type* _ConstTypeBuffer;
		typedef VObjectString<_Type , _Terminator , _TrimObject , _Poser , _Caser , _ToInt , _ToDouble , _FromInt , _FromDouble> _String;
	protected:
		VInt		FLength;
		_TypeBuffer	FBuffer;
		VInt*		FRefCount;

		static VInt Compare(_ConstTypeBuffer s1 , _ConstTypeBuffer s2)
		{
			if(s1)
			{
				if(s2)
				{
					while(*s1++==*s2++)
					{
						if(s1[-1]==_Terminator||s2[-1]==_Terminator)
						{
							break;
						}
					}
					return s1[-1]-s2[-1];
				}
				else
				{
					return *s1;
				}
			}
			else
			{
				if(s2)
				{
					return -*s2;
				}
				else
				{
					return 0;
				}
			}
		}

		void RefBuild(_TypeBuffer Buffer)
		{
			FRefCount=new VInt(1);
			FBuffer=Buffer;
			FLength=GetLen(Buffer);
		}

		void RefBuild(VInt StringLength)
		{
			if(StringLength)
			{
				FRefCount=new VInt(1);
				FBuffer=new _Type[StringLength+1];
				FBuffer[StringLength]=_Terminator;
				FLength=StringLength;
			}
			else
			{
				FRefCount=0;
				FBuffer=0;
				FLength=0;
			}
		}

		void RefIncrease()
		{
			if(FRefCount)
			{
				(*FRefCount)++;
			}
		}

		void RefDecrease()
		{
			if(FRefCount)
			{
				if(--(*FRefCount)==0)
				{
					delete FRefCount;
					delete[] FBuffer;
				}
			}
		}

		void RefWriteProtect()
		{
			if(FRefCount && *FRefCount>1)
			{
				(*FRefCount)--;
				_TypeBuffer OldBuffer=FBuffer;
				RefBuild(FLength);
				memcpy(FBuffer,OldBuffer,sizeof(_Type)*(FLength+1));
			}
		}

		static VInt GetLen(_ConstTypeBuffer String)
		{
			VInt Result=0;
			while(*String++!=_Terminator)
			{
				Result++;
			}
			return Result;
		}
	public:
		VObjectString()
		{
			RefBuild(0);
		}
		VObjectString(const _String& String)
		{
			FLength=String.FLength;
			FBuffer=String.FBuffer;
			FRefCount=String.FRefCount;
			RefIncrease();
		}
		VObjectString(_ConstTypeBuffer String)
		{
			FLength=GetLen(String);
			RefBuild(FLength);
			if(FRefCount)
			{
				memcpy(FBuffer,String,sizeof(_Type)*(FLength+1));
			}
		}
		VObjectString(_ConstTypeBuffer String , VInt Length)
		{
			RefBuild(Length);
			if(FRefCount)
			{
				memcpy(FBuffer,String,sizeof(_Type)*(FLength));
			}
		}
		VObjectString(_Type String)
		{
			RefBuild(1);
			FBuffer[0]=String;
		}
		VObjectString(VInt Data)
		{
			RefBuild(0);
			FromInt(Data);
		}
		VObjectString(VDouble Data)
		{
			RefBuild(0);
			FromDouble(Data);
		}
		~VObjectString()
		{
			RefDecrease();
		}

		_Type& operator [] (VInt Index)
		{
			RefWriteProtect();
			return FBuffer[Index];
		}

		_Type GetChar(VInt Index)const
		{
			return FBuffer[Index];
		}

		_String& operator = (const _String& String)
		{
			RefDecrease();
			FLength=String.FLength;
			FBuffer=String.FBuffer;
			FRefCount=String.FRefCount;
			RefIncrease();
			return *this;
		}
		_String& operator = (_ConstTypeBuffer String)
		{
			return operator = (_String(String));
		}

		void operator += (const _String& String)
		{
			if(FRefCount)
			{
				if(String.FRefCount)
				{
					_String TempString(*this);
					RefDecrease();
					RefBuild(FLength+String.FLength);
					memcpy(FBuffer,TempString.FBuffer,sizeof(_Type)*(TempString.FLength));
					memcpy(FBuffer+TempString.FLength,String.FBuffer,sizeof(_Type)*(String.FLength+1));
				}
			}
			else
			{
				operator = (String);
			}
		}
		void operator += (_ConstTypeBuffer String)
		{
			if(FRefCount)
			{
				_String TempString(*this);
				RefDecrease();
				VInt StringLength=GetLen(String);
				RefBuild(FLength+StringLength);
				memcpy(FBuffer,TempString.FBuffer,sizeof(_Type)*(TempString.FLength));
				memcpy(FBuffer+TempString.FLength,String,sizeof(_Type)*(StringLength+1));
			}
			else
			{
				operator = (String);
			}
		}

		_String operator + (const _String& String)const
		{
			if(FRefCount)
			{
				_String Temp(FBuffer);
				Temp+=String;
				return Temp;
			}
			else
			{
				return String;
			}
		}
		_String operator + (_ConstTypeBuffer String)const
		{
			if(FRefCount)
			{
				VInt StringLength=GetLen(String);
				_String Result;
				Result.RefDecrease();
				Result.RefBuild(FLength+StringLength);
				memcpy(Result.FBuffer,FBuffer,sizeof(_Type)*FLength);
				memcpy(Result.FBuffer+FLength,String,sizeof(_Type)*StringLength);
				return Result;
			}
			else
			{
				return _String(String);
			}
		}

		friend _String operator + (_ConstTypeBuffer String , const _String& String2)
		{
			if(String2.FRefCount)
			{
				VInt StringLength=GetLen(String);
				_String Result;
				Result.RefDecrease();
				Result.RefBuild(StringLength+String2.Length());
				memcpy(Result.FBuffer,String,sizeof(_Type)*StringLength);
				memcpy(Result.FBuffer+StringLength,String2.FBuffer,sizeof(_Type)*(String2.Length()+1));
				return Result;
			}
			else
			{
				return _String(String);
			}
		}

		VBool operator < (_ConstTypeBuffer String)const
		{
			return _String::Compare(FBuffer,String)<0;
		}
		VBool operator > (_ConstTypeBuffer String)const
		{
			return _String::Compare(FBuffer,String)>0;
		}
		VBool operator == (_ConstTypeBuffer String)const
		{
			return _String::Compare(FBuffer,String)==0;
		}
		VBool operator <= (_ConstTypeBuffer String)const
		{
			return _String::Compare(FBuffer,String)<=0;
		}
		VBool operator >= (_ConstTypeBuffer String)const
		{
			return _String::Compare(FBuffer,String)>=0;
		}
		VBool operator != (_ConstTypeBuffer String)const
		{
			return _String::Compare(FBuffer,String)!=0;
		}

		VBool operator < (const _String& String)const
		{
			return _String::Compare(FBuffer,String.FBuffer)<0;
		}
		VBool operator > (const _String& String)const
		{
			return _String::Compare(FBuffer,String.FBuffer)>0;
		}
		VBool operator == (const _String& String)const
		{
			return _String::Compare(FBuffer,String.FBuffer)==0;
		}
		VBool operator <= (const _String& String)const
		{
			return _String::Compare(FBuffer,String.FBuffer)<=0;
		}
		VBool operator >= (const _String& String)const
		{
			return _String::Compare(FBuffer,String.FBuffer)>=0;
		}
		VBool operator != (const _String& String)const
		{
			return _String::Compare(FBuffer,String.FBuffer)!=0;
		}

		friend VBool operator < (_ConstTypeBuffer String , const _String& String2)
		{
			return _String::Compare(String,String2.FBuffer)<0;
		}
		friend VBool operator > (_ConstTypeBuffer String , const _String& String2)
		{
			return _String::Compare(String,String2.FBuffer)>0;
		}
		friend VBool operator == (_ConstTypeBuffer String , const _String& String2)
		{
			return _String::Compare(String,String2.FBuffer)==0;
		}
		friend VBool operator <= (_ConstTypeBuffer String , const _String& String2)
		{
			return _String::Compare(String,String2.FBuffer)<=0;
		}
		friend VBool operator >= (_ConstTypeBuffer String , const _String& String2)
		{
			return _String::Compare(String,String2.FBuffer)>=0;
		}
		friend VBool operator != (_ConstTypeBuffer String , const _String& String2)
		{
			return _String::Compare(String,String2.FBuffer)!=0;
		}
		
		VInt Length()const
		{
			return FLength;
		}
		_TypeBuffer Buffer()const
		{
			if(FRefCount)
			{
				return FBuffer;
			}
			else
			{
				_String* Self=const_cast<_String*>(this);
				Self->FRefCount=new VInt(1);
				Self->FLength=0;
				Self->FBuffer=new _Type[1];
				Self->FBuffer[0]=_Terminator;
				return FBuffer;
			}
		}
		_String SubString(VInt Index,VInt Count)const
		{
			if(Index<0)
			{
				Count+=Index;
				Index=0;
			}
			else if(Index>=FLength)
			{
				return _String();
			}
			if(Count<0)
			{
				Count=0;
			}
			else if(Index+Count>FLength)
			{
				Count=FLength-Index;
			}
			return _String(FBuffer+Index,Count);
		}
		_String Left(VInt Count)const
		{
			if(Count<0)
			{
				Count=0;
			}
			else if(Count>FLength)
			{
				Count=FLength;
			}
			if(Count==FLength)
			{
				return *this;
			}
			else
			{
				return _String(FBuffer,Count);
			}
		}
		_String Right(VInt Count)const
		{
			if(Count<0)
			{
				Count=0;
			}
			else if(Count>FLength)
			{
				Count=FLength;
			}
			if(Count==FLength)
			{
				return *this;
			}
			else
			{
				return _String(FBuffer+FLength-Count,Count);
			}
		}
		_String TrimLeft(_Type Char=_TrimObject)const
		{
			if(FRefCount)
			{
				_TypeBuffer s=FBuffer;
				while(*s++==Char);
				return _String(s-1);
			}
			else
			{
				return *this;
			}
		}
		_String TrimRight(_Type Char=_TrimObject)const
		{
			if(FRefCount)
			{
				_TypeBuffer s=FBuffer+FLength-1;
				while(*s--==Char);
				return _String(FBuffer,(VInt)(s-FBuffer+2));
			}
			else
			{
				return *this;
			}
		}
		_String Trim(_Type Char=_TrimObject)const
		{
			if(FRefCount)
			{
				_TypeBuffer s=FBuffer;
				while(*s++==Char);
				s--;
				_TypeBuffer s2=FBuffer+FLength-1;
				if(s2>=s)
				{
					while(*s2--==Char);
					s2+=2;
					return _String(s,(VInt)(s2-s));
				}
				else
				{
					return _String(s,(VInt)(FBuffer+FLength-s));
				}
			}
			else
			{
				return *this;
			}
		}
		_String UpperCase()const
		{
			if(FRefCount && FLength>0)
			{
				_String s(FBuffer);
				_Caser(s.FBuffer,false);
				return s;
			}
			else
			{
				return *this;
			}
		}
		_String LowerCase()const
		{
			if(FRefCount && FLength>0)
			{
				_String s(FBuffer);
				_Caser(s.FBuffer,true);
				return s;
			}
			else
			{
				return *this;
			}
		}
		void Delete(VInt Index,VInt Count)
		{
			if(Index>=FLength)
			{
				return;
			}
			else if(Index<0)
			{
				Count+=Index;
				Index=0;
			}
			if(Count<0)
			{
				Count=0;
			}
			else if(Index+Count>FLength)
			{
				Count=FLength-Index;
			}
			if(Count)
			{
				_String Temp(*this);
				RefDecrease();
				VInt NewLength=FLength-Count;
				RefBuild(NewLength);
				memcpy(FBuffer,Temp.FBuffer,Index*sizeof(_Type));
				memcpy(FBuffer+Index,Temp.FBuffer+Index+Count,(NewLength-Index)*sizeof(_Type));
			}
		}
		void Insert(VInt Index , const _String& S)
		{
			if(FRefCount)
			{
				if(S.FRefCount)
				{
					if(Index<0)
					{
						Index=0;
					}
					else if(Index>FLength)
					{
						Index=FLength;
					}
					{
						_String Temp(*this);
						RefDecrease();
						VInt NewLength=FLength+S.FLength;
						RefBuild(NewLength);
						memcpy(FBuffer,Temp.FBuffer,Index*sizeof(_Type));
						memcpy(FBuffer+Index,S.FBuffer,S.FLength*sizeof(_Type));
						memcpy(FBuffer+Index+S.FLength,Temp.FBuffer+Index,(Temp.FLength-Index)*sizeof(_Type));
					}
				}
			}
			else
			{
				operator = (S);
			}
		}
		VInt Pos(const _String S)const
		{
			if(FLength==0||S.FLength==0)
			{
				return -1;
			}
			else
			{
				return _Poser(S.FBuffer,FBuffer);
			}
		}
		VInt ToInt()const
		{
			if(FRefCount)
			{
				return _ToInt(FBuffer);
			}
			else
			{
				return 0;
			}
		}
		VDouble ToDouble()const
		{
			if(FRefCount)
			{
				return _ToDouble(FBuffer);
			}
			else
			{
				return 0.0f;
			}
		}
		void FromInt(VInt Num)
		{
			RefDecrease();
			FRefCount=new VInt(1);
			FBuffer=_FromInt(Num,FLength);
		}
		void FromDouble(VDouble Num , VInt Digits=50)
		{
			RefDecrease();
			FRefCount=new VInt(1);
			FBuffer=_FromDouble(&Num,Digits,FLength);
		}
	};

	extern VInt MbcsPoser(PCChar Sub , PChar Self);
	extern void MbcsCaser(PChar Sub , VBool ToLower);
	extern VInt MbcsToInt(PCChar Buffer);
	extern VDouble MbcsToDouble(PCChar Buffer);
	extern PChar MbcsFromInt(VInt Num , VInt& Len);
	extern PChar MbcsFromDouble(VDouble* Num , VInt Digits , VInt& Len);

	extern VInt UnicodePoser(PCWChar Sub , PWChar Self);
	extern void UnicodeCaser(PWChar Sub , VBool ToLower);
	extern VInt UnicodeToInt(PCWChar Buffer);
	extern VDouble UnicodeToDouble(PCWChar Buffer);
	extern PWChar UnicodeFromInt(VInt Num , VInt& Len);
	extern PWChar UnicodeFromDouble(VDouble* Num , VInt Digits , VInt& Len);

	typedef VObjectString<VChar  , '\0'  , ' '  , 
		MbcsPoser   , MbcsCaser    , 
		MbcsToInt   , MbcsToDouble ,
		MbcsFromInt , MbcsFromDouble> VMbcsString;
	typedef VObjectString<VWChar , L'\0' , L' ' , 
		UnicodePoser   , UnicodeCaser , 
		UnicodeToInt   , UnicodeToDouble ,
		UnicodeFromInt , UnicodeFromDouble> VUnicodeString;

	extern VMbcsString ToMbcs(VUnicodeString String);
	extern VUnicodeString ToUnicode(VMbcsString String);
	/*extern istream& operator>>(istream& i,VMbcsString& S);
	extern istream& operator>>(istream& i,VUnicodeString& S);
	extern ostream& operator<<(ostream& o,VMbcsString S);
	extern ostream& operator<<(ostream& o,VUnicodeString S);*/

#ifdef _UNICODE

	typedef VWChar			ApiChar;
	typedef PWChar			PApiChar;
	typedef VUnicodeString	VSysString;
	#define apipch(c)		(L##c)
	#define papilen(c)		wcslen(c)
	#define apistr(sys)		(sys).Buffer()
	#define apitou(sys)		(sys)
	#define apilen(sys)		(sys).Length()

#else

	typedef VChar			ApiChar;
	typedef PChar			PApiChar;
	typedef VMbcsString		VSysString;
	#define apipch(c)		(c)
	#define papiplen(c)		strlen(c)
	#define apistr(sys)		ToMbcs(sys).Buffer()
	#define apitou(sys)		ToUnicode(sys)
	#define apilen(sys)		ToMbcs(sys).Length()

#endif
}

#endif