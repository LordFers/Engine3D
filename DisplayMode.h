#pragma once
#include <string>

class DisplayMode {
	public:
		DisplayMode(const int width, const int height, const std::string& title, const bool fs) : mWidth(width),
			mHeight(height),
			mTitle(title),
			mFS(fs) {}

	public:
		inline int getWidth() const { return mWidth; }
		inline int getHeight() const { return mHeight; }
		inline const std::string& getTitle() { return mTitle; }
		inline bool isFullScreen() const { return mFS; }

	private:
		int mWidth, mHeight;
		std::string mTitle;
		bool mFS;
};