#include <Ogre.h>
using namespace Ogre;

#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

#include "CApp.h"

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char **argv)
#endif
{
    try
    {
		new CApp();
		CApp::getSingletonPtr()->run();
    }
    catch(Exception& e)
    {
#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBoxA(NULL, e.getFullDescription().c_str(), "Exception!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        fprintf(stderr, "Exception: %s\n",
            e.getFullDescription().c_str());
#endif
    }

    return 0;
}
