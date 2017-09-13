#include "includes\Display.h"
#include "Renderer.h"
#include <memory>

class Game: public ApplicationListener {
	public:
		virtual void onCreate() override;
		virtual void onRender(float delta) override;
		virtual void onResize(const int width, const int height) override;
		virtual void onKey_Down(const int keycode) override;
		virtual void onKey_Up(const int keycode) override;
		virtual void onMouseMove(const double x, const double y) override;
		virtual void onMouseButton_Down(const int button) override;
		virtual void onMouseButton_Up(const int button) override;
		virtual void onDispose() override;

	private:
		Renderer* mRenderer;

	private:
		void onEvents(const float delta);

	private:
		int mKeyPressed[350];
		int mMouseX, mMouseY;
		int mClickX, mClickY, mButtonClick;
		bool mIsClicked;
		int mDepth;
};