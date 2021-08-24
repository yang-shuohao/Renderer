// 引用EasyX图形库头文件
#include <graphics.h>		
#include <conio.h>
//glm数学相关头文件
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>


void DrawLine1(glm::vec2 P0, glm::vec2 P1, COLORREF color)
{
	float a = (P1.y - P0.y) / (P1.x - P0.x);
	float b = P0.y - a * P0.x;
	for (int x=P0.x;x<P1.x;x++)
	{
		int y = a * x + b;
		putpixel(x, y, color);
	}
}

void DrawLine2(glm::vec2 P0, glm::vec2 P1, COLORREF color)
{
	float a = (P1.y - P0.y) / (P1.x - P0.x);
	float y = P0.y;
	for (int x = P0.x;x < P1.x;x++)
	{
		putpixel(x, y, color);
		y = y + a;
	}
}

void DrawLine3(glm::vec2 P0, glm::vec2 P1, COLORREF color)
{
	if (P0.x > P1.x)
	{
		std::swap(P0, P1);
	}
	float a = (P1.y - P0.y) / (P1.x - P0.x);
	float y = P0.y;
	for (int x = P0.x;x < P1.x;x++)
	{
		putpixel(x, y, color);
		y = y + a;
	}
}

void DrawLine4(glm::vec2 P0, glm::vec2 P1, COLORREF color)
{
	if (P0.y > P1.y)
	{
		std::swap(P0, P1);
	}
	float a = (P1.x - P0.x) / (P1.y - P0.y);
	float x = P0.x;
	for (int y = P0.y;y < P1.y;y++)
	{
		putpixel(x, y, color);
		x = x + a;
	}
}

void DrawLine5(glm::vec2 P0, glm::vec2 P1, COLORREF color)
{
	float dx = P1.x - P0.x;
	float dy = P1.y - P0.y;
	if (glm::abs(dx) > glm::abs(dy))
	{
		if (P0.x > P1.x)
		{
			std::swap(P0, P1);
		}
		float a = dy / dx;
		float y = P0.y;
		for (int x = P0.x;x < P1.x;x++)
		{
			putpixel(x, y, color);
			y = y + a;
		}
	}
	else
	{
		if (P0.y > P1.y)
		{
			std::swap(P0, P1);
		}
		float a = dx / dy;
		float x = P0.x;
		for (int y = P0.y;y < P1.y;y++)
		{
			putpixel(x, y, color);
			x = x + a;
		}
	}
	
}

std::vector<float> Interpolate(float i0, float d0, float i1, float d1)
{
	std::vector<float> values;
	if (glm::abs(i0 - i1)<=1e-6)
	{
		values.push_back(d0);
		return values;
	}
	float a = (d1 - d0) / (i1 - i0);
	float d = d0;
	for (int i = i0;i < i1;i++)
	{
		values.push_back(d);
		d = d + a;
	}
	return values;
}

void DrawLine6(glm::vec2 P0, glm::vec2 P1, COLORREF color)
{
	if (glm::abs(P1.x-P0.x) > glm::abs(P1.y-P0.y))
	{
		if (P0.x > P1.x)
		{
			std::swap(P0, P1);
		}
		std::vector<float> ys = Interpolate(P0.x,P0.y,P1.x,P1.y);
		for (int x = P0.x;x < P1.x;x++)
		{
			putpixel(x, ys[x-P0.x], color);
		}
	}
	else
	{
		if (P0.y > P1.y)
		{
			std::swap(P0, P1);
		}
		std::vector<float> xs = Interpolate(P0.y, P0.x, P1.y, P1.x);
		for (int y = P0.y;y < P1.y;y++)
		{
			putpixel(xs[y - P0.y], y, color);
		}
	}

}

int main()
{
	initgraph(640, 640);	// 创建绘图窗口，大小为 640x480 像素

	DrawLine6(glm::vec2(100,100), glm::vec2(500,100), RED);
	DrawLine6(glm::vec2(100, 100), glm::vec2(100, 500), GREEN);

	_getch();				// 按任意键继续
	closegraph();			// 关闭绘图窗口
	return 0;
}