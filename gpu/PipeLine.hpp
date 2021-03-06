#pragma once
#include "ShaderUtils.hpp"
#include <functional>
#include <D3DX11.h>

namespace gapi
{
	class GraphicsAPI;

	class Pipeline
	{
	public:
		typedef std::function<void(int x, int y, ShaderIO& p, PSOutput& out)> pixelShaderType;
		typedef std::function<void(ShaderIO& p)> vertexShaderType;

	public:
		Pipeline(GraphicsAPI& gapi);
		void invokePixelShader(int x, int y, ShaderIO* v1, ShaderIO* v2, ShaderIO* v3, float u, float v, float w, PSOutput& out);
		void invokeVertexShader(ShaderIO& p);
		void setVertexShader(const vertexShaderType& vertexProgram) { m_vertexShader = vertexProgram; }
		void setPixelShader(const pixelShaderType& pixelProgram) { m_pixelShader = pixelProgram; }

		bool depthTest(int screenX, int screenY, float z);
		bool sampleDepthTest(int screenX, int screenY, int sample, float z);
		void blener(unsigned int x, unsigned int y, PSOutput& in, int tests);

		void mergeSample(unsigned int x, unsigned int y, P& in, PSOutput& out);

		int getSampleTests() const { return /*m_sampleTests;*/ g_numSamples; }
		const Point2* getSampleMask() const { return m_sampleMask; }

		void setSampleMask(int count, Point2* mask);

		const D3D11_RASTERIZER_DESC& RSGetState() const { return m_rasterizerDesc; };
		void RSSetState(const D3D11_RASTERIZER_DESC& desc) { m_rasterizerDesc = desc; }
	private:
		void defaultSettings();
	private:
		GraphicsAPI& m_gapi;
		int m_sampleTests;
		Point2 m_sampleMask[64];
		vertexShaderType m_vertexShader;
		pixelShaderType m_pixelShader;
		bool m_perspectiveCorrection;

		D3D11_RASTERIZER_DESC m_rasterizerDesc;
	};
}