#include "GraphicsAPI.h"
#include <cstring>
#include "../FreeImage/FreeImage.h"
#include <algorithm>

#include "ScanLineRasterizer.hpp"

namespace gapi
{
	static const unsigned char FONT8x8[] = {
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // ' '
			0x00, 0x06, 0x5F, 0x5F, 0x06, 0x00, 0x00, 0x00,  // '!'
			0x00, 0x07, 0x07, 0x00, 0x07, 0x07, 0x00, 0x00,  // '"'
			0x14, 0x7F, 0x7F, 0x14, 0x7F, 0x7F, 0x14, 0x00,  // '#'
			0x24, 0x2E, 0x6B, 0x6B, 0x3A, 0x12, 0x00, 0x00,  // '$'
			0x46, 0x66, 0x30, 0x18, 0x0C, 0x66, 0x62, 0x00,  // '%'
			0x30, 0x7A, 0x4F, 0x5D, 0x37, 0x7A, 0x48, 0x00,  // '&'
			0x04, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,  // '''
			0x00, 0x1C, 0x3E, 0x63, 0x41, 0x00, 0x00, 0x00,  // '('
			0x00, 0x41, 0x63, 0x3E, 0x1C, 0x00, 0x00, 0x00,  // ')'
			0x08, 0x2A, 0x3E, 0x1C, 0x1C, 0x3E, 0x2A, 0x08,  // '*'
			0x08, 0x08, 0x3E, 0x3E, 0x08, 0x08, 0x00, 0x00,  // '+'
			0x00, 0x80, 0xE0, 0x60, 0x00, 0x00, 0x00, 0x00,  // ','
			0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,  // '-'
			0x00, 0x00, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00,  // '.'
			0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00,  // '/'
			0x3E, 0x7F, 0x71, 0x59, 0x4D, 0x7F, 0x3E, 0x00,  // '0'
			0x40, 0x42, 0x7F, 0x7F, 0x40, 0x40, 0x00, 0x00,  // '1'
			0x62, 0x73, 0x59, 0x49, 0x6F, 0x66, 0x00, 0x00,  // '2'
			0x22, 0x63, 0x49, 0x49, 0x7F, 0x36, 0x00, 0x00,  // '3'
			0x18, 0x1C, 0x16, 0x53, 0x7F, 0x7F, 0x50, 0x00,  // '4'
			0x27, 0x67, 0x45, 0x45, 0x7D, 0x39, 0x00, 0x00,  // '5'
			0x3C, 0x7E, 0x4B, 0x49, 0x79, 0x30, 0x00, 0x00,  // '6'
			0x03, 0x03, 0x71, 0x79, 0x0F, 0x07, 0x00, 0x00,  // '7'
			0x36, 0x7F, 0x49, 0x49, 0x7F, 0x36, 0x00, 0x00,  // '8'
			0x06, 0x4F, 0x49, 0x69, 0x3F, 0x1E, 0x00, 0x00,  // '9'
			0x00, 0x00, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,  // ':'
			0x00, 0x80, 0xE6, 0x66, 0x00, 0x00, 0x00, 0x00,  // ';'
			0x08, 0x1C, 0x36, 0x63, 0x41, 0x00, 0x00, 0x00,  // '<'
			0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x00, 0x00,  // '='
			0x00, 0x41, 0x63, 0x36, 0x1C, 0x08, 0x00, 0x00,  // '>'
			0x02, 0x03, 0x51, 0x59, 0x0F, 0x06, 0x00, 0x00,  // '?'
			0x3E, 0x7F, 0x41, 0x5D, 0x5D, 0x1F, 0x1E, 0x00,  // '@'
			0x7C, 0x7E, 0x13, 0x13, 0x7E, 0x7C, 0x00, 0x00,  // 'A'
			0x41, 0x7F, 0x7F, 0x49, 0x49, 0x7F, 0x36, 0x00,  // 'B'
			0x1C, 0x3E, 0x63, 0x41, 0x41, 0x63, 0x22, 0x00,  // 'C'
			0x41, 0x7F, 0x7F, 0x41, 0x63, 0x3E, 0x1C, 0x00,  // 'D'
			0x41, 0x7F, 0x7F, 0x49, 0x5D, 0x41, 0x63, 0x00,  // 'E'
			0x41, 0x7F, 0x7F, 0x49, 0x1D, 0x01, 0x03, 0x00,  // 'F'
			0x1C, 0x3E, 0x63, 0x41, 0x51, 0x73, 0x72, 0x00,  // 'G'
			0x7F, 0x7F, 0x08, 0x08, 0x7F, 0x7F, 0x00, 0x00,  // 'H'
			0x00, 0x41, 0x7F, 0x7F, 0x41, 0x00, 0x00, 0x00,  // 'I'
			0x30, 0x70, 0x40, 0x41, 0x7F, 0x3F, 0x01, 0x00,  // 'J'
			0x41, 0x7F, 0x7F, 0x08, 0x1C, 0x77, 0x63, 0x00,  // 'K'
			0x41, 0x7F, 0x7F, 0x41, 0x40, 0x60, 0x70, 0x00,  // 'L'
			0x7F, 0x7F, 0x0E, 0x1C, 0x0E, 0x7F, 0x7F, 0x00,  // 'M'
			0x7F, 0x7F, 0x06, 0x0C, 0x18, 0x7F, 0x7F, 0x00,  // 'N'
			0x1C, 0x3E, 0x63, 0x41, 0x63, 0x3E, 0x1C, 0x00,  // 'O'
			0x41, 0x7F, 0x7F, 0x49, 0x09, 0x0F, 0x06, 0x00,  // 'P'
			0x1E, 0x3F, 0x21, 0x71, 0x7F, 0x5E, 0x00, 0x00,  // 'Q'
			0x41, 0x7F, 0x7F, 0x09, 0x19, 0x7F, 0x66, 0x00,  // 'R'
			0x26, 0x6F, 0x4D, 0x59, 0x73, 0x32, 0x00, 0x00,  // 'S'
			0x03, 0x41, 0x7F, 0x7F, 0x41, 0x03, 0x00, 0x00,  // 'T'
			0x7F, 0x7F, 0x40, 0x40, 0x7F, 0x7F, 0x00, 0x00,  // 'U'
			0x1F, 0x3F, 0x60, 0x60, 0x3F, 0x1F, 0x00, 0x00,  // 'V'
			0x7F, 0x7F, 0x30, 0x18, 0x30, 0x7F, 0x7F, 0x00,  // 'W'
			0x43, 0x67, 0x3C, 0x18, 0x3C, 0x67, 0x43, 0x00,  // 'X'
			0x07, 0x4F, 0x78, 0x78, 0x4F, 0x07, 0x00, 0x00,  // 'Y'
			0x47, 0x63, 0x71, 0x59, 0x4D, 0x67, 0x73, 0x00,  // 'Z'
			0x00, 0x7F, 0x7F, 0x41, 0x41, 0x00, 0x00, 0x00,  // '['
			0x01, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x00,  // backslash
			0x00, 0x41, 0x41, 0x7F, 0x7F, 0x00, 0x00, 0x00,  // ']'
			0x08, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x08, 0x00,  // '^'
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,  // '_'
			0x00, 0x00, 0x03, 0x07, 0x04, 0x00, 0x00, 0x00,  // '`'
			0x20, 0x74, 0x54, 0x54, 0x3C, 0x78, 0x40, 0x00,  // 'a'
			0x41, 0x7F, 0x3F, 0x48, 0x48, 0x78, 0x30, 0x00,  // 'b'
			0x38, 0x7C, 0x44, 0x44, 0x6C, 0x28, 0x00, 0x00,  // 'c'
			0x30, 0x78, 0x48, 0x49, 0x3F, 0x7F, 0x40, 0x00,  // 'd'
			0x38, 0x7C, 0x54, 0x54, 0x5C, 0x18, 0x00, 0x00,  // 'e'
			0x48, 0x7E, 0x7F, 0x49, 0x03, 0x02, 0x00, 0x00,  // 'f'
			0x98, 0xBC, 0xA4, 0xA4, 0xF8, 0x7C, 0x04, 0x00,  // 'g'
			0x41, 0x7F, 0x7F, 0x08, 0x04, 0x7C, 0x78, 0x00,  // 'h'
			0x00, 0x44, 0x7D, 0x7D, 0x40, 0x00, 0x00, 0x00,  // 'i'
			0x60, 0xE0, 0x80, 0x80, 0xFD, 0x7D, 0x00, 0x00,  // 'j'
			0x41, 0x7F, 0x7F, 0x10, 0x38, 0x6C, 0x44, 0x00,  // 'k'
			0x00, 0x41, 0x7F, 0x7F, 0x40, 0x00, 0x00, 0x00,  // 'l'
			0x7C, 0x7C, 0x18, 0x38, 0x1C, 0x7C, 0x78, 0x00,  // 'm'
			0x7C, 0x7C, 0x04, 0x04, 0x7C, 0x78, 0x00, 0x00,  // 'n'
			0x38, 0x7C, 0x44, 0x44, 0x7C, 0x38, 0x00, 0x00,  // 'o'
			0x84, 0xFC, 0xF8, 0xA4, 0x24, 0x3C, 0x18, 0x00,  // 'p'
			0x18, 0x3C, 0x24, 0xA4, 0xF8, 0xFC, 0x84, 0x00,  // 'q'
			0x44, 0x7C, 0x78, 0x4C, 0x04, 0x1C, 0x18, 0x00,  // 'r'
			0x48, 0x5C, 0x54, 0x54, 0x74, 0x24, 0x00, 0x00,  // 's'
			0x00, 0x04, 0x3E, 0x7F, 0x44, 0x24, 0x00, 0x00,  // 't'
			0x3C, 0x7C, 0x40, 0x40, 0x3C, 0x7C, 0x40, 0x00,  // 'u'
			0x1C, 0x3C, 0x60, 0x60, 0x3C, 0x1C, 0x00, 0x00,  // 'v'
			0x3C, 0x7C, 0x70, 0x38, 0x70, 0x7C, 0x3C, 0x00,  // 'w'
			0x44, 0x6C, 0x38, 0x10, 0x38, 0x6C, 0x44, 0x00,  // 'x'
			0x9C, 0xBC, 0xA0, 0xA0, 0xFC, 0x7C, 0x00, 0x00,  // 'y'
			0x4C, 0x64, 0x74, 0x5C, 0x4C, 0x64, 0x00, 0x00,  // 'z'
			0x08, 0x08, 0x3E, 0x77, 0x41, 0x41, 0x00, 0x00,  // '{'
			0x00, 0x00, 0x00, 0x77, 0x77, 0x00, 0x00, 0x00,  // '|'
			0x41, 0x41, 0x77, 0x3E, 0x08, 0x08, 0x00, 0x00,  // '}'
			0x08, 0x08, 0x1C, 0x1C, 0x3E, 0x3E, 0x7F, 0x00,  // left arrow / triangle   0x7e
			0x7F, 0x3E, 0x3E, 0x1C, 0x1C, 0x08, 0x08, 0x00,  // right arrow / triangle  0x7f
			0x30, 0x38, 0x3C, 0x3E, 0x3E, 0x3C, 0x38, 0x30,  // Up arrow / triangle     0x80
			0x06, 0x0E, 0x1E, 0x3E, 0x3E, 0x1E, 0x0E, 0x06   // Down arrow / triangle   0x81
	};


GraphicsAPI::GraphicsAPI(unsigned int w, unsigned int h, unsigned long int flags)
	: m_width(w)
	, m_height(h)
	, m_frameBuffer(new Pixel[w*h])
	, m_Zbuffer(new float[w*h])
	, m_flags(flags)
	, m_indexBuffer(nullptr)
	, m_vertexBuffer(nullptr)
	, m_indexCount(0)
	, m_vertexCount(0)
	, m_pipeline(*this)
{
	for (int i = 0; i < 7; i++)
	{
		m_Buffers[i] = nullptr;
	}

	m_textureW = 0;
	m_textureH = 0;

	m_textureData = nullptr;
}

GraphicsAPI::~GraphicsAPI()
{
	if (m_frameBuffer)
	{
		delete[] m_frameBuffer;
		m_frameBuffer = nullptr;
	}
	if (m_Zbuffer)
	{
		delete[] m_Zbuffer;
		m_Zbuffer = nullptr;
	}
	if (m_textureData)
	{
		delete[] m_textureData;
		m_textureData = nullptr;
	}
}

void GraphicsAPI::clear(const Pixel& pixelColor)
{
	for (unsigned int i = 0; i < m_width * m_height; i++)
	{
		m_frameBuffer[i] = pixelColor;
		m_Zbuffer[i] = 1.0f;
	}
}

void GraphicsAPI::loadPicture(const char* path)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP *dib(0);
	int width(0), height(0);
	fif = FreeImage_GetFileType(path, 0);
	if (fif == FIF_UNKNOWN)
	{
		fif = FreeImage_GetFIFFromFilename(path);
	}
	if (fif == FIF_UNKNOWN)
	{
		return;
	}

