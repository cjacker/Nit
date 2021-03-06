/*******************************************************************************
Vczh Library++ 2.0
数据结构::事件处理
开发者：陈梓瀚

接口：
类：
函数：
注意：
    在使用这个文件定义的宏的那个地点必须可以直接访问到vl::collection::VL_List
*******************************************************************************/

#ifndef VL_DATA_EVENT
#define VL_DATA_EVENT

namespace vl
{
#define VL_DEFINE_UNVOID_EVENT(NAME,RETURN,NAMES,PARAMS)									\
																							\
	class IVL_EVENT_##NAME : public VL_Base													\
	{																						\
	public:																					\
		virtual RETURN Invoke PARAMS =0;													\
		virtual void GetElement(void*& A , void*& B)=0;										\
	};																						\
																							\
	class VL_EVENT_NORMAL_##NAME : public IVL_EVENT_##NAME									\
	{																						\
	public:																					\
		typedef RETURN (*Handler) PARAMS ;													\
	protected:																				\
		Handler FHandler;																	\
	public:																					\
		VL_EVENT_NORMAL_##NAME(Handler h);													\
		RETURN Invoke PARAMS;																\
		void GetElement(void*& A , void*& B);												\
	};																						\
																							\
	template<typename Class>																\
	class VL_EVENT_MEMBER_##NAME : public IVL_EVENT_##NAME									\
	{																						\
	public:																					\
		typedef RETURN (Class::*Handler) PARAMS ;											\
	protected:																				\
		Class* FOwner;																		\
		Handler FHandler;																	\
	public:																					\
		VL_EVENT_MEMBER_##NAME(Class* o , Handler h)										\
		{																					\
			FOwner=o;																		\
			FHandler=h;																		\
		}																					\
		RETURN Invoke PARAMS																\
		{																					\
			return (FOwner->*FHandler) NAMES ;												\
		}																					\
		void GetElement(void*& A , void*& B)												\
		{																					\
			A=(void*)FOwner;																\
			B=(void*)(*(VInt*)(&FHandler));													\
		}																					\
	};																						\
																							\
	class NAME : public VL_Base																\
	{																						\
	protected:																				\
		typedef VL_List<IVL_EVENT_##NAME* , true> HandlerList;								\
																							\
		HandlerList FHandlers;																\
																							\
		VInt Find(IVL_EVENT_##NAME* Handler);												\
		void Bind(IVL_EVENT_##NAME* Handler);												\
		void Unbind(IVL_EVENT_##NAME* Handler);												\
	public:																					\
		NAME();																				\
		~NAME();																			\
		void Bind(VL_EVENT_NORMAL_##NAME::Handler Handler);									\
		void Unbind(VL_EVENT_NORMAL_##NAME::Handler Handler);								\
		template<typename Class>															\
		void Bind(Class* Owner , typename VL_EVENT_MEMBER_##NAME<Class>::Handler Handler)	\
		{																					\
			Bind(new VL_EVENT_MEMBER_##NAME<Class>(Owner,Handler));							\
		}																					\
		template<typename Class>															\
		void Unbind(Class* Owner , typename VL_EVENT_MEMBER_##NAME<Class>::Handler Handler)	\
		{																					\
			Unbind(new VL_EVENT_MEMBER_##NAME<Class>(Owner,Handler));						\
		}																					\
		RETURN operator() PARAMS;															\
	};

#define VL_IMPLEMENT_UNVOID_EVENT(NAME,RETURN,NAMES,PARAMS)									\
																							\
	VL_EVENT_NORMAL_##NAME::VL_EVENT_NORMAL_##NAME(Handler h)								\
	{																						\
		FHandler=h;																			\
	}																						\
	RETURN VL_EVENT_NORMAL_##NAME::Invoke PARAMS											\
	{																						\
		return FHandler NAMES ;																\
	}																						\
	void VL_EVENT_NORMAL_##NAME::GetElement(void*& A , void*& B)							\
	{																						\
		A=0;																				\
		B=(void*)(FHandler);																\
	}																						\
																							\
	VInt NAME::Find(IVL_EVENT_##NAME* Handler)												\
	{																						\
		void* OldA;																			\
		void* OldB;																			\
		void* NewA;																			\
		void* NewB;																			\
		Handler->GetElement(NewA,NewB);														\
		for(VInt i=0;i<FHandlers.GetCount();i++)											\
		{																					\
			FHandlers[i]->GetElement(OldA,OldB);											\
			if((NewA==OldA) && (NewB==OldB))												\
			{																				\
				return i;																	\
			}																				\
		}																					\
		return -1;																			\
	}																						\
	void NAME::Bind(IVL_EVENT_##NAME* Handler)												\
	{																						\
		VInt Index=Find(Handler);															\
		if(Index!=-1)																		\
			delete Handler;																	\
		else																				\
			FHandlers.Add(Handler);															\
	}																						\
	void NAME::Unbind(IVL_EVENT_##NAME* Handler)											\
	{																						\
		VInt Index=Find(Handler);															\
		if(Index!=-1)																		\
			delete FHandlers.Fetch(Index);													\
		delete Handler;																		\
	}																						\
	NAME::NAME()																			\
	{																						\
	}																						\
	NAME::~NAME()																			\
	{																						\
		for(VInt i=0;i<FHandlers.GetCount();i++)											\
		{																					\
			delete FHandlers[i];															\
		}																					\
	}																						\
	void NAME::Bind(VL_EVENT_NORMAL_##NAME::Handler Handler)								\
	{																						\
		Bind(new VL_EVENT_NORMAL_##NAME(Handler));											\
	}																						\
	void NAME::Unbind(VL_EVENT_NORMAL_##NAME::Handler Handler)								\
	{																						\
		Unbind(new VL_EVENT_NORMAL_##NAME(Handler));										\
	}																						\
	RETURN NAME::operator() PARAMS															\
	{																						\
		RETURN Result=RETURN();																\
		for(VInt i=0;i<FHandlers.GetCount();i++)											\
		{																					\
			Result=FHandlers[i]->Invoke NAMES ;												\
		}																					\
		return Result;																		\
	}

#define VL_DEFINE_VOID_EVENT(NAME,NAMES,PARAMS)												\
																							\
	class IVL_EVENT_##NAME : public VL_Base													\
	{																						\
	public:																					\
		virtual void Invoke PARAMS =0;														\
		virtual void GetElement(void*& A , void*& B)=0;										\
	};																						\
																							\
	class VL_EVENT_NORMAL_##NAME : public IVL_EVENT_##NAME									\
	{																						\
	public:																					\
		typedef void (*Handler) PARAMS ;													\
	protected:																				\
		Handler FHandler;																	\
	public:																					\
		VL_EVENT_NORMAL_##NAME(Handler h);													\
		void Invoke PARAMS;																	\
		void GetElement(void*& A , void*& B);												\
	};																						\
																							\
	template<typename Class>																\
	class VL_EVENT_MEMBER_##NAME : public IVL_EVENT_##NAME									\
	{																						\
	public:																					\
		typedef void (Class::*Handler) PARAMS ;												\
	protected:																				\
		Class* FOwner;																		\
		Handler FHandler;																	\
	public:																					\
		VL_EVENT_MEMBER_##NAME(Class* o , Handler h)										\
		{																					\
			FOwner=o;																		\
			FHandler=h;																		\
		}																					\
		void Invoke PARAMS																	\
		{																					\
			(FOwner->*FHandler) NAMES ;														\
		}																					\
		void GetElement(void*& A , void*& B)												\
		{																					\
			A=(void*)FOwner;																\
			B=(void*)(*(VInt*)(&FHandler));													\
		}																					\
	};																						\
																							\
	class NAME : public VL_Base																\
	{																						\
	protected:																				\
		typedef VL_List<IVL_EVENT_##NAME* , true> HandlerList;								\
																							\
		HandlerList FHandlers;																\
																							\
		VInt Find(IVL_EVENT_##NAME* Handler);												\
		void Bind(IVL_EVENT_##NAME* Handler);												\
		void Unbind(IVL_EVENT_##NAME* Handler);												\
	public:																					\
		NAME();																				\
		~NAME();																			\
		void Bind(VL_EVENT_NORMAL_##NAME::Handler Handler);									\
		void Unbind(VL_EVENT_NORMAL_##NAME::Handler Handler);								\
		template<typename Class>															\
		void Bind(Class* Owner , typename VL_EVENT_MEMBER_##NAME<Class>::Handler Handler)	\
		{																					\
			Bind(new VL_EVENT_MEMBER_##NAME<Class>(Owner,Handler));							\
		}																					\
		template<typename Class>															\
		void Unbind(Class* Owner , typename VL_EVENT_MEMBER_##NAME<Class>::Handler Handler)	\
		{																					\
			Unbind(new VL_EVENT_MEMBER_##NAME<Class>(Owner,Handler));						\
		}																					\
		void operator() PARAMS;																\
	};

#define VL_IMPLEMENT_VOID_EVENT(NAME,NAMES,PARAMS)											\
																							\
	VL_EVENT_NORMAL_##NAME::VL_EVENT_NORMAL_##NAME(Handler h)								\
	{																						\
		FHandler=h;																			\
	}																						\
	void VL_EVENT_NORMAL_##NAME::Invoke PARAMS												\
	{																						\
		FHandler NAMES ;																	\
	}																						\
	void VL_EVENT_NORMAL_##NAME::GetElement(void*& A , void*& B)							\
	{																						\
		A=0;																				\
		B=(void*)(FHandler);																\
	}																						\
																							\
	VInt NAME::Find(IVL_EVENT_##NAME* Handler)												\
	{																						\
		void* OldA;																			\
		void* OldB;																			\
		void* NewA;																			\
		void* NewB;																			\
		Handler->GetElement(NewA,NewB);														\
		for(VInt i=0;i<FHandlers.GetCount();i++)											\
		{																					\
			FHandlers[i]->GetElement(OldA,OldB);											\
			if((NewA==OldA) && (NewB==OldB))												\
			{																				\
				return i;																	\
			}																				\
		}																					\
		return -1;																			\
	}																						\
	void NAME::Bind(IVL_EVENT_##NAME* Handler)												\
	{																						\
		VInt Index=Find(Handler);															\
		if(Index!=-1)																		\
			delete Handler;																	\
		else																				\
			FHandlers.Add(Handler);															\
	}																						\
	void NAME::Unbind(IVL_EVENT_##NAME* Handler)											\
	{																						\
		VInt Index=Find(Handler);															\
		if(Index!=-1)																		\
			delete FHandlers.Fetch(Index);													\
		delete Handler;																		\
	}																						\
	NAME::NAME()																			\
	{																						\
	}																						\
	NAME::~NAME()																			\
	{																						\
		for(VInt i=0;i<FHandlers.GetCount();i++)											\
		{																					\
			delete FHandlers[i];															\
		}																					\
	}																						\
	void NAME::Bind(VL_EVENT_NORMAL_##NAME::Handler Handler)								\
	{																						\
		Bind(new VL_EVENT_NORMAL_##NAME(Handler));											\
	}																						\
	void NAME::Unbind(VL_EVENT_NORMAL_##NAME::Handler Handler)								\
	{																						\
		Unbind(new VL_EVENT_NORMAL_##NAME(Handler));										\
	}																						\
	void NAME::operator() PARAMS															\
	{																						\
		for(VInt i=0;i<FHandlers.GetCount();i++)											\
		{																					\
			FHandlers[i]->Invoke NAMES ;													\
		}																					\
	}
}

#endif