// 引用EasyX图形库头文件
#include <graphics.h>		
#include <conio.h>
//glm数学相关头文件
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


int main()
{
	initgraph(640, 640);	// 创建绘图窗口，大小为 640x480 像素

	putpixel(100, 100, RED);


	_getch();				// 按任意键继续
	closegraph();			// 关闭绘图窗口
	return 0;
}