	if (FreeImage_FIFSupportsReading(fif))
	{
		dib = FreeImage_Load(fif, path);
	}

	if (!dib)
	{
		return;
	}

	unsigned char *data = NULL;

	data = FreeImage_GetBits(dib);
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);

	if (m_width != width || m_height != height)
	{
		m_width = width;
		m_height = height;

		if (m_frameBuffer)
		{
			delete[] m_frameBuffer;
			m_frameBuffer = nullptr;
		}

		if (m_Zbuffer)
		{
			delete[] m_Zbuffer;
			m_Zbuffer = nullptr;
		}

		m_Zbuffer = new float[width * height];
		m_frameBuffer = new Pixel[width * height];
	}

	auto frameBuffer = getFrameBuffer();

	for (unsigned int i = 0; i < width * height; i++)
	{
		m_Zbuffer[i] = 1.0f;
		frameBuffer[i] = Pixel(data[i * 3 + 2] / 255.0f, data[i * 3 + 1] / 255.0f, data[i * 3] / 255.0f);
	}
	FreeImage_Unload(dib);
}

void GraphicsAPI::loadTexture(const char * path)
{
	//copy paste
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP *dib(0);
	int width(0), height(0);
	fif = FreeImage_GetFileType(path, 0);
	if (fif == FIF_UNKNOWN)
	{
		fif = FreeImage_GetFIFFromFilename(path);
	}
	if (fif == FIF_UNKNOWN)
	{
		return;
	}

	if (FreeImage_FIFSupportsReading(fif))
	{
		dib = FreeImage_Load(fif, path);
	}

	if (!dib)
	{
		return;
	}

	unsigned char *data = NULL;

	data = FreeImage_GetBits(dib);
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);

	if (m_textureW != width || m_textureH != height)
	{
		m_textureW = width;
		m_textureH = height;

		if (m_textureData)
		{
			delete[] m_textureData;
			m_textureData = nullptr;
		}
		m_textureData = new Pixel[width * height];
	}

	for (unsigned int i = 0; i < width * height; i++)
	{
		m_textureData[i] = Pixel(data[i * 3 + 2] / 255.0f, data[i * 3 + 1] / 255.0f, data[i * 3] / 255.0f);
	}
	FreeImage_Unload(dib);
}

