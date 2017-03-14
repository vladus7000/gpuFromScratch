#include "TiledRasterizer.hpp"

namespace gapi
{
	TiledRasterizer::TiledRasterizer(Pipeline& pipeline)
		: Rasterizer(pipeline)
	{
	}

	void TiledRasterizer::setup(ShaderIO* vertexData1, ShaderIO* vertexData2, ShaderIO* vertexData3, int w, int h)
	{
		m_vertexData1 = vertexData1;
		m_vertexData2 = vertexData2;
		m_vertexData3 = vertexData3;
		m_w = w;
		m_h = h;

		convertFromNDC(m_screenTriangle.p1, *m_vertexData1, w, h);
		convertFromNDC(m_screenTriangle.p2, *m_vertexData2, w, h);
		convertFromNDC(m_screenTriangle.p3, *m_vertexData3, w, h);

		m_screenTriangleO = m_screenTriangle;

		minX = std::floorf( std::min(m_screenTriangleO.p1.realX, std::min(m_screenTriangleO.p2.realX, m_screenTriangleO.p3.realX)));
		maxX = std::floorf( std::max(m_screenTriangleO.p1.realX, std::max(m_screenTriangleO.p2.realX, m_screenTriangleO.p3.realX)));

		minY = std::floorf( std::min(m_screenTriangleO.p1.realY, std::min(m_screenTriangleO.p2.realY, m_screenTriangleO.p3.realY)));
		maxY = std::floorf(std::max(m_screenTriangleO.p1.realY, std::max(m_screenTriangleO.p2.realY, m_screenTriangleO.p3.realY)));
		
		/*
		Point3 a((m_vertexData2->data[0] - m_vertexData1->data[0]).xyz());
		Point3 b((m_vertexData3->data[0] - m_vertexData1->data[0]).xyz());
		Point3 v0(m_vertexData1->data[0].xyz());

		Point3 camera(0.0f, 0.0f, 1.0f);
		
		if (!desc.FrontCounterClockwise)
		{
			Point3 N(a.z*b.y - a.y*b.z, a.x*b.z - a.z*b.x, a.y * b.x - a.x * b.y);
			float r = dot(camera, N);

			m_ccw = r < 0;
		}
		else
		{
			Point3 N(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x * b.y - a.y * b.x);
			m_ccw = (dot(camera, N) >= 0.0f);
		}
		*/
		m_area = edgeFunc(m_screenTriangleO.p1.realX, m_screenTriangleO.p1.realY, m_screenTriangleO.p2.realX, m_screenTriangleO.p2.realY, m_screenTriangleO.p3.realX, m_screenTriangleO.p3.realY);

		const auto& desc = m_pipeLine.RSGetState();
		m_ccw = desc.FrontCounterClockwise ? (m_area >= 0.0f) : (m_area < 0.0f);
	}

