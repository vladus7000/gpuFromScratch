#pragma once
#include "Rasterizer.hpp"

namespace gapi
{
	struct ScreenTriangle
	{
		P p1, p2, p3;
		//P _p1, _p2, _p3;
	};

	class ScanLineRasterizer : public Rasterizer
	{
	public:
		ScanLineRasterizer(Pipeline& pipeline);
		~ScanLineRasterizer() {}

		virtual void setup(ShaderIO& vertexData1, ShaderIO& vertexData2, ShaderIO& vertexData3, int w, int h) override;
		virtual void rasterize() override;

	private:
		//	 1
		//  / \
		// 2---3
		void rasterizeTriangleBottom(ScreenTriangle& s, ShaderIO& vertexData1, ShaderIO& vertexData2, ShaderIO& vertexData3);
		
		// 1---2
		//  \ /
		//   3
		void rasterizeTriangleTop(ScreenTriangle& s, ShaderIO& vertexData1, ShaderIO& vertexData2, ShaderIO& vertexData3);
		void rasterizeStraightLine(ScreenTriangle& s, float x1, float x2, int y, ShaderIO& vertexData1, ShaderIO& vertexData2, ShaderIO& vertexData3, bool bottom);
		void coverageTest(P& p, ScreenTriangle& s, bool bottom);
		void convertFromNDC(P& p, ShaderIO& vertexData);

	private:
		ShaderIO m_vertexData1;
		ShaderIO m_vertexData2;
		ShaderIO m_vertexData3;
		ShaderIO m_vertexData4;
		int m_w;
		int m_h;
		ScreenTriangle m_screenTriangle;
		ScreenTriangle m_screenTriangleO;
		ScreenTriangle m_screenTriangleBottom;
		ScreenTriangle m_screenTriangleTop;
	};
}