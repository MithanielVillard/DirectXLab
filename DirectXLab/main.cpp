#include "pch.h"

#include "RenderContext.h"
#include "Window.h"

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	RenderContext::Init();
	Window window(L"Hello engine", 900, 500);

	while (window.IsOpen())
	{
		window.Update();
	}
}
