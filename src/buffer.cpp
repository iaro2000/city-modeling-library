/*
//*****************************************************************************************
//manual object adding
void CWorld::addTextObject(Vector3 position)
{
	if (Ogre::MeshManager::getSingletonPtr()->getByName("textObjectMesh").isNull())
	{
		ManualObject mo("textObjManual");
		mo.begin("Core/OgreText",RenderOperation::OT_POINT_LIST);
		mo.position(Vector3(0,0,0));
		mo.end();
		mo.convertToMesh("textObjectMesh");

		mSmgr->getRootSceneNode()->createChildSceneNode("textObjectNode");
	}

	Entity *ent;
	ent = mSmgr->createEntity("textObject_"+StringConverter::toString(position),"textObjectMesh");
	
	SceneNode *node = mSmgr->getSceneNode("textObjectNode")->createChildSceneNode();
	node->attachObject(ent);
	node->setPosition(position);
}
*/

/*
//*****************************************************************************************
// Test pentru lineIntersect
SEdge e;
Vector2 p1,p2,p3,p4;

	//testarea intersectiei linie-linie/ segment-segment, etc.
	p1 = Vector2(100,50);
	p2 = Vector2(0,0);
	p3 = Vector2(42.8571,21.4286);
	p4 = Vector2(99.8571,81.4286);	
	
	e.a = p1;
	e.b = p2;
	lineGraph.push_back(e);

	e.a = p3;
	e.b = p4;
	lineGraph.push_back(e);

	Vector2 resultPoint;
	int r;
	r = lineIntersect(p1,p2,p3,p4,resultPoint);
	
	const RenderTarget::FrameStats& stats = mRoot->getAutoCreatedWindow()->getStatistics();
	OverlayElement *fps = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");	
	if (r == 0)
		fps->setCaption("Intersection: LINE INTERSECT");
	else
	if (r == 1)
		fps->setCaption("Intersection: SEGMENT INTERSECT");
	else
	if (r == 2)
		fps->setCaption("Intersection: PARALLEL");
	else
	if (r == 3)
		fps->setCaption("Intersection: COINCIDENT");
	*/

	/*
	// test get point on line / get offset of point on line
	float offset;
	Vector2 p1,p2,pct;

	p1  = Vector2(100,1.34);
	p2  = Vector2(34 ,22.7);
	pct = getPointOnLine(p1,p2,0.67);
	std::cout<<"For offset 0.67: "<<pct.x<<" "<<pct.y<<std::endl;
	offset = getPointOffset(p1,p2,pct);
	std::cout<<offset<<std::endl;

	pct = getPointOnLine(p1,p2,0);
	std::cout<<"For offset 0: "<<pct.x<<" "<<pct.y<<std::endl;
	offset = getPointOffset(p1,p2,pct);
	std::cout<<offset<<std::endl;

	pct = getPointOnLine(p1,p2,1);
	std::cout<<"For offset 1: "<<pct.x<<" "<<pct.y<<std::endl;
	offset = getPointOffset(p1,p2,pct);
	std::cout<<offset<<std::endl;

	//when point does not belong to line
	p1  = Vector2(100,1.34);
	p2  = Vector2(78886,432.234);
	pct = getPointOnLine(p1,p2,0.67778);
	std::cout<<"For offset 0.67778: "<<pct.x<<" "<<pct.y<<std::endl;
	offset = getPointOffset(p1,p2,Vector2(55,66));
	std::cout<<offset<<std::endl;
	*/


