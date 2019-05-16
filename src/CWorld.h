#ifndef CWORLD_H
#define CWORLD_H

#include "CML.h"

#include "Ogre.h"
#include "OgreSingleton.h"
#include "OgreConfigFile.h"
using namespace Ogre;
#include "iostream"

#include "CPlayer.h"

class CWorld : public Ogre::Singleton<CWorld>, public OIS::KeyListener
{
public:
	CWorld(String name);
	~CWorld();
	
	void update(double time);
	void enterState(int state);
	void exitState(int state);
	void constructCell(const cml::CityCell *cell,String uniqueName);
	void createMiscelaneousData();

	bool keyPressed( const OIS::KeyEvent &e );
	bool keyReleased( const OIS::KeyEvent &e);

private:
	void initialize();
	void generate();

	Root*			mRoot;
	SceneManager*	mSmgr;

	CPlayer*		mPlayer;
	cml::City*		city;
	bool			cityDefinitionFileParsed;

	SceneNode*		debugCursor;
};

#endif