#pragma once
#include "GTMATH.hpp"
#include <string.h>
#include <vector>
#include "Image.h"

namespace GT
{
	struct Point
	{
	public:
		float		m_x;
		float		m_y;
		float		m_z;
		RGBA	m_color;
		floatV2 m_uv;
		Point(float _x = 0, float _y = 0, float _z = 0, RGBA _color = RGBA(0, 0, 0, 0), floatV2 _uv = floatV2(0.0, 0.0))
		{
			m_x = _x;
			m_y = _y;
			m_z = _z;
			m_color = _color;
			m_uv = _uv;
		}
		~Point()
		{

		}
	};

	enum DATA_TYPE
	{
		GT_FLOAT = 0,
		GT_INT = 1
	};

	enum DRAW_MODE
	{
		GT_LINE = 0,
		GT_TRIANGLE = 1
	};

	struct DataElement
	{
		int			m_size;
		DATA_TYPE	m_type;
		int			m_stride;
		byte*		m_data;

		DataElement()
		{
			m_size = -1;
			m_type = GT_FLOAT;
			m_stride = -1;
			m_data = NULL;
		}
	};

	struct Statement
	{
		// 状态变量
		byte	m_alphaLimit;	// 大于此值的像素才可以进行绘制
		bool					m_useBlend;		// 是否进行颜色混合
		bool					m_enableTexture;// 是否启用纹理切图
		const Image*			m_texture;
		Image::TEXTURE_TYPE		m_texType;

		DataElement				m_vertexData;
		DataElement				m_colorData;
		DataElement				m_texCoordData;

		Statement()
		{
			m_alphaLimit = 0;
			m_useBlend = false;
			m_enableTexture = false;
			m_texture = NULL;
			m_texType = Image::TX_REPEAT;
		}
	};

	class Canvas
	{
	private:
		int		m_width;
		int		m_height;
		RGBA*	m_buffer;
		float*	m_zBuffer;
		
		Statement m_state;
	public:
		Canvas(int _width, int _height, void* _buffer)
		{
			if (_width <= 0 || _height <= 0)
			{
				m_width = -1;
				m_height = -1;
				m_buffer = nullptr;
			}

			m_width = _width;
			m_height = _height;
			m_buffer = (RGBA*)_buffer;
			m_zBuffer = new float[_width * _height];
			m_state.m_useBlend = false;
			m_state.m_enableTexture = false;
		}

		~Canvas()
		{
			delete[] m_zBuffer;
		}

		void clear()
		{
			if (m_buffer != NULL)
			{
				memset(m_buffer, 0, sizeof(RGBA) * m_width * m_height);
				memset(m_zBuffer, 255, sizeof(float) * m_width * m_height);
			}
		}

		//*******画点操作
		void drawPoint(Point _pt)
		{
			if (_pt.m_x < 0 || _pt.m_x >= m_width || _pt.m_y < 0 || _pt.m_y >= m_height)
			{
				// 超出范围，不画
				return;
			}
			int _index = _pt.m_y * m_width + _pt.m_x;

			if (_pt.m_z > m_zBuffer[_index])
			{
				return;
			}
			m_zBuffer[_index] = _pt.m_z;
			m_buffer[_index] = _pt.m_color;
		}

		RGBA getColor(int x, int y)
		{
			if (x < 0 || x >= m_width || y < 0 || y >= m_height)
			{
				return RGBA();
			}
			return m_buffer[y * m_width + x];
		}

		inline RGBA colorLerp(RGBA _color1, RGBA _color2, float _scale)
		{
			RGBA _color;
			_color.m_r = _color1.m_r + (float)(_color2.m_r - _color1.m_r) * _scale;
			_color.m_g = _color1.m_g + (float)(_color2.m_g - _color1.m_g) * _scale;
			_color.m_b = _color1.m_b + (float)(_color2.m_b - _color1.m_b) * _scale;
			_color.m_a = _color1.m_a + (float)(_color2.m_a - _color1.m_a) * _scale;
			return _color;
		}

		inline floatV2 uvLerp(floatV2 _uv1, floatV2 _uv2, float _scale)
		{
			floatV2 _uv;
			_uv.x = _uv1.x + (_uv2.x - _uv1.x) * _scale;
			_uv.y = _uv1.y + (_uv2.y - _uv1.y) * _scale;
			return _uv;
		}

		inline float zLerp(float _z1, float _z2, float _scale)
		{
			return _z1 + (_z2 - _z1) * _scale;
		}

		//*******画线操作
		void drawLine(Point pt1, Point pt2);
		void drawTriangle(Point pt1, Point pt2, Point pt3);
		void drawTrangleFlat(Point ptFlat1, Point ptFlat2, Point pt);

		bool judgeInRect(Point pt, GT_RECT _rect);
		bool judgeInTriangle(Point pt, std::vector<Point> _ptArray);
		bool judgeLineAndRect(int _x1, int _x2, int& _x1Cut, int& _x2Cut);

		//*******图片操作
		void drawImage(int _x, int _y, Image* _image);
		void setAlphaLimit(byte _limit);
		void setBlend(bool _useBlend);

		//**********纹理
		void enableTexture(bool _enable);

		void bindTexture(const Image* _image);

		void setTextureType(Image::TEXTURE_TYPE _type);

		void gtVertexPointer(int _size, DATA_TYPE _type, int _stride, byte* _data);
		void gtColorPointer(int _size, DATA_TYPE _type, int _stride, byte* _data);
		void gtTexCoordPointer(int _size, DATA_TYPE _type, int _stride, byte* _data);

		void gtDrawArray(DRAW_MODE _mode, int _first, int _count);
	};
}
