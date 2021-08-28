// 引用EasyX图形库头文件
#include <graphics.h>		
#include <conio.h>
//glm数学相关头文件
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//加载图像
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <vector>

#define ScreenWidth 800
#define ScreenHeight 600

int ImgaeWidth;
int ImgaeHeight;
int nrChannels;
unsigned char* data;

//深度缓存，保存z
std::vector<std::vector<float>> DepthBuffer;

// struct Light
// {
// 	COLORREF lightColor;
// 	float lightStrength;
// };
// 
// struct PointLight : Light
// {
// 	glm::vec3 lightPosition;
// };
// 
// struct DirectionLight : Light
// {
// 	glm::vec3 lightDirection;
// };
// 
// struct SpotLight : PointLight
// {
// 	glm::vec3 lightDirection;
// };

struct Vertex
{
	glm::vec4 position;
	float color;
	glm::vec2 uv;
};

struct Triangle
{
	glm::vec3 index;
	COLORREF color;
};

struct Model
{
	std::string name;
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;
};

struct  Transform
{
	glm::vec3 translate;
	glm::vec4 rotation;
	glm::vec3 scale3D;
};

struct Instance
{
	Model model;
	Transform transform;
};

struct Scene
{
	std::vector<Instance> instances;
};

float GetTexel(unsigned char* texture, float tx, float ty)
{
	float fx = glm::fract(tx);
	float fy = glm::fract(ty);
	tx = glm::floor(tx);
	ty = glm::floor(ty);

	int itx = (int)tx;
	int ity = (int)ty;

	float TL = texture[itx * ImgaeWidth + ity];
	float TR = texture[(itx + 1) * ImgaeWidth + ity];
	float BL = texture[itx * ImgaeWidth + ity + 1];
	float BR = texture[(itx + 1) * ImgaeWidth + ity + 1];

	float CT = fx * TR + (1 - fx) * TL;
	float CB = fx * BR + (1 - fx) * BL;

	return fy * CB + (1 - fy) * CT;

}

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
			//Sleep(1);
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
void DrawFilledTriangle(Vertex P0, Vertex P1, Vertex P2, COLORREF color)
{
	//排序顶点 P0.position.y <= P1.position.y <= P2.position.y
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
	//P0P1边z坐标数组
	std::vector<float> z01 = Interpolate(P0.position.y, P0.position.z, P1.position.y, P1.position.z);

	//P1P2边x坐标数组
	std::vector<float> x12 = Interpolate(P1.position.y, P1.position.x, P2.position.y, P2.position.x);
	//P1P2边z坐标数组
	std::vector<float> z12 = Interpolate(P1.position.y, P1.position.z, P2.position.y, P2.position.z);

	//P0P2边x坐标数组
	std::vector<float> x02 = Interpolate(P0.position.y, P0.position.x, P2.position.y, P2.position.x);
	//P0P2边z坐标数组
	std::vector<float> z02 = Interpolate(P0.position.y, P0.position.z, P2.position.y, P2.position.z);


	//【注意】去掉重复坐标，P0P1和P1P2重复了P1
	//x01.pop_back();
	//x012=x01+x12 x012代表P0P1和P1P2两条边的x坐标数组
	x01.insert(x01.end(), x12.begin(), x12.end());
	std::vector<float> x012(x01);

	float mx = glm::floor(x012.size() / 2);
	std::vector<float> x_left;
	std::vector<float> x_right;

	//-------第一种情况
	//---------P2|\
	//-----------| \
	//-----------|  \ P1
	//-----------|  /
	//-----------| /
	//---------P0|/		
	if (x02[mx] < x012[mx])
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

	z01.insert(z01.end(), z12.begin(), z12.end());
	std::vector<float> z012(z01);

	float mz = glm::floor(z012.size() / 2);
	std::vector<float> z_left;
	std::vector<float> z_right;

	if (z02[mz] < z012[mz])
	{

		z_left = z02;
		z_right = z012;
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
		z_left = z012;
		z_right = z02;
	}

	//从左到右填充
	for (int y = P0.position.y;y < P2.position.y;y++)
	{
		float z = 1/z_left[y - P0.position.y];
		for (int x = x_left[y - P0.position.y];x < x_right[y - P0.position.y];x++)
		{
			if (z > DepthBuffer[x][y])
			{
				putpixel(x, y, color);
				DepthBuffer[x][y] = z;
			}
		}
		//Sleep(1);
	}
}

