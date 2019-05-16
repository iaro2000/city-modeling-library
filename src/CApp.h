#ifndef CAPP_H
#define CAPP_H

#include "Ogre.h"
#include "OgreSingleton.h"
#include "OgreConfigFile.h"
using namespace Ogre;
#include "CPlayer.h"
#include "CWorld.h"
#include "CInputMgr.h"

enum State
{
	ZEROTH = 0,
	MAINMENU = 1,
	WORLD = 2
};

class CApp : public Ogre::Singleton<CApp>, public FrameListener, public OIS::KeyListener
{
public:
	CApp();
	~CApp();
	
	void initialize();
	void run();

private:
	void update(Real time);
	void setState(int state);
	bool frameStarted(const FrameEvent &evt);
	bool keyPressed(const OIS::KeyEvent &e);
	bool keyReleased(const OIS::KeyEvent &e){return true;};

	Root*		mRoot;
	Timer		mTimer;	
	bool		mShutdown;
	int			mCurrentState;
	Overlay*	mDebugOverlay;

	CWorld*		mWorld;
	CInputMgr*	mInputManager;

};

#endif