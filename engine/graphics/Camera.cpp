#include "Camera.h"
#include "Window.h"

void Camera::UpdateMatrix() {
	Matrix4 cameraWorld = CreateAffine4(1.0f, mRotate, mTranslate);
	mViewMat = Inverse(cameraWorld);
	Window& window = Window::GetInstance();
	float winW = static_cast<float>(window.GetClientWidth());
	float winH = static_cast<float>(window.GetClientHeight());
	switch (mProjectionMode) {
	case ProjectionMode::kOrthographic:
		mProjectionMat = CreateOrthographic(0.0f, 0.0f, winW, winH, mNearZ, mFarZ);
		break;
	case ProjectionMode::kPerspective:
		mProjectionMat = CreatePerspectiveFov(mFovY, winW / winH, mNearZ, mFarZ);
		break;
	}
	mViewProjectionMat = mViewMat * mProjectionMat;
}
