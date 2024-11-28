#include <algorithm>
#include "Canvas.h"
#include <math.h>
#include <vector>


#define MIN(left, right) ((left) < (right) ? (left) : (right))
#define MAX(left, right) ((left) > (right) ? (left) : (right))


namespace GT
{
	void GT::Canvas::drawLine(Point pt1, Point pt2)
	{
		int disY = abs(pt2.m_y - pt1.m_y);
		int disX = abs(pt2.m_x - pt1.m_x);

		int xNow = pt1.m_x;
		int yNow = pt1.m_y;

		int stepX = 0;
		int stepY = 0;

		// 判断两个方向步进的正负
		if (pt1.m_x < pt2.m_x)
		{
			// 正常步进
			stepX = 1;
		}
		else
		{
			stepX = -1;
		}

		if (pt1.m_y < pt2.m_y)
		{
			// 正常步进
			stepY = 1;
		}
		else
		{
			stepY = -1;
		}

		// 对比xy偏移量，决定步进的方向，选取x or y
		int sumStep = disX;
		bool useXStep = true;
		if (disX < disY)
		{
			sumStep = disY;
			useXStep = false;
			SWAP_INT(disX, disY);
		}

		// 初始化P值
		int p = 2 * disY - disX;

		RGBA _color;
		for (int i = 0; i <= sumStep; ++i)
		{
			float _scale = 0;
			if (useXStep)
			{
				if (pt2.m_x == pt1.m_x)
				{
					_scale = 0;
				}
				else
				{
					_scale = (float)(xNow - pt1.m_x) / (float)(pt2.m_x - pt1.m_x);
				}
			}
			else
			{
				if (pt2.m_y == pt1.m_y)
				{
					_scale = 0;
				}
				else
				{
					_scale = (float)(yNow - pt1.m_y) / (float)(pt2.m_y - pt1.m_y);
				}
			}

			if (m_enableTexture)
			{
				floatV2 _uv = uvLerp(pt1.m_uv, pt2.m_uv, _scale);
				if (m_texture)
				{
					_color = m_texture->getColorByUV(_uv.x, _uv.y, m_texType);
				}
				else
				{
					_color = colorLerp(pt1.m_color, pt2.m_color, _scale);
				}
			}
			else
			{
				_color = colorLerp(pt1.m_color, pt2.m_color, _scale);
			}
			drawPoint(xNow, yNow, _color);

			if (p >= 0)
			{
				if (useXStep)
				{
					yNow += stepY;
				}
				else
				{
					xNow += stepX;
				}
				p = p - 2 * disX;
			}
			// 步进主坐标
			if (useXStep)
			{
				xNow += stepX;
			}
			else
			{
				yNow += stepY;
			}

			p = p + 2 * disY;
		}
	}