void GraphicsAPI::setPixel(const Point2& p, const Pixel & pixelColor)
{
	setPixel(std::roundf(p.x), std::roundf(p.y), pixelColor);
}

void GraphicsAPI::setPixel(unsigned int x, unsigned int y, const Pixel & pixelColor)
{
	if (x < m_width && y < m_height && x >= 0 && y >= 0)
	{
		unsigned int _y = y;

		if (m_flags & Y_AXIS_TOP)
		{
			_y = (m_height - 1) - y;
		}
		m_frameBuffer[_y * m_width + x] = pixelColor;
	}
}

const Pixel& GraphicsAPI::getPixel(const Point2& p)
{
	static Pixel wrongPixel;
	unsigned int y = p.y;
	if (p.x < m_width && p.y < m_height && p.x >= 0 && p.y >= 0)
	{
		if (m_flags & Y_AXIS_TOP)
		{
			y = m_height - p.y - 1;
		}
		return m_frameBuffer[(int)p.y * m_width + (int)p.x];
	}
	return wrongPixel;
}

#define f(x, y) (dy*(x)-dx*(y)-(x1*dy-y1*dx))

void GraphicsAPI::drawLine(const Point2& p1, const Point2& p2, const Pixel& pixelColor)
{
	int x, y, count, dx, dy;
	x = p1.x; y = p1.y;
	dx = p2.x - p1.x;
	dy = p2.y - p1.y;
	count = dx;
	setPixel(Point2(x, y), pixelColor);
	while (count > 0)
	{
		count--;
		int res = (dy*(x + 1) - dx*(y + 0.5f) - ((int)p1.x*dy - (int)p1.y*dx));
		if (res > 0)
		{
			y = y + 1;
		}
		x++;
		setPixel(Point2(x, y), pixelColor);
	}
}