	void TiledRasterizer::rasterize()
	{
		const auto& desc = m_pipeLine.RSGetState();
		if (m_area == 0.0f || (desc.CullMode != D3D11_CULL_NONE && ((desc.CullMode == D3D11_CULL_FRONT && !m_ccw) || (desc.CullMode == D3D11_CULL_BACK && m_ccw))))
		{
			return;
		}
		m_area = 1.0 / m_area;
		for (int X = minX; X <= maxX; X++)
		{
			for (int Y = minY; Y <= maxY; Y++)
			{
				P p;
				p.x = X;
				p.y = Y;
				
				coverageTest(p, m_screenTriangleO);

				if (p.needShade)
				{
					int screenX = X;
					int screenY = Y;

					const float center = 0.5f;
					float u = edgeFunc(screenX + center, screenY + center, m_screenTriangleO.p2.realX, m_screenTriangleO.p2.realY, m_screenTriangleO.p3.realX, m_screenTriangleO.p3.realY);
					float v = edgeFunc(screenX + center, screenY + center, m_screenTriangleO.p3.realX, m_screenTriangleO.p3.realY, m_screenTriangleO.p1.realX, m_screenTriangleO.p1.realY);
					float w = edgeFunc(screenX + center, screenY + center, m_screenTriangleO.p1.realX, m_screenTriangleO.p1.realY, m_screenTriangleO.p2.realX, m_screenTriangleO.p2.realY);

					u *= m_area; // barycentric u
					v *= m_area; // barycentric v
					w *= m_area; // barycentric w
					if ((u >= 0 && u <= 1.0f) && (v >= 0 && v <= 1.0f) && (w >= 0 && w <= 1.0f))
					{
						PSOutput out;
						m_pipeLine.invokePixelShader(screenX, screenY, m_vertexData1, m_vertexData2, m_vertexData3, u, v, w, out);

						m_pipeLine.mergeSample(screenX, screenY, p, out);
					}
				}
			}
		}

	}
	bool TiledRasterizer::onRight(float x, float y, float X1, float Y1, float X2, float Y2)
	{
		float res = edgeFunc(x, y, X1, Y1, X2, Y2);
		//res = m_ccw ? res * -1.0f : res;
		if (res >= 0)
		{
			return true;
		}
		return false;
	}
	inline bool Compare(float A, float B)
	{
		float diff = A - B;
		//return (diff < FLT_EPSILON) && (-diff < FLT_EPSILON);
		return std::fabsf(A - B) < FLT_EPSILON;
	}
	void TiledRasterizer::coverageTest(P& screenPoint, ScreenTriangle& s)
	{
		const Point2* mask = m_pipeLine.getSampleMask();
		for (int i = 0; i < m_pipeLine.getSampleTests(); i++)
		{
			screenPoint.samplesCovered[i] = false;

			const float center = 0.5f;
			double u = edgeFunc(screenPoint.x + mask[i].x + center, screenPoint.y + mask[i].y + center, s.p2.realX, s.p2.realY, s.p3.realX, s.p3.realY);
			double v = edgeFunc(screenPoint.x + mask[i].x + center, screenPoint.y + mask[i].y + center, s.p3.realX, s.p3.realY, s.p1.realX, s.p1.realY);
			double w = edgeFunc(screenPoint.x + mask[i].x + center, screenPoint.y + mask[i].y + center, s.p1.realX, s.p1.realY, s.p2.realX, s.p2.realY);

			u *= m_area; // barycentric u
			v *= m_area; // barycentric v
			w *= m_area; // barycentric w
			if ((u >= 0 && u <= 1.0f) && (v >= 0 && v <= 1.0f) && (w >= 0 && w <= 1.0f))
			{
				const float epsilon = 0.0f;// 1e-10;
				//const double z1 = Compare(m_vertexData1->data[0].z, 0.0f) ? 1e-10 : 1.0 / (m_vertexData1->data[0].z);
				//const double z2 = Compare(m_vertexData2->data[0].z, 0.0f) ? 1e-10 : 1.0 / (m_vertexData2->data[0].z);
				//const double z3 = Compare(m_vertexData3->data[0].z, 0.0f) ? 1e-10 : 1.0 / (m_vertexData3->data[0].z);
				const double z1 = Compare(m_vertexData1->data[0].z, 0.0f) ? 1.0/1e-10 : 1.0 / (m_vertexData1->data[0].z);
				const double z2 = Compare(m_vertexData2->data[0].z, 0.0f) ? 1.0/1e-10 : 1.0 / (m_vertexData2->data[0].z);
				const double z3 = Compare(m_vertexData3->data[0].z, 0.0f) ? 1.0/1e-10 : 1.0 / (m_vertexData3->data[0].z);

				//const float z11 = 1.0f / (m_vertexData1->data[0].z + epsilon);
				//const float z21 = 1.0f / (m_vertexData2->data[0].z + epsilon);
				//const float z31 = 1.0f / (m_vertexData3->data[0].z + epsilon);

				const double invZ = (float)(u * z1 + v * z2 + w * z3);
				double z = 1.0f / invZ;// -epsilon;

				if (m_pipeLine.sampleDepthTest(screenPoint.x, screenPoint.y, i, z))
				{
					screenPoint.samplesCovered[i] = true;
					screenPoint.needShade = true;
					screenPoint.numSamplesCovered++;
				}
			}
		}
	}
}