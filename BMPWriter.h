#pragma once
#include <string>

namespace writer
{
	class BMPWriter
	{
	public:
		void writeRGB_Float(const std::string& filename, const void* rawRGB, unsigned int w, unsigned int h);
	};
}