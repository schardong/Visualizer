#include "eventmanager.h"
#include <cstring>

EventManager::EventManager()
{
    memset(m_aKeys, false, sizeof(bool) * 256);
    memset(m_aMouseButtons, false, sizeof(bool) * 3);
    memset(m_aMousePos, false, sizeof(unsigned int) * 2);
}

EventManager::~EventManager()
{
    memset(m_aKeys, false, sizeof(bool) * 256);
    memset(m_aMouseButtons, false, sizeof(bool) * 3);
    memset(m_aMousePos, false, sizeof(unsigned int) * 2);
}
