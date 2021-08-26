// 引用EasyX图形库头文件
#include <graphics.h>		
#include <conio.h>
//glm数学相关头文件
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>


//插值
std::vector<float> Interpolate(float i0, float d0, float i1, float d1)
{
	std::vector<float> values;
	if (glm::abs(i0 - i1) < 1e-6)
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

//画线
void DrawLine(glm::vec2 P0, glm::vec2 P1, COLORREF color)
{
	if (glm::abs(P1.x - P0.x) > glm::abs(P1.y - P0.y))
	{
		if (P0.x > P1.x)
		{
			std::swap(P0, P1);
		}
		std::vector<float> ys = Interpolate(P0.x, P0.y, P1.x, P1.y);
		for (int x = P0.x;x < P1.x;x++)
		{
			putpixel(x, ys[x - P0.x], color);
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

//画线框三角形
void DrawWireframeTriangle(glm::vec2 P0, glm::vec2 P1, glm::vec2 P2, COLORREF color)
{
	DrawLine(P0, P1, color);
	DrawLine(P1, P2, color);
	DrawLine(P2, P0, color);
}

//画填充三角形
void DrawFilledTriangle(glm::vec2 P0, glm::vec2 P1, glm::vec2 P2, COLORREF color)
{
	//排序顶点 P0.y <= P1.y <= P2.y
	if (P1.y < P0.y) { std::swap(P1, P0); }
	if (P2.y < P0.y) { std::swap(P2, P0); }
	if (P2.y < P1.y) { std::swap(P2, P1); }

	//------------------P2|\
	//--------------------| \
	//--------------------|  \ P1
	//--------------------|  /
	//--------------------| /
	//------------------P0|/			  			   

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
	//-------第一种情况
	//---------P2|\
	//-----------| \
	//-----------|  \ P1
	//-----------|  /
	//-----------| /
	//---------P0|/		
	if (x02[m] < x012[m])
	{
		x_left = x02;
		x_right = x012;
	}
	//-------第二种情况
	//----------/|P2
	//---------/ | 
	//------p1/  | 
	//--------\	 |
	//---------\ |
	//----------\|P0		
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

struct Vertex
{
	glm::vec4 position;
	float color;
};

//画着色三角形
void DrawShadedTriangle(Vertex P0, Vertex P1, Vertex P2, COLORREF color)
{

	BYTE r = GetRValue(color);
	BYTE g = GetGValue(color);
	BYTE b = GetBValue(color);

	//排序顶点 P0.y <= P1.y <= P2.y
	if (P1.position.y < P0.position.y) { std::swap(P1, P0); }
	if (P2.position.y < P0.position.y) { std::swap(P2, P0); }
	if (P2.position.y < P1.position.y) { std::swap(P2, P1); }

	//------------------P2|\
	//--------------------| \
	//--------------------|  \ P1
	//--------------------|  /
	//--------------------| /
	//------------------P0|/			  			   

		//P0P1边x坐标数组
	std::vector<float> x01 = Interpolate(P0.position.y, P0.position.x, P1.position.y, P1.position.x);
	std::vector<float> h01 = Interpolate(P0.position.y, P0.color, P1.position.y, P1.color);

	//P1P2边x坐标数组
	std::vector<float> x12 = Interpolate(P1.position.y, P1.position.x, P2.position.y, P2.position.x);
	std::vector<float> h12 = Interpolate(P1.position.y, P1.color, P2.position.y, P2.color);

	//P0P2边x坐标数组
	std::vector<float> x02 = Interpolate(P0.position.y, P0.position.x, P2.position.y, P2.position.x);
	std::vector<float> h02 = Interpolate(P0.position.y, P0.color, P2.position.y, P2.color);

	//【注意】去掉重复坐标，P0P1和P1P2重复了P1
	//x01.pop_back();
	//x012=x01+x12 x012代表P0P1和P1P2两条边的x坐标数组
	x01.insert(x01.end(), x12.begin(), x12.end());
	std::vector<float> x012(x01);

	h01.insert(h01.end(), h12.begin(), h12.end());
	std::vector<float> h012(h01);

	float m = glm::floor(x012.size() / 2);
	std::vector<float> x_left;
	std::vector<float> x_right;

	std::vector<float> h_left;
	std::vector<float> h_right;

	//-------第一种情况
	//---------P2|\
	//-----------| \
	//-----------|  \ P1
	//-----------|  /
	//-----------| /
	//---------P0|/	

	if (x02[m] < x012[m])
	{
		x_left = x02;
		x_right = x012;

		h_left = h02;
		h_right = h012;
	}
	//-------第二种情况
	//----------/|P2
	//---------/ | 
	//------p1/  | 
	//--------\	 |
	//---------\ |
	//----------\|P0	
	else
	{
		x_left = x012;
		x_right = x02;

		h_left = h012;
		h_right = h02;
	}
	//从左到右填充
	for (int y = P0.position.y; y < P2.position.y; y++)
	{
		float x_l = x_left[y - P0.position.y];
		float x_r = x_right[y - P0.position.y];

		std::vector<float> h_segment = Interpolate(x_l, h_left[y - P0.position.y], x_r, h_right[y - P0.position.y]);

		for (int x = x_left[y - P0.position.y]; x < x_right[y - P0.position.y]; x++)
		{
			COLORREF shaded_color = RGB(r * h_segment[x - x_l], g * h_segment[x - x_l], b * h_segment[x - x_l]);
			putpixel(x, y, shaded_color);
		}
	}
}

int main()
{
	int ScreenWidth = 800;
	int ScreenHeight = 600;

	initgraph(ScreenWidth, ScreenHeight);	// 创建绘图窗口，大小为 640x480 像素

	Vertex v0;
	v0.position = { -0.5f, -0.5f ,0.0f,1.0f };
	v0.color = 1.0f;
	Vertex v1;
	v1.position = { 0.5f, -0.5f,0.0f,1.0f };
	v1.color = 0.5f;
	Vertex v2;
	v2.position = { 0.0f, 0.5f ,0.0f,1.0f };
	v2.color = 0.0f;


	ExMessage m;		// 定义消息变量

	float radians = 0.0f;

	while (true)
	{
		// 获取一条鼠标或按键消息
		m = getmessage(EM_MOUSE | EM_KEY | EM_WINDOW);

		switch (m.message)
		{
		case WM_PAINT:
			radians += 10.0f;
			break;

		case WM_LBUTTONDOWN:
			radians += 10.0f;
			break;

		case WM_KEYDOWN:
			if (m.vkcode == VK_ESCAPE)
				return 0;	// 按 ESC 键退出程序
		}
		{
			glm::mat4 sm = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
			glm::mat4 rm = glm::rotate(glm::mat4(1.0f), glm::radians(radians), glm::vec3(0, 1, 0));
			glm::mat4 tm = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 1));
			//构造模型矩阵
			glm::mat4 model = tm * rm * sm;
			//构造视图矩阵
			glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
			//构造透视投影矩阵
			glm::mat4 perspective = glm::perspective(glm::radians(90.0f), (float)ScreenWidth / (float)ScreenHeight, 0.1f, 100.0f);
			//构造MVP矩阵
			glm::mat4 mvp = perspective * view * model;
			//对顶点进行MVP矩阵变换
			v0.position = mvp * v0.position;
			v1.position = mvp * v1.position;
			v2.position = mvp * v2.position;

			//透视除法
			float reciprocalW0 = 1 / v0.position.w;
			float reciprocalW1 = 1 / v1.position.w;
			float reciprocalW2 = 1 / v2.position.w;

			//屏幕映射
			v0.position.x = (v0.position.x * reciprocalW0 + 1.0f) * 0.5f * ScreenWidth;
			v0.position.y = (1.0f - v0.position.y * reciprocalW0) * 0.5f * ScreenHeight;
			v1.position.x = (v1.position.x * reciprocalW1 + 1.0f) * 0.5f * ScreenWidth;
			v1.position.y = (1.0f - v1.position.y * reciprocalW1) * 0.5f * ScreenHeight;
			v2.position.x = (v2.position.x * reciprocalW2 + 1.0f) * 0.5f * ScreenWidth;
			v2.position.y = (1.0f - v2.position.y * reciprocalW2) * 0.5f * ScreenHeight;
		}

		DrawShadedTriangle(v0, v1, v2, RGB(255, 0, 0));

	}

	closegraph();			// 关闭绘图窗口
	return 0;
}