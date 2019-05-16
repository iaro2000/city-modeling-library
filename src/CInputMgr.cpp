#include "CInputMgr.h"

template<> CInputMgr* Ogre::Singleton<CInputMgr>::ms_Singleton = 0;

CInputMgr::CInputMgr()
{
	mMouse = 0;
	mKeyboard = 0;
	mInputSystem = 0;
}

CInputMgr::~CInputMgr()
{
	if (mInputSystem)
	{
		if (mMouse)
		{
			mInputSystem->destroyInputObject(mMouse);
			mMouse = 0;
		}

		if (mKeyboard)
		{
			mInputSystem->destroyInputObject(mKeyboard);
			mKeyboard = 0;
		}
	}

	mInputSystem->destroyInputSystem(mInputSystem);
	mInputSystem = 0;

	mKeyListeners.clear();
	mMouseListeners.clear();
}

void CInputMgr::initialize(RenderWindow *rwnd)
{
	if (!mInputSystem)
	{
		//create input system
		OIS::ParamList paramList;
		size_t windowHandle = 0;
		std::ostringstream windowHandleStr;
		rwnd->getCustomAttribute("WINDOW",&windowHandle);
		windowHandleStr<< (unsigned int) windowHandle;
		paramList.insert(std::make_pair(std::string("WINDOW"), windowHandleStr.str()));
		mInputSystem = OIS::InputManager::createInputSystem(paramList);

		//create buffered keyboard
		if (mInputSystem->numKeyboards() > 0)
		{
			mKeyboard = static_cast<OIS::Keyboard*>(mInputSystem->createInputObject(OIS::OISKeyboard,true));
			mKeyboard->setEventCallback(this);
		}

		//create buffered mouse
		if (mInputSystem->numMice() > 0)
		{
			mMouse = static_cast<OIS::Mouse*>(mInputSystem->createInputObject(OIS::OISMouse,true));
			mMouse->setEventCallback(this);

			//window size
			unsigned int width, height, depth;
			int left, top;
			rwnd->getMetrics(width,height,depth,left,top);
			this->setWindowExtents(width, height);
		}
	}
}

void CInputMgr::setWindowExtents(int width, int height)
{
	//mouse region
	const OIS::MouseState &mouseState = mMouse->getMouseState();
	mouseState.width  = width;
	mouseState.height = height;
}

void CInputMgr::capture()
{
	if (mMouse) mMouse->capture();
	if (mKeyboard) mKeyboard->capture();
}

void CInputMgr::addKeyListener( OIS::KeyListener *keyListener, const std::string& instanceName)
{
	if (mKeyboard)
	{
		//listener with same name does not exist
		if (mKeyListeners.find(instanceName) == mKeyListeners.end())
			mKeyListeners[instanceName] = keyListener;
	}
}

void CInputMgr::addMouseListener( OIS::MouseListener *mouseListener, const std::string& instanceName)
{
	if (mMouse)
	{
		//listener with same name does not exist
		if (mMouseListeners.find(instanceName) == mMouseListeners.end())
			mMouseListeners[instanceName] = mouseListener;
	}
}

void CInputMgr::removeKeyListener( const std::string& instanceName)
{
	itKeyListener = mKeyListeners.find(instanceName);
	if (itKeyListener != mKeyListeners.end())
	{
		mKeyListeners.erase(itKeyListener);
	}
}

void CInputMgr::removeMouseListener( const std::string& instanceName)
{
	itMouseListener = mMouseListeners.find(instanceName);
	if (itMouseListener != mMouseListeners.end())
	{
		mMouseListeners.erase(itMouseListener);
	}
}

bool CInputMgr::keyPressed( const OIS::KeyEvent &e )
{
	itKeyListener = mKeyListeners.begin();
	itKeyListenerEnd = mKeyListeners.end();

	for (; itKeyListener != itKeyListenerEnd; ++itKeyListener)
	{
		if (!itKeyListener->second->keyPressed(e))
			break;
	}
	return true;
}

bool CInputMgr::keyReleased( const OIS::KeyEvent &e )
{
	itKeyListener = mKeyListeners.begin();
	itKeyListenerEnd = mKeyListeners.end();

	for (; itKeyListener != itKeyListenerEnd; ++itKeyListener)
	{
		if (!itKeyListener->second->keyReleased(e))
			break;
	}
	return true;
}

bool CInputMgr::mouseMoved( const OIS::MouseEvent &e)
{
	itMouseListener = mMouseListeners.begin();
	itMouseListenerEnd = mMouseListeners.end();

	for (; itMouseListener != itMouseListenerEnd; ++itMouseListener)
	{
		if (!itMouseListener->second->mouseMoved(e))
			break;
	}
	return true;
}

bool CInputMgr::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	itMouseListener = mMouseListeners.begin();
	itMouseListenerEnd = mMouseListeners.end();

	for (; itMouseListener != itMouseListenerEnd; ++itMouseListener)
	{
		if (!itMouseListener->second->mousePressed(e, id))
			break;
	}
	return true;
}

bool CInputMgr::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	itMouseListener = mMouseListeners.begin();
	itMouseListenerEnd = mMouseListeners.end();

	for (; itMouseListener != itMouseListenerEnd; ++itMouseListener)
	{
		if (!itMouseListener->second->mouseReleased(e, id))
			break;
	}
	return true;
}
