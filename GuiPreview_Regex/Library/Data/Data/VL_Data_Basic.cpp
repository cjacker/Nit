#include "VL_Data_Basic.h"

namespace vl
{
	const VFloat Pi_f=3.1415927f;

	VL_ArrayDimension<vacNewArray , 0>	NewArray;
	
	VInt Round(VDouble Num)
	{
		VInt c=(VInt)ceil(Num);
		VInt f=(VInt)floor(Num);
		return fabs(c-Num)<=fabs(f-Num)?c:f;
	}

	VInt Trunc(VDouble Num)
	{
		if(Num>=0)
		{
			return (VInt)floor(Num);
		}
		else
		{
			return (VInt)ceil(Num);
		}
	}

	VDouble Roundf(VDouble Num)
	{
		VDouble c=ceil(Num);
		VDouble f=floor(Num);
		return fabs(c-Num)<=fabs(f-Num)?c:f;
	}

	VDouble Truncf(VDouble Num)
	{
		if(Num>=0)
		{
			return floor(Num);
		}
		else
		{
			return ceil(Num);
		}
	}
}