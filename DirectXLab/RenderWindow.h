#pragma once
#include "Window.h"

class RenderWindow : public Window
{
public:
	RenderWindow(std::wstring_view const title, int const width, int const height) : Window(title, width, height) {}
	~RenderWindow() override = default;

	void BeginFrame();
	void EndFrame();
};