glm::vec4 ApplyTransform(glm::vec4 pos, Transform transform)
{
	glm::mat4 sm = glm::scale(glm::mat4(1.0f), transform.scale3D);
	glm::mat4 rm = glm::rotate(glm::mat4(1.0f), glm::radians(transform.rotation.z), glm::vec3(transform.rotation));
	glm::mat4 tm = glm::translate(glm::mat4(1.0f), transform.translate);
	//构造模型矩阵
	glm::mat4 model = tm * rm * sm;
	return model * pos;
}

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


//渲染一个三角形
void RenderTriangle(Triangle triangle, std::vector<Vertex> vertices)
{
	DrawFilledTriangle(vertices[triangle.index.x],
		vertices[triangle.index.y],
		vertices[triangle.index.z],
		triangle.color);
}

//渲染一个物体
void RenderObject(std::vector<Vertex> vertices, std::vector<Triangle> triangles)
{
	for (int i = 0; i < triangles.size(); i++)
	{
		RenderTriangle(triangles[i], vertices);
	}
}

//渲染一个实例
void RenderInstance(Instance instances)
{
	Model model = instances.model;
	for (int i = 0;i < model.vertices.size();i++)
	{
		model.vertices[i].position = ApplyTransform(model.vertices[i].position, instances.transform);
	}

	for (int j = 0;j < model.triangles.size();j++)
	{
		RenderTriangle(model.triangles[j], model.vertices);
	}
}

//渲染一个场景
void RenderScene(Scene s)
{
	for (int i = 0; i < s.instances.size(); i++)
	{
		RenderInstance(s.instances[i]);
	}
}


glm::mat4 GetModel(glm::vec3 s = { 1.0f, 1.0f, 1.0f }, glm::vec4 r = { 0,1,0,45.0f }, glm::vec3 t = { 0, 0, 5 })
{
	glm::mat4 sm = glm::scale(glm::mat4(1.0f), s);
	glm::mat4 rm = glm::rotate(glm::mat4(1.0f), glm::radians(r.w), glm::vec3(r));
	glm::mat4 tm = glm::translate(glm::mat4(1.0f), t);
	//构造模型矩阵
	return tm * rm * sm;
}

glm::mat4 GetView(glm::vec3 eye=glm::vec3(0,0,0),glm::vec3 center=glm::vec3(0,0,1),glm::vec3 up=glm::vec3(0,1,0))
{
	return glm::lookAt(eye, center, up);
}

glm::mat4 GetPerspective(float fov=glm::radians(90.0f),float aspect= ScreenWidth/ScreenHeight,float n=0.1f,float f=100.0f)
{
	return glm::perspective(fov, aspect, n, f);
}

glm::mat4 GetMVP(glm::mat4 model, glm::mat4 view, glm::mat4 perspective)
{
	return perspective * view * model;
}


