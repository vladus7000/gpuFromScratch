#include "ScanLineRasterizer.hpp"
#include <algorithm>
#include <iostream>
namespace gapi
{
	namespace
	{
		float edgeFunc(float x, float y, float X1, float Y1, float X2, float Y2)
		{
			float dx = X2 - X1;
			float dy = Y2 - Y1;
			return ((x - X1)* dy - (y - Y1) * dx);

		}
		bool onRight(float x, float y, float X1, float Y1, float X2, float Y2)
		{
			if (edgeFunc(x, y, X1, Y1, X2, Y2) >= 0.0f)
			{
				return true;
			}
			return false;
		}
	}

	ScanLineRasterizer::ScanLineRasterizer(Pipeline& pipeline)
		: Rasterizer(pipeline)
	{}

	void ScanLineRasterizer::setup(ShaderIO* p1, ShaderIO* p2, ShaderIO* p3, int w, int h)
	{
		m_vertexData1 = p1;
		m_vertexData2 = p2;
		m_vertexData3 = p3;
		m_w = w;
		m_h = h;

		convertFromNDC(m_screenTriangle.p1, *m_vertexData1);
		convertFromNDC(m_screenTriangle.p2, *m_vertexData2);
		convertFromNDC(m_screenTriangle.p3, *m_vertexData3);

		m_screenTriangleO = m_screenTriangle;
	}

	void ScanLineRasterizer::rasterize()
	{
		// from: http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
		if (m_screenTriangle.p1.y < m_screenTriangle.p2.y)
		{
			std::swap(m_screenTriangle.p1, m_screenTriangle.p2);
		}
		if (m_screenTriangle.p2.y < m_screenTriangle.p3.y)
		{
			std::swap(m_screenTriangle.p2, m_screenTriangle.p3);
		}
		if (m_screenTriangle.p1.y < m_screenTriangle.p2.y)
		{
			std::swap(m_screenTriangle.p1, m_screenTriangle.p2);
		}

		if (m_screenTriangle.p2.y == m_screenTriangle.p3.y)
		{
			rasterizeTriangleBottom(m_screenTriangle);
		}
		else if (m_screenTriangle.p1.y == m_screenTriangle.p2.y)
		{
			rasterizeTriangleTop(m_screenTriangle);
		}
		else
		{
			P newPoint;
			newPoint.realX = m_screenTriangle.p1.realX + (float)(m_screenTriangle.p2.realY - m_screenTriangle.p1.realY) / (float)(m_screenTriangle.p3.realY - m_screenTriangle.p1.realY)*(float)(m_screenTriangle.p3.realX - m_screenTriangle.p1.realX);
			newPoint.realY = m_screenTriangle.p2.realY;
	
			newPoint.x = std::lroundf(newPoint.realX);
			newPoint.y = std::lroundf(newPoint.realY);
			
			m_screenTriangleBottom = m_screenTriangle;
			m_screenTriangleBottom.p3 = newPoint;
			if (m_screenTriangleBottom.p2.realX > m_screenTriangleBottom.p3.realX)
			{
				std::swap(m_screenTriangleBottom.p3, m_screenTriangleBottom.p2);
			}

			rasterizeTriangleBottom(m_screenTriangleBottom);
			m_screenTriangleTop = m_screenTriangle;
			m_screenTriangleTop.p1 = m_screenTriangle.p2;
			m_screenTriangleTop.p2 = newPoint;
			if (m_screenTriangleTop.p1.realX > m_screenTriangleTop.p2.realX)
			{
				std::swap(m_screenTriangleTop.p1, m_screenTriangleTop.p2);
			}
			rasterizeTriangleTop(m_screenTriangleTop);
		}
	}

	//	 1
	//  / \
	// 2---3
	void ScanLineRasterizer::rasterizeTriangleBottom(ScreenTriangle& s)
	{
		float invSlope1 = -(float)(s.p2.x - s.p1.x) / (float)(s.p2.y - s.p1.y);
		float invSlope2 = -(float)(s.p3.x - s.p1.x) / (float)(s.p3.y - s.p1.y);

		float curX = s.p1.x;
		float curX2 = s.p1.x;
		//for (int slY = std::floorf(s.p1.realY); slY >= std::ceilf(s.p2.realY); slY--)
		//for (int slY = std::lroundf(s.p1.realY); slY >= std::lroundf(s.p2.realY); slY--)
		for (int slY = s.p1.y; slY >= s.p2.y; slY--)
		{
			rasterizeStraightLine(s, std::lroundf(curX), std::lroundf(curX2), slY, true);
			curX += invSlope1;
			curX2 += invSlope2;
		}
	}

	// 1---2
	//  \ /
	//   3
	void ScanLineRasterizer::rasterizeTriangleTop(ScreenTriangle& s)
	{
		float invSlope1 = -(float)(s.p3.x - s.p1.x) / (float)(s.p3.y - s.p1.y);
		float invSlope2 = -(float)(s.p3.x - s.p2.x) / (float)(s.p3.y - s.p2.y);

		float curX = s.p3.x;
		float curX2 = s.p3.x;

		//for (int slY = std::floorf(s.p3.realY); slY <= std::ceilf(s.p1.realY); slY++)
		//for (int slY = std::lroundf(s.p3.realY); slY <= std::lroundf(s.p1.realY); slY++)
		for (int slY = s.p3.y; slY <= s.p1.y; slY++)
		{
			rasterizeStraightLine(s, std::lroundf(curX), std::lroundf(curX2), slY, false);
			curX -= invSlope1;
			curX2 -= invSlope2;
		}
	}

