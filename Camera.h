#pragma once
#ifdef WIN32
	#include <Windows.h>
#endif
#include "Maths.h"

enum PROJECTION_CAMERA {
	PERSPECTIVE = 1, ORTHOGONAL
};

class Camera {
	public:
		Camera() : mYaw(0.0f), mPitch(0.0f), mRoll(0.0f) {
			mPosition = glm::vec3(1.0f, 1.0f, 1.0f);
			Update();
		}
		~Camera() { }

	public:
		inline const glm::vec3& getPosition() {
			return mPosition;
		}

		inline const glm::mat4& getViewMatrix() const {
			return mView;
		}

		inline const glm::mat4& getViewMatrixInverse() const {
			return mViewInverse;
		}

		inline const glm::mat4& getProjection() const {
			return mProjection;
		}

		inline void setRotating(const bool rotating) {
			mRotating = rotating;
		}

		inline void setHeight(const float height) {
			mHeight = height;
		}

		inline void setProjection(const glm::mat4& projection) {
			mProjection = projection;
		}

	public:
		void translate(const unsigned char key, const float delta) {
			float dx = 0; float dz = 0;
			switch (key) {
				case 'w':
					dz = 2.0f * delta / 100.0f;
				break;

				case 's':
					dz = -2.0f * delta / 100.0f;
				break;

				case 'a':
					dx = -2.0f * delta / 100.0f;
				break;

				case 'd':
					dx = 2.0f * delta / 100.0f;
				break;
			}

			/* Get current view-Matrix */
			glm::mat4 mat = mView;

			/* Row major */
			glm::vec3 forward(mat[0][2], mat[1][2], mat[2][2]);
			glm::vec3 strafe(mat[0][0], mat[1][0], mat[2][0]);

			/* Const-speed */
			const float speed = 1.0f;
			mPosition += (-dz * forward + dx * strafe) * speed;

			/* Update Camera */
			Update();
		}

		inline void RotateX(float x) {
			mYaw += x;
			Update();
		}

		inline void RotateY(float y) {
			mPitch += y;
			Update();
		}

		void Rotate(double x, double y, int width, int height) {
			/* isRotating? */
			if (mRotating == false)
				return;

			glm::vec2 delta = (glm::vec2((float)x, (float)y) - glm::vec2((float)width / 2.0f, (float)height / 2.0f));

			const float x_sensivity = 0.0020f;
			const float y_sensivity = 0.0020f;

			SetCursorPos(width / 2, height / 2);

			mYaw = x_sensivity * delta.x;
			mPitch = y_sensivity * delta.y;

			Update();
		}

		void Update() {
			glm::quat key_quat = glm::quat(glm::vec3(mPitch, mYaw, mRoll));
			mPitch = mYaw = mRoll = 0.0f;

			mCameraQuat = key_quat * mCameraQuat;
			mCameraQuat = glm::normalize(mCameraQuat);

			glm::mat4 rotate = glm::mat4_cast(mCameraQuat);
			
			mPosition.y = mHeight;
			glm::mat4 translate = glm::translate(glm::mat4(1.0f), -mPosition);
			
			mView = rotate * translate;
			//
			mViewInverse = mView;
			mViewInverse = glm::inverse(mViewInverse);
		}

	private:
		bool mRotating;

	private:
		float mPitch;
		float mYaw;
		float mRoll;

	private:
		glm::vec3 mTarget;
		glm::vec3 mRight;
		glm::vec3 mPosition;

	private:
		glm::mat4 mView;
		glm::mat4 mViewInverse;
		glm::mat4 mProjection;

	private:
		glm::quat mCameraQuat;

	private:
		//FrustumCulling mFrustum;
		glm::vec3 eyeVec;
		glm::vec3 destVec;
		glm::vec3 upVec;

	private:
		float mHeight;
};