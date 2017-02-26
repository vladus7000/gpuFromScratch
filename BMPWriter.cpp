#include "BMPWriter.h"
#include <fstream>

__pragma(pack(push, 1))
struct BMPHeader
{
	char magic[2];
	unsigned int size;
	unsigned int res;
	unsigned int offset;

	unsigned int dibSize;
	unsigned int w;
	unsigned int h;
	unsigned short int plane;
	unsigned short int bpp;
	unsigned int format;
	unsigned int rawSize;
	unsigned int pmh;
	unsigned int pmv;
	unsigned int colors;
	unsigned int impColors;
};
__pragma(pack(pop))

namespace writer
{
	void BMPWriter::writeRGB_Float(const std::string & filename, const void* rawRGB, unsigned int w, unsigned int h)
	{
		struct RGB
		{
			float r;
			float g;
			float b;
		};

		BMPHeader header;
		header.magic[0] = 'B';
		header.magic[1] = 'M';
		header.size = 54 + w * h * 3;
		header.res = 0;
		header.offset = 54;
		header.dibSize = 40;
		header.w = w;
		header.h = h;
		header.plane = 1;
		header.bpp = 24;
		header.format = 0; // BI_RGB
		header.rawSize = w * h * 3;
		header.pmv = 2835;
		header.pmh = 2835;
		header.colors = 0;
		header.impColors = 0;

		std::ofstream file;
		file.open(filename, std::ios::binary);
		file.write((char*)&header, 54);

		unsigned char pad[3] = { 0,0,0 };
		int padSize = (4 - (w * 3) % 4) % 4;

		for (int j = h-1; j >= 0; j--)
		{
			for (int i = 0; i < w; i++)
			{
				const RGB* rgbData = static_cast<const RGB*>(rawRGB);

				const RGB pixel = rgbData[j*w + i];

				unsigned char data[3];

				int tmp = pixel.r * 255;
				if (tmp > 255) tmp = 255;
				data[2] = tmp;

				tmp = pixel.g * 255;
				if (tmp > 255) tmp = 255;
				data[1] = tmp;

				tmp = pixel.b * 255;
				if (tmp > 255) tmp = 255;
				data[0] = tmp;

				file.write((char*)data, 3);

			}
			file.write((char*)pad, padSize);
		}

		file.close();

	}
}