	void Canvas::drawTriangle(Point pt1, Point pt2, Point pt3)
	{
		std::vector<Point> pVec;
		pVec.push_back(pt1);
		pVec.push_back(pt2);
		pVec.push_back(pt3);

		GT_RECT _rect(0, m_width, 0, m_height);
		// 是否双方相交
		while (true)
		{
			if (judgeInRect(pt1, _rect) || judgeInRect(pt2, _rect) || judgeInRect(pt3, _rect))
			{
				break;
			}
			Point rpt1(0, 0, RGBA());
			Point rpt2(m_width, 0, RGBA());
			Point rpt3(0, m_height, RGBA());
			Point rpt4(m_width, m_height, RGBA());

			if (judgeInTriangle(rpt1, pVec) ||
				judgeInTriangle(rpt2, pVec) ||
				judgeInTriangle(rpt3, pVec) ||
				judgeInTriangle(rpt4, pVec))
			{
				break;
			}
			return;
		}

		std::sort(pVec.begin(), pVec.end(), [](const Point& pt1, const Point& pt2) {return pt1.m_y > pt2.m_y; });

		Point ptMax = pVec[0];
		Point ptMid = pVec[1];
		Point ptMin = pVec[2];

		if (ptMax.m_y == ptMid.m_y)
		{
			drawTrangleFlat(ptMax, ptMid, ptMin);
			return;
		}

		if (ptMin.m_y == ptMid.m_y)
		{
			drawTrangleFlat(ptMin, ptMid, ptMax);
			return;
		}

		float k = 0.0;
		if (ptMax.m_x != ptMin.m_x)
		{
			k = (float)(ptMax.m_y - ptMin.m_y) / (float)(ptMax.m_x - ptMin.m_x);
		}
		float b = (float)(ptMax.m_y) - (float)ptMax.m_x * k;

		Point npt(0, 0, RGBA(255, 0, 0));
		npt.m_y = ptMid.m_y;
		if (k == 0)
		{
			npt.m_x = ptMax.m_x;
		}
		else
		{
			npt.m_x = ((float)npt.m_y - b) / k;
		}
		float s = (float)(npt.m_y - ptMin.m_y) / (float)(ptMax.m_y - ptMin.m_y);
		npt.m_color = colorLerp(ptMin.m_color, ptMax.m_color, s);
		npt.m_uv = uvLerp(ptMin.m_uv, ptMax.m_uv, s);

		drawTrangleFlat(ptMid, npt, ptMax);
		drawTrangleFlat(ptMid, npt, ptMin);

		return;
	}

	void Canvas::drawTrangleFlat(Point ptFlat1, Point ptFlat2, Point pt)
	{
		float k1 = 0.0;
		float k2 = 0.0;

		if (ptFlat1.m_x != pt.m_x)
		{
			k1 = (float)(ptFlat1.m_y - pt.m_y) / (float)(ptFlat1.m_x - pt.m_x);
		}

		if (ptFlat2.m_x != pt.m_x)
		{
			k2 = (float)(ptFlat2.m_y - pt.m_y) / (float)(ptFlat2.m_x - pt.m_x);
		}

		float b1 = (float)pt.m_y - (float)pt.m_x * k1;
		float b2 = (float)pt.m_y - (float)pt.m_x * k2;

		int yStart = MIN(pt.m_y, ptFlat1.m_y);
		int yEnd = MAX(pt.m_y, ptFlat1.m_y);

		// 颜色插值相关
		RGBA colorStart1;
		RGBA colorEnd1;
		RGBA colorStart2;
		RGBA colorEnd2;

		floatV2 uvStart1;
		floatV2 uvEnd1;
		floatV2 uvStart2;
		floatV2 uvEnd2;

		if (pt.m_y < ptFlat1.m_y)
		{
			yStart = pt.m_y;
			yEnd = ptFlat1.m_y;

			colorStart1 = pt.m_color;
			colorEnd1 = ptFlat1.m_color;
			colorStart2 = pt.m_color;
			colorEnd2 = ptFlat2.m_color;

			uvStart1 = pt.m_uv;
			uvEnd1 = ptFlat1.m_uv;
			uvStart2 = pt.m_uv;
			uvEnd2 = ptFlat2.m_uv;
		}
		else
		{
			yStart = ptFlat1.m_y;
			yEnd = pt.m_y;
			
			colorStart1 = ptFlat1.m_color;
			colorEnd1 = pt.m_color;
			colorStart2 = ptFlat2.m_color;
			colorEnd2 = pt.m_color;

			uvStart1 = ptFlat1.m_uv; 
			uvEnd1 = pt.m_uv;
			uvStart2 = ptFlat2.m_uv;
			uvEnd2 = pt.m_uv;
		}
		float yColorStep = 1.0 / (float)(yEnd - yStart);
		int yColorStart = yStart;

		if (yStart < 0)
		{
			yStart = 0;
		}
		if (yEnd > m_height)
		{
			yEnd = m_height - 1;
		}

		for (int y = yStart; y <= yEnd; ++y)
		{
			int x1 = 0;
			if (k1 == 0)
			{
				x1 = ptFlat1.m_x;
			}
			else
			{
				x1 = ((float)y - b1) / k1;
			}

			// 剪裁x1
			if (x1 < 0)
			{
				x1 = 0;
			}
			if (x1 > m_width)
			{
				x1 = m_width - 1;
			}

			int x2 = 0;
			if (k2 == 0)
			{
				x2 = ptFlat2.m_x;
			}
			else
			{
				x2 = ((float)y - b2) / k2;
			}

			// 剪裁x2
			if (x2 < 0)
			{
				x2 = 0;
			}
			if (x2 > m_width)
			{
				x2 = m_width - 1;
			}

			float s = (float)(y - yColorStart) * yColorStep;
			RGBA _color1 = colorLerp(colorStart1, colorEnd1, s);
			RGBA _color2 = colorLerp(colorStart2, colorEnd2, s);

			floatV2 _uv1 = uvLerp(uvStart1, uvEnd1, s);
			floatV2 _uv2 = uvLerp(uvStart2, uvEnd2, s);
			Point pt1(x1, y, _color1, _uv1);
			Point pt2(x2, y, _color2, _uv2);

			drawLine(pt1, pt2);
		}
	}

