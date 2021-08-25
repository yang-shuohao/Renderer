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
	if (glm::abs(i0 - i1)<1e-6)
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

void DrawLine(glm::vec2 P0, glm::vec2 P1, COLORREF color)
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

void DrawWireframeTriangle(glm::vec2 P0, glm::vec2 P1, glm::vec2 P2, COLORREF color)
{
	DrawLine(P0, P1, color);
	DrawLine(P1, P2, color);
	DrawLine(P2, P0, color);
}

void DrawFilledTriangle(glm::vec2 P0, glm::vec2 P1, glm::vec2 P2, COLORREF color)
{
	//排序顶点 P0.y <= P1.y <= P2.y
	if (P1.y < P0.y) { std::swap(P1, P0); }
	if (P2.y < P0.y) { std::swap(P2, P0); }
	if (P2.y < P1.y) { std::swap(P2, P1); }
	              
// 				   P2 |\
// 	                  | \
// 					  |  \ P1
// 					  |  /
// 					  | /
// 				   P0 |/			  			   

	//P0P1边x坐标数组
	std::vector<float> x01 = Interpolate(P0.y, P0.x, P1.y, P1.x);
	//P1P2边x坐标数组
	std::vector<float> x12 = Interpolate(P1.y, P1.x, P2.y, P2.x);
	//P0P2边x坐标数组
	std::vector<float> x02 = Interpolate(P0.y, P0.x, P2.y, P2.x);

	//【注意】去掉重复坐标，P0P1和P1P2重复了P1
	//x01.pop_back();
	//x012=x01+x12 x012代表P0P1和P1P2两条边的x坐标数组
	x01.insert(x01.end(), x12.begin(), x12.end());
	std::vector<float> x012(x01);

	float m = glm::floor(x012.size() / 2);
	std::vector<float> x_left;
	std::vector<float> x_right;
//	      第一种情况
// 		  P2 |\
// 	         | \
// 			 |  \ P1
// 			 |  /
// 			 | /
// 		  P0 |/		
	if (x02[m] < x012[m])
	{
		x_left = x02;
		x_right = x012;
	}
//	      第二种情况
// 		    /| P2
// 	       / | 
// 	   p1 /  | 
// 		  \	 |
// 		   \ |
// 		    \| P0	
	else
	{						
		x_left = x012;					
		x_right = x02;
	}
	//从左到右填充
	for (int y = P0.y;y < P2.y;y++)
	{
		for (int x = x_left[y - P0.y];x < x_right[y - P0.y];x++)
		{
			putpixel(x, y, color);
		}
	}
}

int main()
{
	initgraph(640, 640);	// 创建绘图窗口，大小为 640x480 像素

	glm::vec2 P0(200, 200);
	glm::vec2 P1(200, 500);
	glm::vec2 P2(350, 350);

	DrawFilledTriangle(P0, P1, P2, RED);

	_getch();				// 按任意键继续
	closegraph();			// 关闭绘图窗口
	return 0;
}