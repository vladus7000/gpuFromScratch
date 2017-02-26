#pragma once
#include "Math.hpp"
#include "ShaderUtils.hpp"

#include "PipeLine.hpp"

namespace gapi
{
	class GraphicsAPI
	{
	public:
		enum
		{
			Y_AXIS_TOP = 1
		};

		GraphicsAPI(unsigned int w, unsigned int h, unsigned long int flags = 0);
		~GraphicsAPI();

		void clear(const Pixel& pixelColor);
		void loadPicture(const char* path);
		void loadTexture(const char* path);

		/*
		* (0,0) is bottom left corner
		* (w-1, h-1) is top right corner
		*/
		void setPixel(const Point2& p, const Pixel& pixelColor);
		void setPixel(unsigned int x, unsigned int y, const Pixel& pixelColor);
		const Pixel& getPixel(const Point2& p);
		void drawLine(const Point2& p1, const Point2& p2, const Pixel& pixelColor);
		void drawCircle(const Point2& p, unsigned int r, const Pixel& pixelColor, bool filled);

		const Pixel* getFrameBuffer() const { return m_frameBuffer; }
		Pixel* getFrameBuffer() { return m_frameBuffer; }
		unsigned int getWidth() const { return m_width; }
		unsigned int getHeight() const { return m_height; }

		void drawText(const Point2& p, const char* text, const Pixel& pixelColor);
		void drawChar(int x, int y, char c, const Pixel& pixelColor);

		//dx11 methods
		void Draw(unsigned int vertexCount, unsigned int startVertexLocation);
		void DrawIndexed(unsigned int indexCount, unsigned int startIndexLocation, unsigned int baseVertexLocation);

		void setVertexBuffer(Point3* buffer, unsigned int vertexCount);
		void setIndexBuffer(unsigned int* buffer, unsigned int indexCount);

		void setVertexShader(const Pipeline::vertexShaderType& vertexProgram) { m_pipeline.setVertexShader(vertexProgram); }
		void setPixelShader(const Pipeline::pixelShaderType& pixelProgram) { m_pipeline.setPixelShader(pixelProgram); }

		///temp
		void setNBuffer(Point3* buffer, unsigned int n);
		bool depthTest(int screenX, int screenY, float z);
		Point4 sampleTexture(const Point2& tCoord);

	private:
		void processTriangle(ShaderIO& p1, ShaderIO& p2, ShaderIO& p3);

		void setPixel4(int cx, int cy, int r, const Pixel& pixelColor);
		void setPixel8(int cx, int cy, int x, int y, int r, const Pixel& pixelColor, bool filled);

	private:
		unsigned int m_width;
		unsigned int m_height;
		Pixel* m_frameBuffer;
		float* m_Zbuffer;
		unsigned long int m_flags;

		unsigned int* m_indexBuffer;
		Point3* m_vertexBuffer;
		Point3* m_Buffers[7];
		unsigned int m_indexCount;
		unsigned int m_vertexCount;

		unsigned int m_textureW;
		unsigned int m_textureH;
		Pixel* m_textureData;

		Pipeline m_pipeline;
	};
}
