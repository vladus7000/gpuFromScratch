#include "PipeLine.hpp"
#include <algorithm>

#include "GraphicsAPI.h"

namespace gapi
{
	Pipeline::Pipeline(GraphicsAPI& gapi)
		: m_gapi(gapi)
	{
		m_sampleTests = 4;
		//float maskX[] = { -0.4f, 0.05f, 0.4f, -0.05f };
		//float maskY[] = { 0.05f, 0.4f, -0.05f, -0.4f };
		float maskX[] = { -0.5f, 0.0f, 0.5f, 0.0f };
		float maskY[] = { 0.0f, 0.5f, 0.0f, -0.5f };
		for (int i = 0; i < 4; i++)
		{
			m_sampleMask[i].x = maskX[i];
			m_sampleMask[i].y = maskY[i];
		}
	//	Point2 p;
	//	setSampleMask(1, &p);
	}

	void Pipeline::invokePixelShader(int x, int y, ShaderIO* v1, ShaderIO* v2, ShaderIO* v3, float u, float v, float w, PSOutput& out)
	{
		if (m_pixelShader)
		{
			ShaderIO psInput;
			for (int i = 0; i < ShaderIO::dataCount; i++)
			{
				psInput.data[i] = getFromBarycentric2(v1->data[i], v2->data[i], v3->data[i], u, v, w);
			}
			out.outZ = psInput.data[0].z;
			m_pixelShader(x, y, psInput, out);
		}
	}

	void Pipeline::invokeVertexShader(ShaderIO& p)
	{
		if (m_vertexShader)
		{
			m_vertexShader(p);
		}
	}

	bool Pipeline::depthTest(int screenX, int screenY, float z)
	{
		return m_gapi.depthTest(screenX, screenY, z);
	}

	void Pipeline::blener(unsigned int x, unsigned int y, PSOutput& in, int tests)
	{
		Pixel p = m_gapi.getPixel(Point2(x, y));
		//Point4 p1(0.4, 0.5, 0.4);// (p.r, p.g, p.b);
		Point4 p1(p.r, p.g, p.b);
		float frac = (float)tests / (float)m_sampleTests;
		Point4 res = in.colorBuffer * frac + p1 * (1.0f - frac);
		m_gapi.setPixel(x, y, Pixel(res.x, res.y, res.z));
	}
	
	void Pipeline::setSampleMask(int count, Point2* mask)
	{
		if (!mask)
		{
			return;
		}
		for (int i = 0; i < count; i++)
		{
			m_sampleMask[i] = mask[i];
		}
		m_sampleTests = count;
	}
}