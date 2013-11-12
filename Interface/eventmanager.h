#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <GLFW/glfw3.h>

#define KEY_ESCAPE (GLFW_KEY_ESCAPE - 256)
#define KEY_SPACE GLFW_KEY_SPACE

#define KEY_F1 (GLFW_KEY_F1 - 256)
#define KEY_F2 (GLFW_KEY_F2 - 256)
#define KEY_F3 (GLFW_KEY_F3 - 256)
#define KEY_F4 (GLFW_KEY_F4 - 256)
#define KEY_F5 (GLFW_KEY_F5 - 256)
#define KEY_F6 (GLFW_KEY_F6 - 256)
#define KEY_F7 (GLFW_KEY_F7 - 256)
#define KEY_F8 (GLFW_KEY_F8 - 256)
#define KEY_F9 (GLFW_KEY_F9 - 256)
#define KEY_F10 (GLFW_KEY_F10 - 256)

#define KEY_KP_0 (GLFW_KEY_KP_0 - 256)
#define KEY_KP_1 (GLFW_KEY_KP_1 - 256)
#define KEY_KP_2 (GLFW_KEY_KP_2 - 256)
#define KEY_KP_3 (GLFW_KEY_KP_3 - 256)
#define KEY_KP_4 (GLFW_KEY_KP_4 - 256)
#define KEY_KP_5 (GLFW_KEY_KP_5 - 256)
#define KEY_KP_6 (GLFW_KEY_KP_6 - 256)
#define KEY_KP_7 (GLFW_KEY_KP_7 - 256)
#define KEY_KP_8 (GLFW_KEY_KP_8 - 256)
#define KEY_KP_9 (GLFW_KEY_KP_9 - 256)
#define KEY_KP_ADD (GLFW_KEY_KP_ADD - 256)
#define KEY_KP_SUBTRACT (GLFW_KEY_KP_SUBTRACT - 256)

class EventManager
{
private:
    bool m_aKeys[256];
    bool m_aMouseButtons[3];
    unsigned int m_aMousePos[2];
public:
    EventManager();
    ~EventManager();

    void keyTyped(int key)
    {
        m_aKeys[key] = true;
    }

    void keyReleased(int key)
    {
        m_aKeys[key] = false;
    }

    bool isKeyPressed(int key)
    {
        return m_aKeys[key];
    }

    unsigned int* getMousePos()
    {
        return m_aMousePos;
    }

    void setMousePos(unsigned int x, unsigned int y)
    {
        m_aMousePos[0] = x;
        m_aMousePos[1] = y;
    }

    void mouseClick(int button)
    {
        m_aMouseButtons[button] = true;
    }

    void mouseRelease(int button)
    {
        m_aMouseButtons[button] = false;
    }
};

#endif // INPUTMANAGER_H
