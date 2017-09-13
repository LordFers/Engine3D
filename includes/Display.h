#pragma once

#ifdef WIN32
	#include <Windows.h>
#endif

#include "..\ApplicationListener.h"
#include "..\DisplayMode.h"

#include <GLEW\glew.h>
#include <GLFW\glfw3.h>

class Display {
	public:
		Display(DisplayMode& pDisplayMode, ApplicationListener& pApplication);
		~Display() { };
	
	protected:
		ApplicationListener& mListener;
};