#include "ScanLineRasterizer.hpp"
#include <algorithm>
#include <iostream>
namespace gapi
{
	ScanLineRasterizer::ScanLineRasterizer(Pipeline& pipeline)
		: Rasterizer(pipeline)
	{}

	void ScanLineRasterizer::setup(ShaderIO& p1, ShaderIO& p2, ShaderIO& p3, int w, int h)
	{
		m_vertexData1 = p1;
		m_vertexData2 = p2;
		m_vertexData3 = p3;
		m_w = w;
		m_h = h;

		convertFromNDC(m_screenTriangle.p1, m_vertexData1);
		convertFromNDC(m_screenTriangle.p2, m_vertexData2);
		convertFromNDC(m_screenTriangle.p3, m_vertexData3);

		m_screenTriangleO = m_screenTriangle;
	//	m_s._p1 = m_s.p1;
	//	m_s._p2 = m_s.p2;
	//	m_s._p3 = m_s.p3;
	}

	void ScanLineRasterizer::rasterize()
	{
		// from: http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
		if (m_screenTriangle.p1.y < m_screenTriangle.p2.y)
		{
		//	std::swap(m_vertexData1, m_vertexData2);
			std::swap(m_screenTriangle.p1, m_screenTriangle.p2);
		}
		if (m_screenTriangle.p2.y < m_screenTriangle.p3.y)
		{
		//	std::swap(m_vertexData2, m_vertexData3);
			std::swap(m_screenTriangle.p2, m_screenTriangle.p3);
		}
		if (m_screenTriangle.p1.y < m_screenTriangle.p2.y)
		{
		//	std::swap(m_vertexData1, m_vertexData2);
			std::swap(m_screenTriangle.p1, m_screenTriangle.p2);
		}

		if (m_screenTriangle.p2.y == m_screenTriangle.p3.y)
		{
			/*if (m_screenTriangle.p2.realX > m_screenTriangle.p3.realX)
			{
				std::swap(m_vertexData2, m_vertexData3);
				std::swap(m_screenTriangle.p3, m_screenTriangle.p2);
			}*/
			rasterizeTriangleBottom(m_screenTriangle, m_vertexData1, m_vertexData2, m_vertexData3);
		}
		else if (m_screenTriangle.p1.y == m_screenTriangle.p2.y)
		{
			/*if (m_screenTriangle.p1.realX > m_screenTriangle.p2.realX)
			{
				std::swap(m_vertexData1, m_vertexData2);
				std::swap(m_screenTriangle.p1, m_screenTriangle.p2);
			}*/
			rasterizeTriangleTop(m_screenTriangle, m_vertexData1, m_vertexData2, m_vertexData3);
		}
		else
		{
			P newPoint;
			newPoint.realX = m_screenTriangle.p1.realX + (float)(m_screenTriangle.p2.realY - m_screenTriangle.p1.realY) / (float)(m_screenTriangle.p3.realY - m_screenTriangle.p1.realY)*(float)(m_screenTriangle.p3.realX - m_screenTriangle.p1.realX);
			newPoint.realY = m_screenTriangle.p2.realY;
	
			float a = 0, b = 0, c = 0;
			Barycentric(Point2(newPoint.realX, newPoint.realY), Point2(m_screenTriangleO.p1.realX, m_screenTriangleO.p1.realY), Point2(m_screenTriangleO.p2.realX, m_screenTriangleO.p2.realY), Point2(m_screenTriangleO.p3.realX, m_screenTriangleO.p3.realY), a, b, c);

			newPoint.x = std::lroundf(newPoint.realX);// getFromBarycentric2(m_screenTriangle.p1.realX, m_screenTriangle.p2.realX, m_screenTriangle.p3.realX, a, b, c);
			newPoint.y = std::lroundf(newPoint.realY);// getFromBarycentric2(m_screenTriangle.p1.realY, m_screenTriangle.p2.realY, m_screenTriangle.p3.realY, a, b, c);



			for (int i = 0; i < ShaderIO::dataCount; i++)
			{
				m_vertexData4.data[i] = getFromBarycentric2(m_vertexData1.data[i], m_vertexData2.data[i], m_vertexData3.data[i], a, b, c);
			}
			
			m_screenTriangleBottom = m_screenTriangle;
			m_screenTriangleBottom.p3 = newPoint;
			if (m_screenTriangleBottom.p2.realX > m_screenTriangleBottom.p3.realX)
			{
				std::swap(m_screenTriangleBottom.p3, m_screenTriangleBottom.p2);
			//	std::swap(m_vertexData4, m_vertexData2); //TODO: same check
			}

			//rasterizeTriangleBottom(m_screenTriangleBottom, m_vertexData4, m_vertexData2, m_vertexData3);
			rasterizeTriangleBottom(m_screenTriangleBottom, m_vertexData1, m_vertexData2, m_vertexData3);
			m_screenTriangleTop = m_screenTriangle;
			m_screenTriangleTop.p1 = m_screenTriangle.p2;
			m_screenTriangleTop.p2 = newPoint;
			if (m_screenTriangleTop.p1.realX > m_screenTriangleTop.p2.realX)
			{
				//std::swap(m_vertexData4, m_vertexData2); //TODO: same check
				std::swap(m_screenTriangleTop.p1, m_screenTriangleTop.p2);
			}
			//rasterizeTriangleTop(m_screenTriangleTop, m_vertexData2, m_vertexData4, m_vertexData3);
			rasterizeTriangleTop(m_screenTriangleTop, m_vertexData1, m_vertexData2, m_vertexData3);
		}
	}

