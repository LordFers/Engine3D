#include "..\includes\Display.h"
#include "..\Util.h"

#include <fstream>

Display::Display(DisplayMode& pDisplayMode, ApplicationListener& pApplication)
	: mListener(pApplication)
{
	if (!glfwInit())
		printf("Error creating Display with glfw3.dll, verify your setting. \n");

	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	glfwWindowHint(GLFW_REFRESH_RATE, GL_DONT_CARE);
	glfwSwapInterval(1);

	GLFWwindow* pWindow = glfwCreateWindow(pDisplayMode.getWidth(), pDisplayMode.getHeight(),
										   pDisplayMode.getTitle().c_str(), (pDisplayMode.isFullScreen() ? monitor : nullptr), NULL);

	glfwSetWindowAspectRatio(pWindow, 16, 9);
	glfwSetWindowUserPointer(pWindow, this);
	glfwMakeContextCurrent(pWindow);

	if (glewInit())
		printf("Error creating context and initializing OpenGL with glew32.dll, verify your settings. \n");

	//Check Version
	int major, minor, rev;
	major = glfwGetWindowAttrib(pWindow, GLFW_CONTEXT_VERSION_MAJOR);
	minor = glfwGetWindowAttrib(pWindow, GLFW_CONTEXT_VERSION_MINOR);
	rev = glfwGetWindowAttrib(pWindow, GLFW_CONTEXT_REVISION);
	printf("OpenGL version recieved: %d.%d.%d\n", major, minor, rev);
	printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
	printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

	auto swindow_callback = [](GLFWwindow * window, int w, int h) {
		Display* display = static_cast<Display *>(glfwGetWindowUserPointer(window));
		display->mListener.onResize(w, h);
	};
	glfwSetWindowSizeCallback(pWindow, swindow_callback);

	auto key_callback = [](GLFWwindow * window, int key, int scancode, int action, int mods) {
		Display* display = static_cast<Display *>(glfwGetWindowUserPointer(window));

		if (action == GLFW_PRESS) {
			display->mListener.onKey_Down(key);
		}
		else if (key && !action) {
			display->mListener.onKey_Up(key);
		}
	};
	glfwSetKeyCallback(pWindow, key_callback);

	auto mousepos_callback = [](GLFWwindow *window, double x, double y) {
		Display* display = static_cast<Display *>(glfwGetWindowUserPointer(window));
		display->mListener.onMouseMove(x, y);
	};
	glfwSetCursorPosCallback(pWindow, mousepos_callback);

	auto mousebutton_callback = [](GLFWwindow *window, int button, int action, int mods) {
		Display* display = static_cast<Display *>(glfwGetWindowUserPointer(window));

		if (action == GLFW_PRESS) {
			display->mListener.onMouseButton_Down(button);
		}
		else if (!action) {
			display->mListener.onMouseButton_Up(button);
		}
	};
	glfwSetMouseButtonCallback(pWindow, mousebutton_callback);

	//Game-context start:
	pApplication.onCreate();
	pApplication.onResize(pDisplayMode.getWidth(), pDisplayMode.getHeight());

	float delta = 0.0f;
	unsigned int frameTimer = 0;
	int fps = 60, fpsCounter = 0;

	while (!glfwWindowShouldClose(pWindow)) {
		pApplication.onRender(delta);
		//
		glfwSwapBuffers(pWindow);
		glfwPollEvents();

		if (Util::getTick() >= frameTimer + 1000) {
			frameTimer = Util::getTick();
			fps = fpsCounter;
			fpsCounter = 0;
		}

		delta = Util::getElapsed();
		++fpsCounter;
	}

	pApplication.onDispose();
	glfwTerminate();
}