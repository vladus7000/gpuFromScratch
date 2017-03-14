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

	inline void convertFromNDC(P& screenPoint, ShaderIO & vertexData, int w, int h)
	{
		screenPoint.realX = ((vertexData.data[0].x + 1.0f) / 2.0f * ((float)w)); // -1.0f
		screenPoint.x = std::lroundf(screenPoint.realX);

		screenPoint.realY = ((vertexData.data[0].y + 1.0f) / 2.0f * ((float)h)); // -1.0f
		screenPoint.y = std::lroundf(screenPoint.realY);
	}
}