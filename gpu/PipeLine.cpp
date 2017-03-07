#include "PipeLine.hpp"
#include <algorithm>

#include "GraphicsAPI.h"

namespace gapi
{
	Pipeline::Pipeline(GraphicsAPI& gapi)
		: m_gapi(gapi)
		, m_perspectiveCorrection(!true)
	{
		m_sampleTests = g_numSamples;
	//	float maskX[] = { -0.4f, 0.05f, 0.4f, -0.05f };
	//	float maskY[] = { 0.05f, 0.4f, -0.05f, -0.4f };
		float maskX[] = { -0.5f, -0.5f, 0.5f, 0.5f };
		float maskY[] = { -0.5f, 0.5f, 0.5f, -0.5f };
	//	float maskX[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	//	float maskY[] = { 0.0f, 1.0f, 1.0f, 0.0f };
		for (int i = 0; i < m_sampleTests; i++)
		{
			m_sampleMask[i].x = maskX[i];
			m_sampleMask[i].y = maskY[i];
		}

		if (g_numSamples == 1)
		{
			Point2 p;
			setSampleMask(1, &p);
		}
	}

	void Pipeline::invokePixelShader(int x, int y, ShaderIO* v1, ShaderIO* v2, ShaderIO* v3, float u, float v, float w, PSOutput& out)
	{
		if (m_pixelShader)
		{
			ShaderIO psInput;
			const float epsilon = FLT_MIN;//1e-10;

			const float z1 = v1->data[0].z + epsilon;
			const float z2 = v2->data[0].z + epsilon;
			const float z3 = v3->data[0].z + epsilon;

			const float invz1 = 1.0f / z1;
			const float invz2 = 1.0f / z2;
			const float invz3 = 1.0f / z3;

			//const float invZ = (1.0f / z1) * u + (1.0f / z2) * v + (1.0f / z3) * w;
			const float z = 1.0f / (invz1 * u + invz2 * v + invz3 * w);

			for (int i = 1; i < ShaderIO::dataCount; i++)
			{
				if (m_perspectiveCorrection)
				{
					psInput.data[i] = getFromBarycentric2(v1->data[i] / z1, v2->data[i] / z2, v3->data[i] / z3, u, v, w);
					psInput.data[i] *= z;
				}
				else
				{
					psInput.data[i] = getFromBarycentric2(v1->data[i], v2->data[i], v3->data[i], u, v, w);
				}
			}

			out.outZ = z;

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
	bool Pipeline::sampleDepthTest(int screenX, int screenY, int sample, float z)
	{
		return m_gapi.sampleDepthTest(screenX, screenY, sample, z);
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

	void Pipeline::mergeSample(unsigned int x, unsigned int y, P& in, PSOutput& out)
	{
		for (int i = 0; i < g_numSamples; i++)
		{
			if (in.samplesCovered[i])
			{
				m_gapi.setSampleColor(x, y, i, Pixel(out.colorBuffer.x, out.colorBuffer.y, out.colorBuffer.z));
			}
		}
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