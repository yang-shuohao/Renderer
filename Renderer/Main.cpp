// ����EasyXͼ�ο�ͷ�ļ�
#include <graphics.h>		
#include <conio.h>
//glm��ѧ���ͷ�ļ�
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


int main()
{
	initgraph(640, 640);	// ������ͼ���ڣ���СΪ 640x480 ����

	putpixel(100, 100, RED);


	_getch();				// �����������
	closegraph();			// �رջ�ͼ����
	return 0;
}