/*
//*****************************************************************************************
//edgeGraph + lineGraph de test manuale
void CityCell::generateManual()
{
	cml::Edge e;

	//TODO - pozitii relative
	//ex: 0f = 0; 1f = 100
	//pentru 0.5f = e.a 50

	e.a = cml::Vector2f(100,0);
	e.b = cml::Vector2f(50,50);
	edgeGraph.push_back(e);
	bbox.addLine(e.a,e.b);

	e.a = cml::Vector2f( 50,50);
	e.b = cml::Vector2f(-50,25);
	edgeGraph.push_back(e);
	bbox.addLine(e.a,e.b);

	e.a = cml::Vector2f(-50,25);
	e.b = cml::Vector2f(-55,-65);
	edgeGraph.push_back(e);
	bbox.addLine(e.a,e.b);

	e.a = cml::Vector2f(-55,-65);
	e.b = cml::Vector2f(-35,-35);
	edgeGraph.push_back(e);
	bbox.addLine(e.a,e.b);

	e.a = cml::Vector2f(-35,-35);
	e.b = cml::Vector2f(-25,-55);
	edgeGraph.push_back(e);
	bbox.addLine(e.a,e.b);

	e.a = cml::Vector2f(-25,-55);
	e.b = cml::Vector2f(25,15);
	edgeGraph.push_back(e);
	bbox.addLine(e.a,e.b);

	e.a = cml::Vector2f(25,15);
	e.b = cml::Vector2f(100,0);
	edgeGraph.push_back(e);
	bbox.addLine(e.a,e.b);

	//make the bbox slightly bigger
	//used during lineGraph generation
	bbox.addPadding(1);
}

void CityCell::generateLineGraph()
{
	//ext->int, 1 intersect
	lineGraph.push_back(cml::Edge(cml::Vector2f(150,27),cml::Vector2f(40,26)));
	//int->int, 2 intersect
	//lineGraph.push_back(cml::Edge(cml::Vector2f(0,-15),cml::Vector2f(40,26)));
	lineGraph.push_back(cml::Edge(cml::Vector2f(40,26),cml::Vector2f(0,-15)));
	//int->int, 0 intersect
	lineGraph.push_back(cml::Edge(cml::Vector2f(40,26),cml::Vector2f(-10,15)));		
	//int->int, 0 intersect
	lineGraph.push_back(cml::Edge(cml::Vector2f(-10,15),cml::Vector2f(0,-15)));	
	//int->ext, 1 intersect
	//lineGraph.push_back(cml::Edge(cml::Vector2f(-55,0),cml::Vector2f(-10,15)));	
	lineGraph.push_back(cml::Edge(cml::Vector2f(-10,15),cml::Vector2f(-55,0)));	
	//ext->ext, 0 intersect
	lineGraph.push_back(cml::Edge(cml::Vector2f(-55,0),cml::Vector2f(-90,15)));	
	//ext->ext, 6 intersect
	lineGraph.push_back(cml::Edge(cml::Vector2f(-65,-50),cml::Vector2f(90,17)));
	//test int intersect for 4/3 intersect
	lineGraph.push_back(cml::Edge(cml::Vector2f(-802,-50),cml::Vector2f(-50,-50)));
	//int->ext, 4 intersect
	//lineGraph.push_back(cml::Edge(cml::Vector2f(-50,-50),cml::Vector2f(90,5)));
	//int->ext, 3 intersect
	lineGraph.push_back(cml::Edge(cml::Vector2f(-50,-50),cml::Vector2f(70,5)));
	//ext->ext, 2 intersect
	lineGraph.push_back(cml::Edge(cml::Vector2f( 77,38),cml::Vector2f(-29,40)));

	lineGraph.push_back(cml::Edge(cml::Vector2f(-50,25),cml::Vector2f(40,26)));

	//todo: 	
	// - int->ext, 2 intersect
	// - ext->int, 2 intersect
	// - int->ext, 3 intersect
	// - ext->int, 3 intersect

	//caz extrem - colt la edgeGraph
	//lineGraph.push_back(cml::Edge(cml::Vector2f(50,50),cml::Vector2f(30,40)));	
}

*/

//*****************************************************************************************
//cond pentru mutarea unui bbox in cadranul I
/*
	BBox box = target->getBoundingBox();

	if (box.min.x < 0)
	{
		box.max.x -= box.min.x;
		box.min.x = 0;
	}

	if (box.min.y < 0)
	{
		box.max.y -= box.min.y;
		box.min.y = 0;
	}

	target->setBoundingBox(box);
*/