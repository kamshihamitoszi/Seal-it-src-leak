#pragma once
#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//https://guidedhacking.com/threads/opengl-hooking-drawing-text-rendering.14460/

struct vec3 { float x, y, z; };

namespace GL
{
	class Font
	{
	public:
		bool bBuilt = false;
		unsigned int base;
		HDC hdc = nullptr;
		int height;
		int width;

		void Build(int height);
		void Print(float x, float y, unsigned char color[3], const char* format, ...);

		vec3 centerText(float x, float y, float width, float height, float textWidth, float textHeight);
		float centerText(float x, float width, float textWidth);
	};
}