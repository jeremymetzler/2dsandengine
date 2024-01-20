#include "header_stack.h"

namespace InputHandler
{
    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        I::inputs->kKey = key;
        I::inputs->kAction = action;
        if (key > 0 && key < GLFW_KEY_LAST)
        {
            if (I::inputs->kKeyPressedBuff[key] == false)
            {
                if (action == GLFW_PRESS)
                {
                    I::inputs->kKeyPressedBuff[key] = true;
                    I::inputs->kKeyPressed[key] = true;
                }
            }

            if (action == GLFW_RELEASE)
            {
                I::inputs->kKeyPressedBuff[key] = false;
            }

            I::inputs->kKeyBuff[key] = action;
        }

    }

    void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
    {
        I::inputs->pixelMousePosition.x = xpos;
        I::inputs->pixelMousePosition.y = I::baseClass->screenResolution[1] - ypos;

        I::inputs->particleMousePosition = I::inputs->pixelMousePosition / I::baseClass->particleSize;
        I::inputs->particleMousePosition += I::baseClass->cameraPos - Vec2Dd(I::baseClass->halfXParticleRes, I::baseClass->halfYParticleRes);
    }

    void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        I::inputs->mButton = button;
        I::inputs->mAction = action;

        if (button < GLFW_MOUSE_BUTTON_LAST)
        {
            if (I::inputs->mButtonPressedBuff[button] == false)
            {
                if (action == GLFW_PRESS)
                {
                    I::inputs->mButtonPressedBuff[button] = true;
                    I::inputs->mButtonPressed[button] = true;
                }
            }

            if (action == GLFW_RELEASE)
            {
                I::inputs->mButtonPressedBuff[button] = false;
            }

            I::inputs->mButtonBuff[button] = action;
        }
    }

    void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        //   newResX = width;
         //  newResY = height;
    }

}

bool InputBase::IsMouseButtonHeld(int button)
{
    return (kEnableControl && mButtonBuff[button] & (GLFW_REPEAT | GLFW_PRESS));
}

bool InputBase::IsMouseButtonPressed(int button)
{
    return (kEnableControl && mButtonPressed[button]);
}

bool InputBase::IsKeyHeld(int key)
{
    return (kEnableControl && kKeyBuff[key] & (GLFW_REPEAT | GLFW_PRESS));
}

bool InputBase::IsKeyPressed(int key)
{
    return (kEnableControl && kKeyPressed[key]);
}

void InputBase::DisableInputs()
{
    kEnableControl = false;
}

void InputBase::EnableInputs()
{
    kEnableControl = true;
}
