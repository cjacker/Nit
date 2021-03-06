/*******************************************************************************
Vczh Library++ 2.0
语法工具::有穷状态自动机
开发者：陈梓瀚

接口：
类：
  VLS_RegEdgeInstruction		：边指令
  VLS_RegEdgeReplicator			：循环记录
  VL_RegNFA						：非确定性有穷状态自动机
  VL_RegFAInfo					：正则表达式自动机
  VL_RegDFA						：确定性有穷状态自动机
函数：
  CopyEpsilonNFA				：复制Epsilon-NFA
  EpsilonNFAToNFA				：将EpsilonNFA转换成NFA

  CompressNFA					：压缩NFA
  RegularExpressionToNFA		：从正则表达式构造若干NFA
  CompressedNFAToDFA			：把NFA转换成DFA

  NFAToString					：把NFA转换成字符串
  DFAToString					：把DFA转换成字符串
  RegularExpressionToENFA		：从正则表达式构造若干Epsilon-NFA
*******************************************************************************/

#ifndef VL_REGFA
#define VL_REGFA

#include "VL_Automaton.h"
#include "VL_Regexp.h"
#include "..\Data\VL_Data_Map.h"

namespace vl
{
	namespace grammar
	{

/*********************************************************************************************************
非确定性有穷状态自动机
*********************************************************************************************************/

		enum VLE_RegEdgeType		/*边类别*/
		{
			vretEpsilon,
			vretMatch,				/*Parameter=Storage ID*/
			vretCharset,			/*Parameter=Char Class*/
			vretStringBegin,
			vretStringEnd,
			vretPositive,			/*Parameter=Automaton ID*/
			vretNegative			/*Parameter=Automaton ID*/
		};
		enum VLE_RegEdgeInstruction	/*构造匹配结果的指令*/
		{
			vreiCatch,
			vreiStorage,			/*Parameter=Storage ID*/
			vreiPass,				/*Parameter=Character Count*/
			vreiMatch,				/*Parameter=Storage ID*/
			vreiEnd,
			vreiLongLoop,			/*Parameter=Replicator ID*/
			vreiShortLoop,			/*Parameter=Replicator ID*/
			vreiEndLoop
		};
		enum VLE_RegEdgeReplicator
		{
			vrerLong,
			vrerShort
		};

		struct VLS_RegEdgeInstruction
		{
			VLE_RegEdgeInstruction	Instruction;
			VInt					Parameter;
		};
		typedef VL_List<VLS_RegEdgeInstruction , true>	VL_RegEdgeInsList;

		struct VLS_RegNFAEdgeData
		{
			VL_RegEdgeInsList		Instructions;
			VLE_RegEdgeType			Action;
			VInt					Parameter;
		};
		struct VLS_RegNFAStatusData
		{
			VBool					FinalStatus;
		};

		class VL_RegNFA : public VL_Automaton<VLS_RegNFAEdgeData , VLS_RegNFAStatusData>
		{
		public:

			class EpsilonNFA : public VL_Base
			{
			public:
				Status*				InitStatus;
				Status*				FinalStatus;

				EpsilonNFA();
			};

		public:
			VL_RegNFA(VInt PackSize=1024);

			void					Init(Edge* Data);
			void					Init(Status* Data);
		};

/*********************************************************************************************************
确定性有穷状态自动机
*********************************************************************************************************/

		struct VLS_RegDFAEdgeData
		{
			typedef VL_List<VL_RegNFA::Edge* , true>	NFAEdgeList;
		public:
			VLE_RegEdgeType			Action;
			VInt					Parameter;
			NFAEdgeList				NFAEdges;
		};

		struct VLS_RegDFAStatusData
		{
			typedef VL_OrderedList<VL_RegNFA::Status* , true>	NFAStatusList;
		public:
			VBool					FinalStatus;
			NFAStatusList			NFAStatus;
		};

		class VL_RegDFA : public VL_Automaton<VLS_RegDFAEdgeData , VLS_RegDFAStatusData>
		{
		public:
			VL_RegDFA(VInt PackSize=1024);

			void					Init(Edge* Data);
			void					Init(Status* Data);
		};

/*********************************************************************************************************
工具
*********************************************************************************************************/

		class RegNFAClosureAlgorithm : public VL_RegNFA::IBFSAlgorithm
		{
		public:
			enum ClosureType
			{
				ctAll,
				ctEpsilon,
				ctNoRead
			} ClosureTest;

			VBool EdgeTest(VL_RegNFA::Edge* aEdge);
		};

		class RegDFAClosureAlgorithm : public VL_RegDFA::IBFSAlgorithm
		{
		public:
			enum ClosureType
			{
				ctAll,
			} ClosureTest;

			VBool EdgeTest(VL_RegDFA::Edge* aEdge);
		};

		class VL_RegFAInfo : public VL_Base
		{
			typedef VL_List<VUnicodeString , false>				StringList;
			typedef VL_ListedMap<VUnicodeString , VL_RegExpPtr>	ExpressionMap;
			typedef VL_List<VL_RegNFA::EpsilonNFA , true>		EpsilonNFAList;
			typedef VL_List<VL_RegNFA::Status* , true>			NFAList;
		public:
			class EpsilonNFABag
			{
			public:
				VL_RegNFA::EpsilonNFA	MainEpsilonNFA;
				EpsilonNFAList			SubEpsilonNFAs;
			};
		public:
			StringList				StorageIDs;				/*存储管道名称表*/
			ExpressionMap			NamedExpression;		/*命名子表达式表*/
			VL_RegExpCharClass		CharClass;				/*字符类*/
			VL_RegExpPtr			Expression;				/*正则表达式*/

			VL_RegNFA::Status*		MainNFA;				/*主要NFA*/
			NFAList					SubNFAs;				/*附加NFA*/

			VL_RegFAInfo(VL_RegExpPtr RegularExpression , VL_RegExpCharClass* PrecalculatedCharClass=0);
		};

		extern VL_RegNFA::EpsilonNFA			CopyEpsilonNFA(VL_RegNFA* Automaton , VL_RegNFA::EpsilonNFA EpsilonNFA);
		extern VL_RegFAInfo::EpsilonNFABag		RegularExpressionToEpsilonNFA(VL_RegNFA* Automaton , VL_RegFAInfo* Info);

		extern VL_RegNFA::Status*				EpsilonNFAToNFA(VL_RegNFA* Automaton , VL_RegNFA::EpsilonNFA NFA);
		extern VL_RegNFA::Status*				CompressNFA(VL_RegNFA* Automaton , VL_RegNFA::Status* NFA);
		extern VL_RegDFA::Status*				CompressedNFAToDFA(VL_RegDFA* Automaton , VL_RegNFA::Status* NFA);

		extern VUnicodeString					NFAToString(VL_RegFAInfo* Info , VL_RegNFA::Status* NFA);
		extern VUnicodeString					DFAToString(VL_List<VUnicodeString , false>* StorageIDs , VL_RegExpCharClass* CharClass , VL_RegDFA::Status* DFA);
		extern VUnicodeString					DFAToString(VL_RegFAInfo* Info , VL_RegDFA::Status* DFA);
		extern void								RegularExpressionToNFA(VL_RegNFA* Automaton , VL_RegFAInfo* Info , VBool CompressMain);
	}
}

#endif