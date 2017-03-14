#pragma once
#include "Rasterizer.hpp"

namespace gapi
{
	class TiledRasterizer : public Rasterizer
	{
		struct ScreenTriangle
		{
			P p1, p2, p3;
		};
	public:
		TiledRasterizer(Pipeline& pipeline);
		~TiledRasterizer() {}

		virtual void setup(ShaderIO* vertexData1, ShaderIO* vertexData2, ShaderIO* vertexData3, int w, int h) override;
		virtual void rasterize() override;

	private:
		void coverageTest(P& screenPoint, ScreenTriangle& s);
		bool onRight(float x, float y, float X1, float Y1, float X2, float Y2);

		ShaderIO* m_vertexData1;
		ShaderIO* m_vertexData2;
		ShaderIO* m_vertexData3;
		int m_w;
		int m_h;
		ScreenTriangle m_screenTriangle;
		ScreenTriangle m_screenTriangleO;

		bool m_ccw;
		float m_area;
		int minX, maxX;
		int minY, maxY;
	};
}