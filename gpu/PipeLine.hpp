#pragma once
#include "ShaderUtils.hpp"
#include <functional>

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
		void invokePixelShader(int x, int y, ShaderIO& p, PSOutput& out);
		void invokeVertexShader(ShaderIO& p);
		void setVertexShader(const vertexShaderType& vertexProgram) { m_vertexShader = vertexProgram; }
		void setPixelShader(const pixelShaderType& pixelProgram) { m_pixelShader = pixelProgram; }

		bool depthTest(int screenX, int screenY, float z);
		void blener(unsigned int x, unsigned int y, PSOutput& in, int tests);

		int getSampleTests() const { return m_sampleTests; }
		const Point2* getSampleMask() const { return m_sampleMask; }

		void setSampleMask(int count, Point2* mask);
	private:
		GraphicsAPI& m_gapi;
		int m_sampleTests;
		Point2 m_sampleMask[64];
		vertexShaderType m_vertexShader;
		pixelShaderType m_pixelShader;
	};
}