int main()
{

	initgraph(ScreenWidth, ScreenHeight);	// 创建绘图窗口，大小为 640x480 像素

	DepthBuffer.resize(ScreenWidth);
	for (int i=0;i<ScreenWidth;i++)
	{
		DepthBuffer[i].resize(ScreenHeight);
	}

	for (int i=0;i<ScreenWidth;i++)
	{
		for (int j = 0;j < ScreenHeight;j++)
		{
			DepthBuffer[i][j] = 0.0f;
		}
	}

	data = stbi_load("E:/Image/container.jpg", &ImgaeWidth, &ImgaeWidth, &nrChannels, 0);

	std::vector<Vertex> Vertices;
	Vertices.resize(8);
	Vertices[0].position = { 1,1,1 ,1 };
	Vertices[1].position = { -1,1,1,1 };
	Vertices[2].position = { -1,-1,1 ,1 };
	Vertices[3].position = { 1,-1,1 ,1 };
	Vertices[4].position = { 1,1,-1 ,1 };
	Vertices[5].position = { -1,1,-1 ,1 };
	Vertices[6].position = { -1,-1,-1 ,1 };
	Vertices[7].position = { 1,-1,-1 ,1 };

	Vertices[0].color = 1.0f;
	Vertices[1].color = 1.0f;
	Vertices[2].color = 0.5f;
	Vertices[3].color = 0.5f;
	Vertices[4].color = 0.5f;
	Vertices[5].color = 0.5f;
	Vertices[6].color = 1.0f;
	Vertices[7].color = 1.0f;

	Vertices[0].uv = {0,0};
	Vertices[1].uv = {1,0};
	Vertices[2].uv = {0,1};
	Vertices[3].uv = {1,1};
	Vertices[4].uv = {0,0};
	Vertices[5].uv = {1,0};
	Vertices[6].uv = {0,1};
	Vertices[7].uv = {1,1};

	std::vector<Triangle> triangles;
	triangles.resize(12);
	triangles[0].index = { 0,1,2 };
	triangles[0].color = RED;
	triangles[1].index = { 0,2,3 };
	triangles[1].color = RED;
	triangles[2].index = { 4,0,3 };
	triangles[2].color = GREEN;
	triangles[3].index = { 4,3,7 };
	triangles[3].color = GREEN;
	triangles[4].index = { 5,4,7 };
	triangles[4].color = BLUE;
	triangles[5].index = { 5,7,6 };
	triangles[5].color = BLUE;
	triangles[6].index = { 1,5,6 };
	triangles[6].color = YELLOW;
	triangles[7].index = { 1,6,2 };
	triangles[7].color = YELLOW;
	triangles[8].index = { 4,5,1 };
	triangles[8].color = WHITE;
	triangles[9].index = { 4,1,0 };
	triangles[9].color = WHITE;
	triangles[10].index = { 2,6,7 };
	triangles[10].color = CYAN;
	triangles[11].index = { 2,7,3 };
	triangles[11].color = CYAN;

	{
		//构造模型矩阵
		glm::mat4 model =GetModel();
		//构造视图矩阵
		glm::mat4 view = GetView();
		//构造透视投影矩阵
		glm::mat4 perspective =GetPerspective();
		//构造MVP矩阵
		glm::mat4 mvp = GetMVP(model,view,perspective);

		for (int i = 0; i < Vertices.size(); i++)
		{
			//对顶点进行MVP矩阵变换
			Vertices[i].position = mvp * Vertices[i].position;

			//透视除法
			float reciprocalW = 1 / Vertices[i].position.w;

			//屏幕映射
			Vertices[i].position.x = (Vertices[i].position.x * reciprocalW + 1.0f) * 0.5f * ScreenWidth;
			Vertices[i].position.y = (1.0f - Vertices[i].position.y * reciprocalW) * 0.5f * ScreenHeight;

		}

	}

	Scene s;
	Instance instance1;
	instance1.model.name = "1";
	instance1.model.vertices = Vertices;
	instance1.model.triangles = triangles;
	instance1.transform.translate = { 30,30,1 };
	instance1.transform.rotation = { 0,1,0,60.0f };
	instance1.transform.scale3D = { 1,1,1 };

	Instance instance2;
	instance2.model.name = "2";
	instance2.model.vertices = Vertices;
	instance2.model.triangles = triangles;
	instance2.transform.translate = { -30,30,1 };
	instance2.transform.rotation = { 0,1,0,60.0f };
	instance2.transform.scale3D = { 1,1,1 };


	s.instances.push_back(instance1);
	s.instances.push_back(instance2);


	RenderScene(s);

	_getch();
	closegraph();			// 关闭绘图窗口
	return 0;
}