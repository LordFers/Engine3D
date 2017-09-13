#pragma once

class ApplicationListener {
	public:
		virtual ~ApplicationListener() {};

	public:
		virtual void onCreate() = 0;
		virtual void onRender(float delta) = 0;
		virtual void onResize(const int width, const int height) = 0;
		virtual void onKey_Down(const int keycode) = 0;
		virtual void onKey_Up(const int keycode) = 0;
		virtual void onMouseMove(const double x, const double y) = 0;
		virtual void onMouseButton_Down(const int button) = 0;
		virtual void onMouseButton_Up(const int button) = 0;
		virtual void onDispose() = 0;
};