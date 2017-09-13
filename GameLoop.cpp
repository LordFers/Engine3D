#include "Game.h"

int mWidth, mHeight;
int main(int argc, char* argv[]) {
	/*std::ifstream file("resources/display.cfg", std::ios::in | std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		MessageBox(NULL, L"Don't exist the file 'display.cfg'.", (LPCWSTR)"Argentum Online", MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}

	int tempInt = 0;
	file.seekg(0, std::ios::beg);
	file.read((char *)&tempInt, 4);
	file.read((char *)&tempInt, 4);

	//data
	//data
	//data

	file.close();*/

	mWidth = 1920;
	mHeight = 1080;

	try {
		Display* display = new Display(DisplayMode(mWidth, mHeight, "Argentum Online", false), Game());
	}
	catch (int e) {
		printf("Error. Exit the program with exception nro %d \n", e);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

bool wireFrame = false;

void Game::onCreate() {
	/* Set state of GL */
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_ALPHA_TEST);

	/* Renderer Master */
	mRenderer = new Renderer(mWidth, mHeight);
	mDepth = mRenderer->getDepthWorld();
}

void Game::onRender(float delta) {
	/* Clear */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);

	if (wireFrame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	/* Renderer */
	mRenderer->render(delta);

	if (wireFrame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	/* Events */
	onEvents(delta);
}

void Game::onResize(const int width, const int height) {
	glViewport(0, 0, width, height);
}

void Game::onEvents(const float delta) {
	//getCamera member:
	Camera& camera = mRenderer->getCamera();

	if (mIsClicked == true && mButtonClick == 2) {
		// Mouse Picking
		glm::mat4 tmpView = camera.getViewMatrix();
		glm::mat4 tmpProj = camera.getProjection();
		glm::vec4 viewPort = glm::vec4(0.0f, 0.0f, mWidth, mHeight);

		float z = 0.0f;
		glReadPixels(mMouseX, mHeight - mMouseY - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

		glm::vec3 screenPos = glm::vec3(mMouseX, mHeight - mMouseY - 1, z);
		glm::vec3 worldPos = glm::unProject(screenPos, tmpView, tmpProj, viewPort);
		
		// Limit
		if (worldPos.x >= mDepth)
			return;

		if (worldPos.z >= mDepth)
			return;

		if (worldPos.x < 0.0f)
			return;

		if (worldPos.z < 0.0f)
			return;
		//
		//
		if (mButtonClick == 2) {
			mRenderer->getTerrain().setHeight((int)worldPos.x, (int)worldPos.z, 6);
		}
	}

	if (mKeyPressed[262]) {
		camera.RotateX(0.05f);
	}

	if (mKeyPressed[263]) {
		camera.RotateX(-0.05f);
	}

	if (mKeyPressed[87]) {
		camera.translate('w', delta);
		return;
	}

	if (mKeyPressed[65]) {
		camera.translate('a', delta);
		return;
	}

	if (mKeyPressed[83]) {
		camera.translate('s', delta);
		return;
	}

	if (mKeyPressed[68]) {
		camera.translate('d', delta);
		return;
	}
}

void Game::onKey_Down(const int keycode) {
	mKeyPressed[keycode] = true;
	static int level = 0;
	
	if (keycode == GLFW_KEY_R) {
		if (wireFrame) {
			wireFrame = false;
		}
		else {
			wireFrame = true;
		}
	}
	if (keycode == 257) {
		level++;
		if (level > 4) {
			level = 0;
		}
		//mRenderer->getTerrain().setLOD(level);
		mRenderer->getTerrain().setLOD(level);
	}
}

void Game::onKey_Up(const int keycode) {
	mKeyPressed[keycode] = false;
}

void Game::onMouseMove(const double x, const double y) {
	mMouseX = (int)x; mMouseY = (int)y;
	mRenderer->getCamera().Rotate(x, y, mWidth, mHeight);
}

void Game::onMouseButton_Down(const int button) {
	mIsClicked = true;
	mButtonClick = button;
	mClickX = mMouseX; mClickY = mMouseY;

	if (button == 0) {
		ShowCursor(NULL);
		mRenderer->getCamera().setRotating(true);
	}
}

void Game::onMouseButton_Up(const int button) {
	mIsClicked = false;

	if (button == 0) {
		mRenderer->getCamera().setRotating(false);
		ShowCursor(true);
	}
}

void Game::onDispose() {
	delete mRenderer;
}