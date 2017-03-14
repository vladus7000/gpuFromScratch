#include "PipeLine.hpp"
#include <algorithm>

#include "GraphicsAPI.h"

namespace gapi
{
	Pipeline::Pipeline(GraphicsAPI& gapi)
		: m_gapi(gapi)
		, m_perspectiveCorrection(true)
	{
		m_sampleTests = g_numSamples;
	//	float maskX[] = { -0.4f, 0.05f, 0.4f, -0.05f };
	//	float maskY[] = { 0.05f, 0.4f, -0.05f, -0.4f };
		float maskX[] = { -0.25f, -0.25f, 0.25f, 0.25f };
		float maskY[] = { -0.25f, 0.25f, 0.25f, -0.25f };
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


		defaultSettings();
	}
	inline bool Compare(float A, float B)
	{
		float diff = A - B;
		//return (diff < FLT_EPSILON) && (-diff < FLT_EPSILON);
		return std::fabsf(A - B) < FLT_EPSILON;
	}
	void Pipeline::invokePixelShader(int x, int y, ShaderIO* v1, ShaderIO* v2, ShaderIO* v3, float u, float v, float w, PSOutput& out)
	{
		if (m_pixelShader)
		{
			ShaderIO psInput;
		/*	const float epsilon = 1e-10;//FLT_MIN;

			const float z1g = v1->data[0].z + epsilon;
			const float z2g = v2->data[0].z + epsilon;
			const float z3g = v3->data[0].z + epsilon;

			const float invz1 = 1.0f / z1g;
			const float invz2 = 1.0f / z2g;
			const float invz3 = 1.0f / z3g;

			//const float invZ = (1.0f / z1) * u + (1.0f / z2) * v + (1.0f / z3) * w;
			const float z11 = 1.0f / (invz1 * u + invz2 * v + invz3 * w);
			
			*/
			//const double z1 = Compare(m_vertexData1->data[0].z, 0.0f) ? 1e-10 : 1.0 / (m_vertexData1->data[0].z);
			//const double z2 = Compare(m_vertexData2->data[0].z, 0.0f) ? 1e-10 : 1.0 / (m_vertexData2->data[0].z);
			//const double z3 = Compare(m_vertexData3->data[0].z, 0.0f) ? 1e-10 : 1.0 / (m_vertexData3->data[0].z);
			const double z1 = Compare(v1->data[0].w, 0.0f) ? 1.0 / 1e-10 : 1.0 / (v1->data[0].w);
			const double z2 = Compare(v2->data[0].w, 0.0f) ? 1.0 / 1e-10 : 1.0 / (v2->data[0].w);
			const double z3 = Compare(v3->data[0].w, 0.0f) ? 1.0 / 1e-10 : 1.0 / (v3->data[0].w);

			//const float z11 = 1.0f / (m_vertexData1->data[0].z + epsilon);
			//const float z21 = 1.0f / (m_vertexData2->data[0].z + epsilon);
			//const float z31 = 1.0f / (m_vertexData3->data[0].z + epsilon);

			const double invW = (float)(u * z1 + v * z2 + w * z3);
			double W = 1.0f / invW;// -epsilon;

			for (int i = 1; i < ShaderIO::dataCount; i++)
			{
				if (m_perspectiveCorrection)
				{
					Point4 d1 = v1->data[i] * z1;
					Point4 d2 = v2->data[i] * z2;
					Point4 d3 = v3->data[i] * z3;

					psInput.data[i] = getFromBarycentric2(d1, d2, d3, u, v, w);
					psInput.data[i] *= W;
				}
				else
				{
					psInput.data[i] = getFromBarycentric2(v1->data[i], v2->data[i], v3->data[i], u, v, w);
				}
			}

			out.outZ = 1;//z

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

	void Pipeline::defaultSettings()
	{
		m_rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		m_rasterizerDesc.CullMode = D3D11_CULL_NONE;
		m_rasterizerDesc.FrontCounterClockwise = false;
		m_rasterizerDesc.DepthBias = false;
		m_rasterizerDesc.DepthBiasClamp = 0;
		m_rasterizerDesc.SlopeScaledDepthBias = 0;
		m_rasterizerDesc.DepthClipEnable = true;
		m_rasterizerDesc.ScissorEnable = false;
		m_rasterizerDesc.MultisampleEnable = g_numSamples > 1;
		m_rasterizerDesc.AntialiasedLineEnable = false;
	}
}