	bool Canvas::judgeInRect(Point pt, GT_RECT _rect)
	{
		if (pt.m_x > _rect.m_left && pt.m_x < _rect.m_right && pt.m_y > _rect.m_top && pt.m_y < _rect.m_bottom)
		{
			return true;
		}
		return false;
	}

	bool Canvas::judgeInTriangle(Point pt, std::vector<Point> _ptArray)
	{
		Point pt1 = _ptArray[0];
		Point pt2 = _ptArray[1];
		Point pt3 = _ptArray[2];

		int x = pt.m_x;
		int y = pt.m_y;

		// 计算三条边的斜率和截距
		float k1 = 0, k2 = 0, k3 = 0;
		float b1 = 0, b2 = 0, b3 = 0;

		// 防止除零错误，检查是否为垂直线
		if (pt1.m_x != pt2.m_x) {
			k1 = (float)(pt1.m_y - pt2.m_y) / (float)(pt1.m_x - pt2.m_x);
			b1 = (float)pt1.m_y - k1 * (float)pt1.m_x;
		}

		if (pt1.m_x != pt3.m_x) {
			k2 = (float)(pt1.m_y - pt3.m_y) / (float)(pt1.m_x - pt3.m_x);
			b2 = (float)pt1.m_y - k2 * (float)pt1.m_x;
		}

		if (pt2.m_x != pt3.m_x) {
			k3 = (float)(pt2.m_y - pt3.m_y) / (float)(pt2.m_x - pt3.m_x);
			b3 = (float)pt2.m_y - k3 * (float)pt2.m_x;
		}

		// 判断当前点是否在三角形内部
		// 通过比较点与三角形三条边的关系来判断
		float judge1 = (y - (k1 * x + b1)) * (pt3.m_y - (k1 * pt3.m_x + b1));
		float judge2 = (y - (k2 * x + b2)) * (pt2.m_y - (k2 * pt2.m_x + b2));
		float judge3 = (y - (k3 * x + b3)) * (pt1.m_y - (k3 * pt1.m_x + b3));

		// 如果所有判断结果都为正或零，说明点在三角形内部
		if (judge1 >= 0 && judge2 >= 0 && judge3 >= 0)
		{
			return true;
		}
		return false;
	}

