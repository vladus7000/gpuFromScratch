#pragma once
#include "Rasterizer.hpp"

namespace gapi
{
	struct ScreenTriangle
	{
		P p1, p2, p3;
	};

	class ScanLineRasterizer : public Rasterizer
	{
	public:
		ScanLineRasterizer(Pipeline& pipeline);
		~ScanLineRasterizer() {}

		virtual void setup(ShaderIO* vertexData1, ShaderIO* vertexData2, ShaderIO* vertexData3, int w, int h) override;
		virtual void rasterize() override;

	private:
		//	 1
		//  / \
		// 2---3
		void rasterizeTriangleBottom(ScreenTriangle& s);
		
		// 1---2
		//  \ /
		//   3
		void rasterizeTriangleTop(ScreenTriangle& s);
		void rasterizeStraightLine(ScreenTriangle& s, int x1, int x2, int y, bool bottom);
		void coverageTest(P& p, ScreenTriangle& s, bool bottom);
		void convertFromNDC(P& p, ShaderIO& vertexData);
		bool onRight(float x, float y, float X1, float Y1, float X2, float Y2);

	private:
		ShaderIO* m_vertexData1;
		ShaderIO* m_vertexData2;
		ShaderIO* m_vertexData3;
		int m_w;
		int m_h;
		ScreenTriangle m_screenTriangle;
		ScreenTriangle m_screenTriangleO;
		ScreenTriangle m_screenTriangleBottom;
		ScreenTriangle m_screenTriangleTop;
		bool m_ccw;
		float m_area;
	};
}