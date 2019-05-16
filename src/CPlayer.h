#ifndef CPLAYER_H
#define CPLAYER_H

#include "Ogre.h"
#include "OgreSingleton.h"
#include "OIS/OISMouse.h"
#include "OIS/OISKeyboard.h"
using namespace Ogre;
#include "iostream"

#include "CInputMgr.h"

class CPlayer : public Ogre::Singleton<CPlayer>, public OIS::KeyListener, public OIS::MouseListener
{
public:
	CPlayer();
	~CPlayer();
	
	void update(double time);
	Camera *getCamera(){return mCamera;};
	void initialize();
	void setPosition(Vector3 position);
	Vector3 getPosition(){ return mCameraNode->getPosition(); };
	void setRotation(float yaw, float pitch);

	bool keyPressed( const OIS::KeyEvent &e );
	bool keyReleased( const OIS::KeyEvent &e);
	bool mouseMoved( const OIS::MouseEvent &e);
	bool mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id){return true;};
	bool mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id){return true;};	

private:
	void moveAndRotateCamera(double time);

	Root*			mRoot;
	SceneManager*	mSmgr;
	Camera*			mCamera;
	SceneNode*		mCameraNode;
	SceneNode*		mCameraYawNode;
	SceneNode*		mCameraPitchNode;
	SceneNode*		mCameraRollNode;

	Vector3			mTranslateVector;	
	double			mMoveSpeed,mRotateSpeed;
	//position of the camera at the last frame
	Vector3			lastCameraPosition;
	//maximum position difference between two frames
	float			maxPositionOffset;

	Real			nearViewDistance, farViewDistance;
};

#endif