#include "CWorld.h"

template<> CWorld* Ogre::Singleton<CWorld>::ms_Singleton = 0;

CWorld::CWorld(String name)
{
	CInputMgr::getSingletonPtr()->addKeyListener(this,"worldKeyListener");
	//initialize the scene manager
	mRoot = Ogre::Root::getSingletonPtr();
	mSmgr = mRoot->createSceneManager(ST_EXTERIOR_CLOSE,"smgr");

	//player (avatar) configuration
	mPlayer = CPlayer::getSingletonPtr();
	mPlayer->initialize();
	mPlayer->setPosition(Vector3(0,200,0));
	mPlayer->setRotation(0,-90);
	
	//mSmgr->setFog(FOG_LINEAR,ColourValue(0.3,0.5,0.8),0.0,20,4000);
	
	//Scene node for the city structure
	mSmgr->getRootSceneNode()->createChildSceneNode("cityNode");

	//debug cursor (yellow cross)
	createMiscelaneousData();

	city = new cml::City();

	cityDefinitionFileParsed = false;	
}

CWorld::~CWorld()
{

}

void CWorld::initialize()
{	
	//seed-ul unic pentru oras. Toate deriva de aici
	//incarcat in prealabil la loadCity
	//srand(city->seed)
	/*
	srand(10598076);
	city->generate();
	
	cml::CityCell *cell;
	cell = city->getCell(0,0);

	constructCell(cell);
	*/
}

void CWorld::generate()
{
	Entity *ent;
	SceneNode *node;
	String name;
	node = mSmgr->getSceneNode("cityNode");

	SceneNode::ObjectIterator iter = node->getAttachedObjectIterator();
	SceneNode::ChildNodeIterator citer = node->getChildIterator();

	//remove all entities attached to the scene node
	//and their corresponding meshes
	while (iter.hasMoreElements())
	{
		ent = (Entity*)iter.getNext();
		MeshManager::getSingletonPtr()->remove(ent->getMesh()->getName());
		mSmgr->destroyEntity(ent);
	}

	//remove all entities attached to the child
	//scene nodes and their corresponding meshes
	//as well as the child scene nodes
	while (citer.hasMoreElements())
	{
		SceneNode *n = (SceneNode*)citer.getNext();
		iter = n->getAttachedObjectIterator();
		//remove all entities and meshes
		while (iter.hasMoreElements())
		{
			ent = (Entity*)iter.getNext();
			MeshManager::getSingletonPtr()->remove(ent->getMesh()->getName());
			mSmgr->destroyEntity(ent);
		}
		//detach the child from the parent node
		node->removeChild(n);
		mSmgr->destroySceneNode(n);
	}

	city->reset();

	city->generate();
	
	cml::CityCell *cell;
	/*
	for (int i=0;i<city->getNumberOfCellsForLevel(0);i++)
	{
		cell = city->getCell(0,i);
		name  = "";
		name += StringConverter::toString(0);
		name += StringConverter::toString(i);
		constructCell(cell,name);
	}
	*/

	for (int i=0;i<city->getNumberOfCellsForLevel(1);i++)
	{
		cell = city->getCell(1,i);
		name  = "";
		name += StringConverter::toString(1);
		name += StringConverter::toString(i);
		constructCell(cell,name);
	}
	
	for (int i=0;i<city->getNumberOfCellsForLevel(2);i++)
	{
		cell = city->getCell(2,i);
		name  = "";
		name += StringConverter::toString(2);
		name += StringConverter::toString(i);
		constructCell(cell,name);
	}	

	mSmgr->getSceneNode("cityNode")->setScale(Ogre::Vector3(4,4,4));
}

void CWorld::enterState(int state)
{
	mRoot->getAutoCreatedWindow()->getViewport(0)->setCamera(mPlayer->getCamera());
}

void CWorld::exitState(int state)
{

}

void CWorld::update(double time)
{
	mPlayer->update(time);

	debugCursor->setPosition(mPlayer->getPosition().x,0,mPlayer->getPosition().z-50);

	OverlayElement *fps = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");	
	std::string s;
	s  = "Cursor ";
	s += debugCursor->getPosition().x;
	s += " ";
	s += debugCursor->getPosition().z;	
	fps->setCaption(StringConverter::toString(debugCursor->getPosition()));
}

