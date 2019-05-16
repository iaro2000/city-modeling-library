#include "CPlayer.h"

template<> CPlayer* Ogre::Singleton<CPlayer>::ms_Singleton = 0;

CPlayer::CPlayer()
{
	mRoot = Ogre::Root::getSingletonPtr();	
}

CPlayer::~CPlayer()
{

}

void CPlayer::initialize()
{
	CInputMgr::getSingletonPtr()->addKeyListener(this,"playerKeyListener");
	CInputMgr::getSingletonPtr()->addMouseListener(this,"playerMouseListener");

	mSmgr = mRoot->getSceneManager("smgr");
	mCamera  = mSmgr->createCamera("camera");
	

	mMoveSpeed = 500;
	mRotateSpeed = 0.13;
	mTranslateVector = Vector3(0,0,0);
	maxPositionOffset = 1.0f;
	nearViewDistance = 0.01;
	farViewDistance = 10000;

	mCamera->setNearClipDistance(nearViewDistance);
	mCamera->setFarClipDistance(farViewDistance);

	mCameraNode = mSmgr->getRootSceneNode()->createChildSceneNode();
	mCameraYawNode = mCameraNode->createChildSceneNode();
	mCameraPitchNode = mCameraYawNode->createChildSceneNode();
	mCameraRollNode = mCameraPitchNode->createChildSceneNode();
	mCameraRollNode->attachObject(mCamera);
	mCameraNode->setPosition(Vector3(0,0,0));
	mCamera->setNearClipDistance(5);
}

void CPlayer::moveAndRotateCamera(double time)
{
	if (time != 0)
	{
		lastCameraPosition = getPosition();

		mCameraNode->translate(
			  mCameraYawNode->getOrientation()
			* mCameraPitchNode->getOrientation()
			* mTranslateVector * time,
			Ogre::SceneNode::TS_LOCAL);

		Vector3 diff = getPosition() - lastCameraPosition;
		if (diff.x < 0 ) diff.x -= diff.x;
		if (diff.y < 0 ) diff.y -= diff.y;
		if (diff.z < 0 ) diff.z -= diff.z;
		if ((diff.x > maxPositionOffset)
			||
			(diff.y > maxPositionOffset)
			||
			(diff.x > maxPositionOffset))
		{
			//setPosition(lastCameraPosition);
		}
	}

}

void CPlayer::update(double time)
{
	moveAndRotateCamera(time);
}

void CPlayer::setPosition(Vector3 position)
{
	mCameraNode->setPosition(position);
	lastCameraPosition = position;
}

void CPlayer::setRotation(float yaw, float pitch)
{
	mCameraYawNode->yaw(Degree(yaw));
	mCameraPitchNode->pitch(Degree(pitch));
}

bool CPlayer::keyPressed( const OIS::KeyEvent &e )
{
	if (e.key == OIS::KC_W)
	{
		mTranslateVector.z = -mMoveSpeed;
	}

	if (e.key == OIS::KC_S)
	{
		mTranslateVector.z = mMoveSpeed;
	}

	if (e.key == OIS::KC_A)
	{
		mTranslateVector.x = -mMoveSpeed;
	}

	if (e.key == OIS::KC_D)
	{
		mTranslateVector.x = mMoveSpeed;
	}

	if (e.key == OIS::KC_B)
	{
		farViewDistance += 200;
		mCamera->setFarClipDistance(farViewDistance);
	}

	if (e.key == OIS::KC_N)
	{
		farViewDistance -= 200;
		mCamera->setFarClipDistance(farViewDistance);
	}

	if (e.key == OIS::KC_M)
	{
		farViewDistance = 4000;
		mCamera->setFarClipDistance(farViewDistance);
	}

	return true;
}

bool CPlayer::keyReleased( const OIS::KeyEvent &e )
{
	
	if (e.key == OIS::KC_W)
	{
		mTranslateVector.z = 0;
	}

	if (e.key == OIS::KC_S)
	{
		mTranslateVector.z = 0;
	}

	if (e.key == OIS::KC_A)
	{
		mTranslateVector.x = 0;
	}

	if (e.key == OIS::KC_D)
	{
		mTranslateVector.x = 0;
	}

	return true;
}

bool CPlayer::mouseMoved( const OIS::MouseEvent &e)
{
	mCameraYawNode->yaw(Degree(-e.state.X.rel * mRotateSpeed),	   Node::TS_LOCAL);
	mCameraPitchNode->pitch(Degree(-e.state.Y.rel * mRotateSpeed), Node::TS_LOCAL);
	
	return true;
}
