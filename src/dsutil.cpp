// The file from 0x43a7b0: the DirectX 8 SDK's dsutil.cpp, as the game
// builds it.

#include "dsutil.h"

// From the SDK's dxutil.h.
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

CSoundManager::CSoundManager()
{
    m_pDS = NULL;
}

CSoundManager::~CSoundManager()
{
    SAFE_RELEASE(m_pDS);
}