void CWorld::createMiscelaneousData()
{
	Entity *ent;
	SceneNode *node;
	debugCursor = mSmgr->getRootSceneNode()->createChildSceneNode("debugCursorNode");

	ManualObject mo("dbgCursor");
	mo.begin("debugDataMaterial",RenderOperation::OT_LINE_LIST);
	mo.position( 10,0,  0);
	mo.position(-10,0,  0);
	mo.position(  0,0, 10);
	mo.position(  0,0,-10);
	mo.end();
	mo.convertToMesh("cursorMesh");

	ent = mSmgr->createEntity("cursorEntity","cursorMesh");
	debugCursor->attachObject(ent);

	float size = 10000;
	float texRepeat = 300;
	float height = -1;
	ManualObject gridMo("grid");
	gridMo.begin("gridMaterial",RenderOperation::OT_TRIANGLE_LIST);
	//first triangle
	gridMo.position( -size,height, size);
	gridMo.textureCoord(0,texRepeat);
	gridMo.position(  size,height, size);
	gridMo.textureCoord(texRepeat,texRepeat);
	gridMo.position( -size,height,-size);
	gridMo.textureCoord(0,0);
	//second triangle
	gridMo.position(  size,height, size);
	gridMo.textureCoord(texRepeat,texRepeat);
	gridMo.position(  size,height,-size);
	gridMo.textureCoord(texRepeat,0);
	gridMo.position( -size,height,-size);
	gridMo.textureCoord(0,0);
	gridMo.end();
	gridMo.convertToMesh("gridMesh");

	ent = mSmgr->createEntity("gridEntity","gridMesh");
	node = mSmgr->getRootSceneNode()->createChildSceneNode("gridNode");
	node->attachObject(ent);

	mSmgr->setSkyBox(true,"sky1Material");

}

