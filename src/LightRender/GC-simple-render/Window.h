#pragma once

#include <string_view>
#include <RenderWindow.h>
#include <define.h>

#include "Renderer.h"
#undef DrawText

class Text;

enum WINDOW_TYPE : int32
{
	FLAG_NO_MSAA = 0x00000001,
	FLAG_MSAA_X4 = 0x00000010,
	FLAG_MSAA_X8 = 0x00000100,
	FLAG_SHADOW_PASS_OFF = 0x00001000,
};

class Window
{
public:

	Window() = default;
	Window(std::wstring_view title, uint32 width, uint32 height, int32 type = 0);
	Window(Window const& other) = delete;
	Window(Window&& other) = delete;

	Window& operator=(Window const& other) = delete;
	Window& operator=(Window&& other) = delete;

	~Window();

	void Begin(Camera& camera);
	void Draw(Geometry& geo);
	void DrawText(Text& text);
	void End();

	void Display();

	bool IsOpen() const { return m_window.IsOpen(); }

private:
	void Create(std::wstring_view title, uint32 width, uint32 height, int32 type);
	void RenderShadowPass();
	void RenderMainPass();
	void Render2DPass();
	Renderer& GetRenderer() { return m_renderer; }

private:
	bool m_IsShadowMapEnabled;

	gce::RenderWindow m_window;
	Renderer m_renderer;

	uint32 m_width = 0;
	uint32 m_height = 0;

	std::vector<Geometry*> m_geoToDraw;
	std::vector<Text*> m_textToDraw;

	friend class Renderer;
	friend class ShadowMap;
};