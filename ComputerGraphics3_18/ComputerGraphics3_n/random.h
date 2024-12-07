#pragma once
#include <random>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

std::random_device rd;
std::default_random_engine dre{ rd() };

GLfloat RandomColor();

GLfloat RandomColor()
{
	std::uniform_int_distribution uid{ 0, 1000000 };
	return uid(dre) / 1000000.f;
}

GLfloat RandomGLfloat()
{
	std::uniform_int_distribution uid{ 0, 2000000 };
	return (uid(dre) / 1000000.f) - 1.f;
}

GLfloat RandomLenght()
{
	std::uniform_int_distribution uid{ 0, 1000000 };
	return (uid(dre) / 4000000.f);
}

GLfloat RandomSize()
{
	std::uniform_int_distribution uid{ 0, 1000000 };
	return ((uid(dre) - 500000.f) / 5000000.f);
}

int RandomDic()
{
	std::uniform_int_distribution uid{ 0, 7 };
	return (uid(dre));
}

int RandomInt()
{
	std::uniform_int_distribution uid{ 0, std::numeric_limits<int>::max()};
	return (uid(dre));
}