void GraphicsAPI::drawCircle(const Point2& p, unsigned int r, const Pixel & pixelColor, bool filled)
{
	int cx = p.x;
	int cy = p.y;
	int x, y;

	if (!filled)
	{
		setPixel4(cx, cy, r, pixelColor);
	}
	else
	{
		drawLine(Point2(cx - r, cy), Point2(cx + r, cy), pixelColor);
	}
	x = 0; y = r;
	int r2 = (int)r*r;
	while (x <= y)
	{
		if (((int)(x + 1)*(x + 1) + (int)(y - 0.5f)*(y - 0.5f) - r2) > 0)
		{
			y--;
		}
		x++;
		setPixel8(cx, cy, x, y, r, pixelColor, filled);
	}
}

void GraphicsAPI::drawText(const Point2& p, const char * text, const Pixel & pixelColor)
{
	int i = 0;
	while (*text)
	{
		drawChar(p.x + i++ * 8, p.y, *text - 32, pixelColor);
		text++;
	}
}

void GraphicsAPI::drawChar(int x, int y, char c, const Pixel& pixelColor)
{
	const unsigned char *table = FONT8x8;;
	table += (int)c * 8; // 8 = h

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			char _c = *table;
			if ((*table & (0x80 >> j)) != 0)
			{
				setPixel(Point2(x + i, y + j), pixelColor);
			}
		}
		table++;
	}
}