void CWorld::constructCell(const cml::CityCell *cell,String uniqueName)
{
	const std::vector<cml::Edge>& edgeGraphData = cell->getEdgeGraphData();
	const std::vector<cml::Edge>& lineGraphData = cell->getLineGraphData();
	const std::vector<cml::Edge>& debugData	  = cell->getDebugData();
	const std::vector<cml::Triangle3f>& roadGeometryData = cell->getRoadGeometryData();
	const std::vector<cml::Triangle3f>& cellGeometryData = cell->getCellGeometryData();
	const std::vector<std::string>& cellGeometryMaterialList = cell->getCellGeometryMaterialList();
	const std::vector<cml::Prefab>& prefabData = cell->getPrefabData();
	const cml::BBox& bbox = cell->getBoundingBox();
	Entity *ent;
	SceneNode *node;

	node = mSmgr->getSceneNode("cityNode");
	ManualObject mo("cellManualObject");
	//convert edge graph to graphical representation
	//if (edgeGraphData.size() > 0)
	//temporar - trebuie metoda mai buna de generare/vizibilitate
	if (0)
	{		
		mo.begin("edgeGraphMaterial",RenderOperation::OT_LINE_LIST);	
		for (unsigned int i=0;i<edgeGraphData.size();i++)
		{
			if (edgeGraphData[i].enabled)
			{
				mo.position(edgeGraphData[i].a.x,0,edgeGraphData[i].a.y);
				mo.position(edgeGraphData[i].b.x,0,edgeGraphData[i].b.y);
			}
		}
		mo.end();
	}
	
	//convert line graph to graphical representation
	//if (lineGraphData.size() > 0)
	//temporar - trebuie metoda mai buna de generare/vizibilitate
	if (0)
	{
		mo.begin("lineGraphMaterial",RenderOperation::OT_LINE_LIST);
		for (unsigned int i=0;i<lineGraphData.size();i++)
		{
			//mo.position(lineGraphData[i].a.x,0.01,lineGraphData[i].a.y);
			//mo.position(lineGraphData[i].b.x,0.01,lineGraphData[i].b.y);
			mo.position(lineGraphData[i].a.x,17,lineGraphData[i].a.y);
			mo.position(lineGraphData[i].b.x,17,lineGraphData[i].b.y);
		}
		mo.end();
	}

	//construct the axes
	/*
	mo.begin("axesMaterial",RenderOperation::OT_LINE_LIST);
		mo.position(-1000,0, 0);
		mo.position( 1000,0, 0);
		mo.position( 0   ,0,-1000);
		mo.position( 0   ,0, 1000);
	mo.end();
	*/

	//construct the neighbour debug data
	if (debugData.size() > 0)
	{
		mo.begin("debugDataMaterial",RenderOperation::OT_LINE_LIST);
		for (unsigned int i=2;i<debugData.size();i++)
		{
			mo.position(debugData[i].a.x,0.1,debugData[i].a.y);
			mo.position(debugData[i].b.x,0.1,debugData[i].b.y);
		}
		mo.end();
	}

	//construct the road geometry
	if (roadGeometryData.size() > 0)
	{
		mo.begin("road1Material",RenderOperation::OT_TRIANGLE_LIST);
		for (unsigned int i=0;i<roadGeometryData.size();i++)
		{
			cml::Triangle3f t;
			t = roadGeometryData[i];

			mo.position(t.a.x,t.a.y,t.a.z);
			mo.normal(Ogre::Vector3(t.a.x,t.a.y,t.a.z).normalisedCopy());
			mo.textureCoord(t.atcoord.x,t.atcoord.y);

			mo.position(t.b.x,t.b.y,t.b.z);
			mo.normal(Ogre::Vector3(t.b.x,t.b.y,t.b.z).normalisedCopy());
			mo.textureCoord(t.btcoord.x,t.btcoord.y);

			mo.position(t.c.x,t.c.y,t.c.z);
			mo.normal(Ogre::Vector3(t.c.x,t.c.y,t.c.z).normalisedCopy());
			mo.textureCoord(t.ctcoord.x,t.ctcoord.y);			
		}
		mo.end();
	}

	//construct the cell geometry
	if (cellGeometryData.size() > 0)
	{
		for (int i=0;i<cellGeometryMaterialList.size();i++)
		{
			mo.begin(cellGeometryMaterialList[i],RenderOperation::OT_TRIANGLE_LIST);
			for (unsigned int j=0;j<cellGeometryData.size();j++)
			{
				cml::Triangle3f t;
				t = cellGeometryData[j];

				if (t.material == cellGeometryMaterialList[i])
				{
					mo.position(t.a.x,t.a.y,t.a.z);
					mo.normal(Ogre::Vector3(t.a.x,t.a.y,t.a.z).normalisedCopy());
					mo.textureCoord(t.atcoord.x,t.atcoord.y);

					mo.position(t.b.x,t.b.y,t.b.z);
					mo.normal(Ogre::Vector3(t.b.x,t.b.y,t.b.z).normalisedCopy());
					mo.textureCoord(t.btcoord.x,t.btcoord.y);

					mo.position(t.c.x,t.c.y,t.c.z);
					mo.normal(Ogre::Vector3(t.c.x,t.c.y,t.c.z).normalisedCopy());
					mo.textureCoord(t.ctcoord.x,t.ctcoord.y);			
				}
			}
			mo.end();
		}
	}

	//construct the bounding box
	if (cell->isBoundingBoxVisible())
	{
		mo.begin("debugDataMaterial",RenderOperation::OT_LINE_LIST);
			mo.position(bbox.min.x ,0,bbox.max.y);
			mo.position(bbox.max.x,0,bbox.max.y);

			mo.position(bbox.max.x,0,bbox.max.y);
			mo.position(bbox.max.x,0,bbox.min.y);

			mo.position(bbox.max.x,0,bbox.min.y);
			mo.position(bbox.min.x,0,bbox.min.y);

			mo.position(bbox.min.x,0,bbox.min.y);
			mo.position(bbox.min.x,0,bbox.max.y);
		mo.end();
	}
	
	//convert the manual object to a mesh
	mo.convertToMesh("cellMesh_"+uniqueName);	
	mSmgr->destroyManualObject("cellManualObject");
	//create an entity from the generated mesh
	ent = mSmgr->createEntity("cellEntity_"+uniqueName,"cellMesh_"+uniqueName);	
	//attach it to a scene node	
	node->attachObject(ent);

	//finished with manual objects
	//now add prefabs if needed
	//ent = mSmgr->createEntity("cellEntity_prefab_"+uniqueName,"cube.mesh");
	//node->attachObject(ent);

	if (prefabData.size() > 0)
	{
		String name;
		for (int i=0;i<prefabData.size();i++)
		{
			cml::Prefab p;
			SceneNode* n;
			p = prefabData[i];

			name  = "";			
			name += StringConverter::toString(i);

			ent = mSmgr->createEntity("cellEntity_"+uniqueName+"_prefab_"+name,p.resourceName);			
			n = node->createChildSceneNode();
			n->attachObject(ent);
			n->setPosition(Ogre::Vector3(p.position.x,p.position.y,p.position.z));
			n->setDirection(Ogre::Vector3(p.rotation.x,p.rotation.y,p.rotation.z));
			n->setScale(Ogre::Vector3(p.scale.x,p.scale.y,p.scale.z));
		}
	}
}

bool CWorld::keyPressed( const OIS::KeyEvent &e )
{
	return true;
}

bool CWorld::keyReleased( const OIS::KeyEvent &e )
{
	if (e.key == OIS::KC_L)
	{
		std::string path;
		std::cin>>path;
		path = "../data/"+path;
		//de pus parse... intr-un eveniment ca in LSTree - textbox
		city->parseCityDefinitionFile(path);
		cityDefinitionFileParsed = true;
	}
	
	if (e.key == OIS::KC_G)
	{
		if (cityDefinitionFileParsed)
		{
			generate();
		}
		else
		{
			std::cout<<"City definition file not yet loaded!\n";
		}
	}

	return true;
}