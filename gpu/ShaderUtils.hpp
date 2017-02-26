#pragma once
#include "Math.hpp"

namespace gapi
{
	struct PSOutput
	{
		Point4 colorBuffer;
		float outZ;
	};

	struct ShaderIO
	{
		static const int dataCount = 8;
		Point4 data[dataCount];
	};
}