	//	 1
	//  / \
	// 2---3
	void ScanLineRasterizer::rasterizeTriangleBottom(ScreenTriangle& s, ShaderIO& p1, ShaderIO& p2, ShaderIO& p3)
	{
		float invSlope1 = -(float)(s.p2.x - s.p1.x) / (float)(s.p2.y - s.p1.y);
		float invSlope2 = -(float)(s.p3.x - s.p1.x) / (float)(s.p3.y - s.p1.y);

		float curX = s.p1.x;
		float curX2 = s.p1.x;
		for (int slY = std::floorf(s.p1.realY); slY >= std::ceilf(s.p2.realY); slY--)
		{
			rasterizeStraightLine(s, (curX), (curX2), slY, p1, p2, p3, true);
			curX += invSlope1;
			curX2 += invSlope2;
		}
	}

	// 1---2
	//  \ /
	//   3
	void ScanLineRasterizer::rasterizeTriangleTop(ScreenTriangle& s, ShaderIO& p1, ShaderIO& p2, ShaderIO& p3)
	{
		float invSlope1 = -(float)(s.p3.x - s.p1.x) / (float)(s.p3.y - s.p1.y);
		float invSlope2 = -(float)(s.p3.x - s.p2.x) / (float)(s.p3.y - s.p2.y);

		float curX = s.p3.x;
		float curX2 = s.p3.x;

		for (int slY = std::floorf(s.p3.realY); slY < std::ceilf(s.p1.realY); slY++)
		{
			rasterizeStraightLine(s, (curX), (curX2), slY, p1, p2, p3, false);
			curX -= invSlope1;
			curX2 -= invSlope2;
		}
	}

	void ScanLineRasterizer::rasterizeStraightLine(ScreenTriangle& s, float x1, float x2, int y, ShaderIO& p1, ShaderIO& p2, ShaderIO& p3, bool bottom)
	{
		ShaderIO psInput;
		if (x2 < x1)
		{
			std::swap(x1, x2);
		}
		for (int i = std::lroundf(x1); i <= std::lroundf(x2); i ++)
		{
			if (i < 0 || i > m_w || y < 0 || y > m_h)
			{
				continue;
			}

			P p;
			p.x = i;// lroundf(i);
			p.y = y;
			p.realX = i;
			p.realY = y;

			float a = 0, b = 0, c = 0;
			//Barycentric(P(p.realX, p.realY), P(s.p1.realX, s.p1.realY), P(s.p2.realX, s.p2.realY), P(s.p3.realX, s.p3.realY), a, b, c);
			Barycentric(P(p.realX, p.realY), P(m_screenTriangleO.p1.realX, m_screenTriangleO.p1.realY), P(m_screenTriangleO.p2.realX, m_screenTriangleO.p2.realY), P(m_screenTriangleO.p3.realX, m_screenTriangleO.p3.realY), a, b, c);

			coverageTest(p, s, bottom); // TODO:!!!
			if (p.needShade || 0)
			{
				for (int i = 0; i < ShaderIO::dataCount; i++)
				{
					psInput.data[i] = getFromBarycentric2(p1.data[i], p2.data[i], p3.data[i], a, b, c);
				}

				int screenX = i;
				int screenY = y;

				PSOutput out;
				out.outZ = psInput.data[0].z;
				m_pipeLine.invokePixelShader(screenX, screenY, psInput, out);

				if (m_pipeLine.depthTest(screenX, screenY, out.outZ))
				{
					m_pipeLine.blener(screenX, screenY, out, /*p.sampelCovered*/4);
				}
			}
			else
			{
				// DEBUG
				static	int x = 0;
				x++;
				coverageTest(p, s, bottom);
				PSOutput out;
				out.colorBuffer.x = 1.0f;
				out.colorBuffer.y = bottom ? 0.0f : 1.0f;
				out.colorBuffer.z = 0.0f;
				out.outZ = psInput.data[0].z;

				if (m_pipeLine.depthTest(i, y, out.outZ))
				{
					m_pipeLine.blener(i, y, out, p.sampelCovered);
				}
			}
		}
	}

