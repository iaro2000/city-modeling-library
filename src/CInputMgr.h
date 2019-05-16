#ifndef CINPUTMANAGER_H
#define CINPUTMANAGER_H

#include "Ogre.h"
#include "OgreRenderWindow.h"
using namespace Ogre;
#include "OIS/OISMouse.h"
#include "OIS/OISKeyboard.h"
#include "OIS/OISInputManager.h"



class CInputMgr : public Ogre::Singleton<CInputMgr>, public OIS::KeyListener, public OIS::MouseListener
{
public:
	CInputMgr();
	~CInputMgr();

	void initialize(RenderWindow* renderWnd);
	void capture();
	void setWindowExtents(int width, int height);

	void addKeyListener( OIS::KeyListener *keyListener, const std::string& instanceName);
	void addMouseListener( OIS::MouseListener *mouseListener, const std::string& instanceName);
	void removeKeyListener( const std::string& instanceName);
	void removeMouseListener( const std::string& instanceName);

	OIS::Mouse*		getMouse() {return mMouse;};
	OIS::Keyboard*	getKeyboard() {return mKeyboard;};
	OIS::InputManager *getManager(){return mInputSystem;};
	
private:

	bool keyPressed( const OIS::KeyEvent &e );
	bool keyReleased( const OIS::KeyEvent &e);
	bool mouseMoved( const OIS::MouseEvent &e);
	bool mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id);
	bool mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id);

	OIS::Mouse*			mMouse;
	OIS::Keyboard*		mKeyboard;
	OIS::InputManager*	mInputSystem;

	std::map<std::string, OIS::KeyListener*> mKeyListeners;
	std::map<std::string, OIS::MouseListener*> mMouseListeners;

	std::map<std::string, OIS::KeyListener*>::iterator itKeyListener;
	std::map<std::string, OIS::KeyListener*>::iterator itKeyListenerEnd;
	std::map<std::string, OIS::MouseListener*>::iterator itMouseListener;
	std::map<std::string, OIS::MouseListener*>::iterator itMouseListenerEnd;
};

#endif