	void Canvas::drawImage(int _x, int _y, Image* _image)
	{
		for (int u = 0; u < _image->getWidth(); u++)
		{
			for (int v = 0; v < _image->getHeight(); ++v)
			{
				RGBA _srcColor = _image->getColor(u, v);
				if (!m_useBlend)
				{
					drawPoint(_x + u, _y + v, _srcColor);
				}
				else
				{
					RGBA _dstColor = getColor(_x + u, _y + v);
					float _srcAlpha = (float)_srcColor.m_a / 255.0f;
					float _dstAlpha = (float)_dstColor.m_a / 255.0f;

					// Alpha blending formula: finalColor = srcAlpha * srcColor + (1 - srcAlpha) * dstColor
					RGBA _finalColor;
					_finalColor.m_r = _srcAlpha * _srcColor.m_r + (1.0f - _srcAlpha) * _dstColor.m_r;
					_finalColor.m_g = _srcAlpha * _srcColor.m_g + (1.0f - _srcAlpha) * _dstColor.m_g;
					_finalColor.m_b = _srcAlpha * _srcColor.m_b + (1.0f - _srcAlpha) * _dstColor.m_b;
					_finalColor.m_a = std::min(1.0f, _srcAlpha + _dstAlpha) * 255;  // Combined alpha

					drawPoint(_x + u, _y + v, _finalColor);
				}

			}
		}
	}

	void Canvas::setAlphaLimit(byte _limit)
	{
		m_alphaLimit = _limit;
	}

	void Canvas::setBlend(bool _useBlend)
	{
		m_useBlend = _useBlend;
	}

	void Canvas::enableTexture(bool _enable)
	{
		m_enableTexture = _enable;
	}

	void Canvas::bindTexture(const Image* _image)
	{
		m_texture = _image;
	}

	void Canvas::setTextureType(Image::TEXTURE_TYPE _type)
	{
		m_texType = _type;
	}

	//void Canvas::drawTriangle(Point pt1, Point pt2, Point pt3)
	//{
	//	RGBA _color(255, 0, 0);
	//	// 构建包围体
	//	int left = MIN(pt3.m_x, MIN(pt1.m_x, pt2.m_x));
	//	int top = MIN(pt3.m_y, MIN(pt1.m_y, pt2.m_y));
	//	int right = MAX(pt3.m_x, MAX(pt1.m_x, pt2.m_x));
	//	int bottom = MAX(pt3.m_y, MAX(pt1.m_y, pt2.m_y));

	//	// 剪裁屏幕
	//	left = left < 0 ? 0 : left;
	//	top = top < 0 ? 0 : top;
	//	right = right > (m_width - 1) ? (m_width - 1) : right;
	//	bottom = bottom > (m_height - 1) ? (m_height - 1) : bottom;

	//	// 计算直线参数值
	//	float k1 = (float)(pt2.m_y - pt3.m_y) / (float)(pt2.m_x - pt3.m_x);
	//	float k2 = (float)(pt1.m_y - pt3.m_y) / (float)(pt1.m_x - pt3.m_x);
	//	float k3 = (float)(pt2.m_y - pt1.m_y) / (float)(pt2.m_x - pt1.m_x);

	//	// 计算直线的b值
	//	float b1 = (float)pt2.m_y - k1 * (float)pt2.m_x;
	//	float b2 = (float)pt3.m_y - k2 * (float)pt3.m_x;
	//	float b3 = (float)pt1.m_y - k3 * (float)pt1.m_x;

	//	// 循环判断
	//	for (int x = left; x <= right; x++)
	//	{
	//		for (int y = top; y <= bottom; y++)
	//		{
	//			// 判断当前点是否在三角形范围内
	//			float judge1 = (y - (k1 * x + b1)) * (pt1.m_y - (k1 * pt1.m_x + b1));
	//			float judge2 = (y - (k2 * x + b2)) * (pt2.m_y - (k2 * pt2.m_x + b2));
	//			float judge3 = (y - (k3 * x + b3)) * (pt3.m_y - (k3 * pt3.m_x + b3));

	//			if (judge1 >= 0 && judge2 >= 2 && judge3 >= 0)
	//			{
	//				drawPoint(x, y, _color);
	//			}
	//		}
	//	}
	//}
}

