#include "CApp.h"

template<> CApp* Ogre::Singleton<CApp>::ms_Singleton = 0;

CApp::CApp()
{
	initialize();
}

CApp::~CApp()
{
	delete mInputManager;		
	if (mRoot) OGRE_DELETE mRoot;
}

void CApp::initialize()
{
	//initialize Ogre root
	mRoot = new Root();

	//initialize base resource locations
	String secName,typeName,archName;
	ConfigFile conf;
	conf.load("base_res.cfg");
	ConfigFile::SectionIterator seci = conf.getSectionIterator();
	while (seci.hasMoreElements())
	{
		//return next key element without advancing
		secName = seci.peekNextKey();
		//return next value element and advance
		ConfigFile::SettingsMultiMap *settings = seci.getNext();
		ConfigFile::SettingsMultiMap::iterator iter;
		//iterate over section contents
		for (iter = settings->begin(); iter != settings->end(); ++iter)
		{
			typeName = iter->first;
			archName = iter->second;
			//add resource location
			ResourceGroupManager::getSingleton().addResourceLocation(archName,typeName,secName);
		}
	}
	
	//show config dialog

	if (!mRoot->showConfigDialog())
           throw Exception(52,
						   "No configuration selected!", 
						   "CApp::initialize()");

	mRoot->restoreConfig();

	//initialize the renderer
	mRoot->initialise(true,"City");

	//initialize the resource groups
	TextureManager::getSingleton().setDefaultNumMipmaps(5);
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	//add a viewport
	mRoot->getAutoCreatedWindow()->addViewport(0);	

	mInputManager = new CInputMgr();
	mInputManager->initialize(mRoot->getAutoCreatedWindow());

	//misc stuff
	mTimer.reset();
	mShutdown = false;
	mDebugOverlay = OverlayManager::getSingleton().getByName("Core/DebugOverlay");
	mDebugOverlay->show();
	mCurrentState = ZEROTH;

			 new CPlayer();
	mWorld = new CWorld("agol");

	setState(WORLD);

	mRoot->addFrameListener(this);
	mInputManager->addKeyListener(this,"appKeyListener");
}

void CApp::setState(int state)
{
	if (mCurrentState == ZEROTH)
	{
		if (state == WORLD)
		{
			CWorld::getSingletonPtr()->enterState(WORLD);
			mCurrentState = WORLD;
		}
	}
}

void CApp::run()
{
	mRoot->startRendering();
	/*
	double timeSinceLastFrame = 0;
	double startTime = 0;

	mRoot->getAutoCreatedWindow()->resetStatistics();	
	while (!mShutdown)
	{
		if (mRoot->getAutoCreatedWindow()->isClosed()) mShutdown = true;

		if (mRoot->getAutoCreatedWindow()->isActive())
		{
			startTime = mTimer.getMillisecondsCPU();			

			Ogre::WindowEventUtilities::messagePump();
			mInputManager->capture();			
			mRoot->renderOneFrame();
			update(timeSinceLastFrame);
			timeSinceLastFrame = mTimer.getMillisecondsCPU() - startTime;			
		}
	}
	*/
}

bool CApp::frameStarted(const FrameEvent &evt)
{
	mInputManager->capture();
	update(evt.timeSinceLastFrame);
	
	return !mShutdown;
}

bool CApp::keyPressed(const OIS::KeyEvent &e)
{
	if (e.key == OIS::KC_ESCAPE) mShutdown = true;

	return true;
}

void CApp::update(Real time)
{
	const RenderTarget::FrameStats& stats = mRoot->getAutoCreatedWindow()->getStatistics();
	OverlayElement *fps = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");	
	fps->setCaption("" + StringConverter::toString((int)(stats.lastFPS)));

	if (mCurrentState == WORLD) mWorld->update(time);
}