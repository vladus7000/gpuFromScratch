#pragma once

#include "PipeLine.hpp"

namespace gapi
{
	class Rasterizer
	{
	public:
		Rasterizer(Pipeline& pipeline)
			: m_pipeLine(pipeline)
		{}
		virtual ~Rasterizer() {}

		virtual void setup(ShaderIO* p1, ShaderIO* p2, ShaderIO* p3, int w, int h) = 0;
		virtual void rasterize() = 0;
	protected:
		Pipeline& m_pipeLine;
	};
}