void GraphicsAPI::setVertexBuffer(Point3* buffer, unsigned int vertexCount)
{
	m_vertexBuffer = buffer;
	m_vertexCount = vertexCount;
}

void GraphicsAPI::setIndexBuffer(unsigned int* buffer, unsigned int indexCount)
{
	m_indexBuffer = buffer;
	m_indexCount = indexCount;
}

void GraphicsAPI::setNBuffer(Point3* buffer, unsigned int n)
{
	if (n >= 0 && n < 7)
	{
		m_Buffers[n] = buffer;
	}
}

bool GraphicsAPI::depthTest(int screenX, int screenY, float z)
{
	if (screenX < m_width && screenY < m_height && screenX >= 0 && screenY >= 0 && z <= 1.0f && z >= -1.0f)
	{
		z *= -1.0f; // like in DX11
		unsigned int y = screenY;

		if (m_flags & Y_AXIS_TOP)
		{
			y = (m_height - 1) - screenY;
		}
		if (z < m_Zbuffer[y * m_width + screenX])
		{
			m_Zbuffer[y * m_width + screenX] = z;
			return true;
		}
	}
	return false;
}

Point4 GraphicsAPI::sampleTexture(const Point2& tCoord)
{
	float x, y;
	//clamp
	if (tCoord.x > 1.0f)
	{
		x = 1.0f;
	}
	else if (tCoord.x < 0.0f)
	{
		x = 0.0f;
	}
	else
	{
		x = tCoord.x;
	}

	if (tCoord.y > 1.0f)
	{
		y = 1.0f;
	}
	else if (tCoord.y < 0.0f)
	{
		y = 0.0f;
	}
	else
	{
		y = tCoord.y;
	}
	// filtering
	Point3 result;
	if (0)
	{
		// nearest
		int _x = std::ceilf(x * ((float)m_textureW));
		int _y = std::ceilf(y * ((float)m_textureH));

		if (_x >= m_textureW)
		{
			_x = m_textureW - 1;
		}
		if (_y >= m_textureH)
		{
			_y = m_textureH - 1;
		}

		Pixel p = m_textureData[_y * m_textureW + _x];
		result.x = p.r;
		result.y = p.g;
		result.z = p.b;
	}
	else
	{
		// bilinear
		float u = x;
		float v = y;
		u *= (float)m_textureW;
		v *= (float)m_textureH;
		int _x = std::floorf(u);
		int _y = std::floorf(v);
		float u_ratio = u - _x;
		float v_ratio = v - _y;
		float u_opposite = 1 - u_ratio;
		float v_opposite = 1 - v_ratio;

		if (_x >= m_textureW)
		{
			_x = m_textureW - 1;
		}
		if (_y >= m_textureH)
		{
			_y = m_textureH - 1;
		}

		Pixel _Cxy = m_textureData[_y * m_textureW + _x];
		
		Pixel _Cx1y = m_textureData[_y * m_textureW + (_x + 1)% m_textureW];
		Pixel _Cxy1 = m_textureData[((_y + 1) % m_textureH) * m_textureW + _x];
		Pixel _Cx1y1 = m_textureData[((_y + 1)% m_textureH) * m_textureW + (_x + 1) % m_textureW];

		Point3	Cxy(_Cxy.r, _Cxy.g, _Cxy.b);
		Point3	Cx1y(_Cx1y.r, _Cx1y.g, _Cx1y.b);
		Point3	Cxy1(_Cxy1.r, _Cxy1.g, _Cxy1.b);
		Point3	Cx1y1(_Cx1y1.r, _Cx1y1.g, _Cx1y1.b);

		result = (Cxy * u_opposite + Cx1y * u_ratio) * v_opposite +
			(Cxy1 * u_opposite + Cx1y1 * u_ratio) * v_ratio;
	}
	return Point4(result.x, result.y, result.z, 0.0f);
}