	void ScanLineRasterizer::rasterizeStraightLine(ScreenTriangle& s, int x1, int x2, int y, bool bottom)
	{
		if (x2 < x1)
		{
			std::swap(x1, x2);
		}
		//for (int i = std::lroundf(x1); i <= std::lroundf(x2); i ++)
		for (int i = x1; i <= x2; i++)
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

			coverageTest(p, s, bottom); // TODO:!!!
			if (p.needShade || 0)
			{
				int screenX = i;
				int screenY = y;
				
				float area = edgeFunc(m_screenTriangleO.p1.realX, m_screenTriangleO.p1.realY, m_screenTriangleO.p2.realX, m_screenTriangleO.p2.realY, m_screenTriangleO.p3.realX, m_screenTriangleO.p3.realY);

				float u = edgeFunc(p.realX, p.realY, m_screenTriangleO.p2.realX, m_screenTriangleO.p2.realY, m_screenTriangleO.p3.realX, m_screenTriangleO.p3.realY);
				float v = edgeFunc(p.realX, p.realY, m_screenTriangleO.p3.realX, m_screenTriangleO.p3.realY, m_screenTriangleO.p1.realX, m_screenTriangleO.p1.realY);
				float w = edgeFunc(p.realX, p.realY, m_screenTriangleO.p1.realX, m_screenTriangleO.p1.realY, m_screenTriangleO.p2.realX, m_screenTriangleO.p2.realY);

				u /= area; // barycentric u
				v /= area; // barycentric v
				w /= area; // barycentric w

				PSOutput out;
				m_pipeLine.invokePixelShader(screenX, screenY, m_vertexData1, m_vertexData2, m_vertexData3, u, v, w, out);

				//if (m_pipeLine.depthTest(screenX, screenY, out.outZ))
				{
					//m_pipeLine.blener(screenX, screenY, out, p.numSamplesCovered);
					m_pipeLine.mergeSample(i, y, p, out);
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
				out.outZ = 0;

				if (m_pipeLine.depthTest(i, y, out.outZ))
				{
					//m_pipeLine.blener(i, y, out, p.numSamplesCovered);
				//	m_pipeLine.mergeSample(i, y, )
				}
			}
		}
	}

	void ScanLineRasterizer::coverageTest(P& screenPoint, ScreenTriangle& s, bool bottom)
	{
		const Point2* mask = m_pipeLine.getSampleMask();
		for (int i = 0; i < m_pipeLine.getSampleTests(); i++)
		{
			screenPoint.samplesCovered[i] = false;
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
			test &= onRight(screenPoint.x +0.5f + mask[i].x, screenPoint.y + 0.5f + mask[i].y, m_screenTriangleO.p1.realX, m_screenTriangleO.p1.realY, m_screenTriangleO.p2.realX, m_screenTriangleO.p2.realY); // P1P2
			test &= onRight(screenPoint.x +0.5f + mask[i].x, screenPoint.y + 0.5f + mask[i].y, m_screenTriangleO.p2.realX, m_screenTriangleO.p2.realY, m_screenTriangleO.p3.realX, m_screenTriangleO.p3.realY); // P2P3
			test &= onRight(screenPoint.x +0.5f + mask[i].x, screenPoint.y + 0.5f + mask[i].y, m_screenTriangleO.p3.realX, m_screenTriangleO.p3.realY, m_screenTriangleO.p1.realX, m_screenTriangleO.p1.realY); // P3P1
			if (test)
			{
				float area = edgeFunc(m_screenTriangleO.p1.realX, m_screenTriangleO.p1.realY, m_screenTriangleO.p2.realX, m_screenTriangleO.p2.realY, m_screenTriangleO.p3.realX, m_screenTriangleO.p3.realY);

				float u = edgeFunc(screenPoint.x + 0.5f + mask[i].x, screenPoint.y + 0.5f + mask[i].y, m_screenTriangleO.p2.realX, m_screenTriangleO.p2.realY, m_screenTriangleO.p3.realX, m_screenTriangleO.p3.realY);
				float v = edgeFunc(screenPoint.x + 0.5f + mask[i].x, screenPoint.y + 0.5f + mask[i].y, m_screenTriangleO.p3.realX, m_screenTriangleO.p3.realY, m_screenTriangleO.p1.realX, m_screenTriangleO.p1.realY);
				float w = edgeFunc(screenPoint.x + 0.5f + mask[i].x, screenPoint.y + 0.5f + mask[i].y, m_screenTriangleO.p1.realX, m_screenTriangleO.p1.realY, m_screenTriangleO.p2.realX, m_screenTriangleO.p2.realY);

				u /= area; // barycentric u
				v /= area; // barycentric v
				w /= area; // barycentric w

				float z = getFromBarycentric2(m_vertexData1->data[0].z, m_vertexData2->data[0].z, m_vertexData3->data[0].z, u, v, w);

				if (m_pipeLine.sampleDepthTest(screenPoint.x, screenPoint.y, i, z))
				{
					screenPoint.samplesCovered[i] = true;
					screenPoint.needShade = true;
					screenPoint.numSamplesCovered++;
				}
			}
		}	
	}

	void ScanLineRasterizer::convertFromNDC(P& screenPoint, ShaderIO & vertexData)
	{
		screenPoint.realX = ((vertexData.data[0].x + 1.0f) / 2.0f * ((float)m_w));
		screenPoint.x = std::lroundf(screenPoint.realX);
		if (screenPoint.x >= m_w)
		{
			screenPoint.x = m_w - 1;
		}
		screenPoint.fracX = screenPoint.realX - (float)screenPoint.x;
		
		screenPoint.realY = ((vertexData.data[0].y + 1.0f) / 2.0f * ((float)m_h));
		screenPoint.y = std::lroundf(screenPoint.realY);
		if (screenPoint.y >= m_h)
		{
			screenPoint.y = m_h - 1;
		}
		screenPoint.fracY = screenPoint.realY - (float)screenPoint.y;
	}

}