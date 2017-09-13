/*****************************************************************************
*   Open-source Game Engine (OGE) 0.1.0 programmed in C++
******************************************************************************
*   Copyright (C) 2016 - Fernando Alan Quinteros, 10th Dec 2015.
*   http://email:lordfers@gmail.com
******************************************************************************
*   This program is open-source and free software.
*   OGE used the standard libraries CEGUI 8.4.x GLFW and GLEW using OpenGL 2.x
*   https://github.com/LordFers/OGE/
*****************************************************************************/

#pragma once
#include <chrono>

#ifdef WIN32
	#include <Windows.h>
#else
	#include <sys/time.h>
#endif

namespace Util {
	static std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

	unsigned int getTick() {
		#ifdef WIN32
			return GetTickCount();
		#else
			struct timeval tv;
			gettimeofday(&tv, 0);
			return unsigned int((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
		#endif
	}

	float getElapsed() {
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<float> elapsed_seconds = (end - start) * 1000;
		start = std::chrono::system_clock::now();

		return elapsed_seconds.count();
	}
}