void GraphicsAPI::Draw(unsigned int vertexCount, unsigned int startVertexLocation)
{
	if (startVertexLocation + vertexCount > m_vertexCount)
	{
		return;
	}

	ShaderIO p1, p2, p3;
	for (int i = startVertexLocation; i < vertexCount + startVertexLocation; i += 3)
	{
		// fetch
		p1.data[0] = m_vertexBuffer[i];
		p2.data[0] = m_vertexBuffer[i + 1];
		p3.data[0] = m_vertexBuffer[i + 2];

		for (int j = 0; j < 7; j++)
		{
			if (m_Buffers[j])
			{
				p1.data[j + 1] = m_Buffers[j][i + startVertexLocation];
				p2.data[j + 1] = m_Buffers[j][i + 1 + startVertexLocation];
				p3.data[j + 1] = m_Buffers[j][i + 2 + startVertexLocation];
			}
		}

		processTriangle(p1, p2, p3);
	}
}

void GraphicsAPI::DrawIndexed(unsigned int indexCount, unsigned int startIndexLocation, unsigned int baseVertexLocation)
{
	if (startIndexLocation + indexCount > m_indexCount)
	{
		return;
	}

	ShaderIO p1, p2, p3;
	for (int i = startIndexLocation; i < indexCount + startIndexLocation; i += 3)
	{
		// fetch
		p1.data[0] = m_vertexBuffer[m_indexBuffer[i] + baseVertexLocation];
		p2.data[0] = m_vertexBuffer[m_indexBuffer[i + 1] + baseVertexLocation];
		p3.data[0] = m_vertexBuffer[m_indexBuffer[i + 2] + baseVertexLocation];

		for (int j = 0; j < 7; j++)
		{
			if (m_Buffers[j])
			{
				p1.data[j + 1] = m_Buffers[j][m_indexBuffer[i] + baseVertexLocation];
				p2.data[j + 1] = m_Buffers[j][m_indexBuffer[i + 1] + baseVertexLocation];
				p3.data[j + 1] = m_Buffers[j][m_indexBuffer[i + 2] + baseVertexLocation];
			}
		}
		processTriangle(p1, p2, p3);
	}
}

void GraphicsAPI::processTriangle(ShaderIO& p1, ShaderIO& p2, ShaderIO& p3)
{
	m_pipeline.invokeVertexShader(p1);
	m_pipeline.invokeVertexShader(p2);
	m_pipeline.invokeVertexShader(p3);

	p1.data[0] /= p1.data[0].w;
	p2.data[0] /= p2.data[0].w;
	p3.data[0] /= p3.data[0].w;

	ScanLineRasterizer r(m_pipeline);
	r.setup(p1, p2, p3, m_width, m_height);
	r.rasterize();
}

void GraphicsAPI::setPixel4(int cx, int cy, int r, const Pixel& pixelColor)
{
	setPixel(Point2(cx, cy + r), pixelColor);
	setPixel(Point2(cx, cy - r), pixelColor);
	setPixel(Point2(cx + r, cy), pixelColor);
	setPixel(Point2(cx - r, cy), pixelColor);
}

void GraphicsAPI::setPixel8(int cx, int cy, int x, int y, int r, const Pixel& pixelColor, bool filled)
{
	if (!filled)
	{
		setPixel(Point2(cx + x, cy + y), pixelColor);
		setPixel(Point2(cx - x, cy + y), pixelColor);

		setPixel(Point2(cx + x, cy - y), pixelColor);
		setPixel(Point2(cx - x, cy - y), pixelColor);
		
		setPixel(Point2(cx + y, cy + x), pixelColor);
		setPixel(Point2(cx - y, cy + x), pixelColor);

		setPixel(Point2(cx + y, cy - x), pixelColor);
		setPixel(Point2(cx - y, cy - x), pixelColor);
	}
	else
	{
		drawLine(Point2(cx - x, cy + y), Point2(cx + x, cy + y), pixelColor);
		drawLine(Point2(cx - y, cy + x), Point2(cx + y, cy + x), pixelColor);
		drawLine(Point2(cx - y, cy - x), Point2(cx + y, cy - x), pixelColor);
		drawLine(Point2(cx - x, cy - y), Point2(cx + x, cy - y), pixelColor);

	}
}

}