	namespace
	{
		bool onRight(float x, float y, float X1, float Y1, float X2, float Y2)
		{
			float dx = X2 - X1;
			float dy = Y2 - Y1;
			float res = ((x - X1)* dy - (y - Y1) * dx);
			if (res >= 0.0f)
			{
				return true;
			}
			return false;
		}
	}

	void ScanLineRasterizer::coverageTest(P& screenPoint, ScreenTriangle& s, bool bottom)
	{
		const Point2* mask = m_pipeLine.getSampleMask();
		for (int i = 0; i < m_pipeLine.getSampleTests(); i++)
		{			
			bool test = true;
			/*if (bottom)
			{
				test &= onRight(screenPoint.x + mask[i].x, screenPoint.y + mask[i].y, s.p2.realX, s.p2.realY, s.p1.realX, s.p1.realY); // P2P1
				test &= onRight(screenPoint.x + mask[i].x, screenPoint.y + mask[i].y, s.p1.realX, s.p1.realY, s.p3.realX, s.p3.realY); // P1P3
				test &= onRight(screenPoint.x + mask[i].x, screenPoint.y + mask[i].y, s.p3.realX, s.p3.realY, s.p2.realX, s.p2.realY); // P3P2
			}								 				  
			else							 				  
			{								 				  
				test &= onRight(screenPoint.x + mask[i].x, screenPoint.y + mask[i].y, s.p1.realX, s.p1.realY, s.p2.realX, s.p2.realY); // P1P2
				test &= onRight(screenPoint.x + mask[i].x, screenPoint.y + mask[i].y, s.p2.realX, s.p2.realY, s.p3.realX, s.p3.realY); // P2P3
				test &= onRight(screenPoint.x + mask[i].x, screenPoint.y + mask[i].y, s.p3.realX, s.p3.realY, s.p1.realX, s.p1.realY); // P3P1
			}*/
			test &= onRight(screenPoint.x + mask[i].x, screenPoint.y + mask[i].y, m_screenTriangleO.p1.realX, m_screenTriangleO.p1.realY, m_screenTriangleO.p2.realX, m_screenTriangleO.p2.realY); // P1P2
			test &= onRight(screenPoint.x + mask[i].x, screenPoint.y + mask[i].y, m_screenTriangleO.p2.realX, m_screenTriangleO.p2.realY, m_screenTriangleO.p3.realX, m_screenTriangleO.p3.realY); // P2P3
			test &= onRight(screenPoint.x + mask[i].x, screenPoint.y + mask[i].y, m_screenTriangleO.p3.realX, m_screenTriangleO.p3.realY, m_screenTriangleO.p1.realX, m_screenTriangleO.p1.realY); // P3P1
			if (test)
			{
				screenPoint.needShade = true;
				screenPoint.sampelCovered++;
			}
		}	
	}

	void ScanLineRasterizer::convertFromNDC(P& screenPoint, ShaderIO & vertexData)
	{
		screenPoint.realX = ((vertexData.data[0].x + 1.0f) / 2.0f * ((float)m_w - 1.0f));
		screenPoint.x = std::lroundf(screenPoint.realX);
		screenPoint.fracX = screenPoint.realX - (float)screenPoint.x;
		
		screenPoint.realY = ((vertexData.data[0].y + 1.0f) / 2.0f * ((float)m_h - 1.0f));
		screenPoint.y = std::lroundf(screenPoint.realY);
		screenPoint.fracY = screenPoint.realY - (float)screenPoint.y;
	}

}