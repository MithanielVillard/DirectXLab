#include "pch.h"

#include "RenderContext.h"
#include "RenderWindow.h"

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	RenderContext::Init();
	RenderWindow window(L"Hello engine", 900, 500);

	while (window.IsOpen())
	{
		window.Update();

		window.BeginFrame();
		window.EndFrame();
	}
}
