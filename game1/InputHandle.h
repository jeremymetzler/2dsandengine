#pragma once

namespace InputHandler
{
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
	void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
}


class InputBase
{
public:
	int kKey = 0;
	int kAction = 0;

	int kKeyBuff[GLFW_KEY_LAST];
	bool kKeyPressedBuff[GLFW_KEY_LAST];
	bool kKeyPressed[GLFW_KEY_LAST];

	bool kEnableControl = true;

	int mButton = 0;
	int mAction = 0;

	int mButtonBuff[GLFW_MOUSE_BUTTON_LAST];
	bool mButtonPressedBuff[GLFW_MOUSE_BUTTON_LAST];
	bool mButtonPressed[GLFW_MOUSE_BUTTON_LAST];

	Vec2Df pixelMousePosition = Vec2Df();
	Vec2Df particleMousePosition = Vec2Df();

	InputBase()
	{
		for (int i = 0; i < GLFW_KEY_LAST; i++)
		{
			kKeyPressed[i] = false;
			kKeyBuff[i] = 0;
			kKeyPressedBuff[i] = false;
		}

		for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; i++)
		{
			mButtonBuff[i] = 0;
			mButtonPressedBuff[i] = false;
			mButtonPressed[i] = false;
		}

		glfwSetKeyCallback(I::baseClass->window, InputHandler::KeyCallback);
		glfwSetCursorPosCallback(I::baseClass->window, InputHandler::CursorPositionCallback);
		glfwSetMouseButtonCallback(I::baseClass->window, InputHandler::MouseButtonCallback);
		glfwSetFramebufferSizeCallback(I::baseClass->window, InputHandler::FrameBufferSizeCallback);
	}

	inline void Reset()
	{
		for (int i = 0; i < GLFW_KEY_LAST; i++)
		{
			kKeyPressed[i] = false;
		}

		for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; i++)
		{
			mButtonPressed[i] = false;
		}

		kKey = 0;
	}

    bool IsMouseButtonHeld(int button);
    bool IsMouseButtonPressed(int button);

	bool IsKeyHeld(int key);

	bool IsKeyPressed(int key);

    void DisableInputs();
    void EnableInputs();

};