#include "Camera.h"
#include "Window.h"

// 行列の更新
void Camera::UpdateMatrix() {
	Matrix4 cameraWorld = CreateAffine4(1.0f, mRotate, mTranslate);
	mViewMat = Inverse(cameraWorld);
	Window& window = Window::GetInstance();
	float windowWidth = static_cast<float>(window.GetClientWidth());
	float windowHeight = static_cast<float>(window.GetClientHeight());
	switch (mProjectionMode) {
	case ProjectionMode::kOrthographic:
		mProjectionMat = CreateOrthographic(0.0f, 0.0f, windowWidth, windowHeight, mNearZ, mFarZ);
		break;
	case ProjectionMode::kPerspective:
		mProjectionMat = CreatePerspectiveFov(mFovY, windowWidth / windowHeight, mNearZ, mFarZ);
		break;
	}
	mViewProjectionMat = mViewMat * mProjectionMat;
}
