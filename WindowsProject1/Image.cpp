#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace GT
{
	Image* GT::Image::readFromFile(const char* _fileName)
	{
		int		_picType = 0;
		int		_width = 0;
		int		_height = 0;

		// stbimage读入的图片是反过来的
		stbi_set_flip_vertically_on_load(true);

		unsigned char* bits = stbi_load(_fileName, &_width, &_height, &_picType, STBI_rgb_alpha);

		for (int i = 0; i < _width * _height * 4; i += 4)
		{
			byte tmp = bits[i];
			bits[i] = bits[i + 2];
			bits[i + 2] = tmp;
		}

		Image* _image = new Image(_width, _height, bits);

		stbi_image_free(bits);
		return _image;
	}
}

