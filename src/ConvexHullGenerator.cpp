#include "ConvexHullGenerator.h"

cml::ConvexHullGenerator::ConvexHullGenerator()
{

}

cml::ConvexHullGenerator::~ConvexHullGenerator()
{

}

void cml::ConvexHullGenerator::setTarget(CityCell *c)
{
	target = c;
}

void cml::ConvexHullGenerator::setParameters(std::string p)
{
	//Parameters:
	//a b c d - bounding box
	//e - maximum number of points

	std::stringstream ss(std::stringstream::in | std::stringstream::out);
	float minx,miny,maxx,maxy;
	int maxNr;

	ss<<p;
	ss>>minx>>miny>>maxx>>maxy>>maxNr;

	bbox = BBox(cml::Vector2f(minx,miny),cml::Vector2f(maxx,maxy));

	//minim 3 puncte
	pointNr = 3+rand()%(maxNr-3);
}

void cml::ConvexHullGenerator::generate()
{	
	std::vector< cml::Vector2f > p,q,w;
	cml::Vector2f p0,externalPoint;
	int p0pos;
	//starting point - highest-rightmost
	//TODO: (maybe) - highest-leftmost
	p0 = bbox.max;

	//a point outside the boundareis used for generating points
	//used below
	externalPoint = bbox.min - cml::Vector2f(1,1);

	//p - initial set of points
	//q = p - {p0} where p0 = lowest rightmost point
	//w - point stack. used for calculating the hull

	//generate points and find the lowest-rightmost one (p0)
	for (int i=0;i<pointNr;i++)
	{
		cml::Vector2f v;
		v.x = bbox.min.x + rand()%(int)(bbox.getLength());
		v.y = bbox.min.y + rand()%(int)(bbox.getWidth());

		if (v.y < p0.y)
		{
			p0 = v;
		}
		else
			if (v.y == p0.y)
				if (v.x > p0.x)
				{
					p0 = v;
					p0pos = i;
				}

		//de debug
		//target->addEdge(cml::Edge(v,v+cml::Vector2f(5,5)));
		p.push_back(v);
	}
	//de debug
	//target->addEdge(cml::Edge(p0,p0+cml::Vector2f(-15,-15)));

	for (int i=0;i<p.size();i++)
		if (p[i] != p0)
		{
			q.push_back(p[i]);
		}
	
	//sorteaza in functie de unghi
	bool sort = false;
	while (!sort)
	{
		sort = true;
		for (int i=0;i<q.size()-1;i++)
		{			
			if (isLeft(p0,q[i+1],q[i]) < 0 )
			{
				cml::Vector2f temp;
				temp = q[i];
				q[i] = q[i+1];
				q[i+1] = temp;

				sort = false;
			}
			else
			if (isLeft(p0,q[i+1],q[i]) == 0 )
			{
				float dist1,dist2;
				dist1 = sqrt((p0.x-q[i+1].x)*(p0.x-q[i+1].x)+(p0.y-q[i+1].y)*(p0.y-q[i+1].y));
				dist2 = sqrt((p0.x-q[i].x)*(p0.x-q[i].x)+(p0.y-q[i].y)*(p0.y-q[i].y));

				if (dist1 >= dist2) q[i]   = externalPoint;
				else				q[i+1] = externalPoint;

				sort = false;
			}
		}
	}

	//repopulate p. p0 = first vertex; + vertices of q
	//externalPoint will not be reintroduced
	p.clear();
	p.push_back(p0);
	for (int i=0;i<q.size();i++)
		if (q[i] != externalPoint) p.push_back(q[i]);

	w.push_back(p[0]);
	w.push_back(p[1]);

	for (int i=2;i<p.size();i++)
	{
		while ((w.size() >=2) && (isLeft(w[w.size()-2],w[w.size()-1],p[i]) > 0))
		{			
			w.pop_back();
		}
		w.push_back(p[i]);		
	}

	//construct edges from the points
	Edge e;
	for (int i=0;i<w.size()-1;i++)
	{
		e.a = w[i];
		e.b = w[i+1];
		if (i == 0)
		{
			e.anb = w.size()-1;
		}
		else
		{
			e.anb = i-1;
		}		
		e.bnb = i+1;
		e.partOfEdgeGraph = true;
		target->addEdge(e);
	}
	e.a = w[w.size()-1];
	e.b = w[0];
	e.anb = w.size()-2;
	e.bnb = 0;
	e.partOfEdgeGraph = true;
	target->addEdge(e);

	//clear temporary data
	p.clear();
	q.clear();
	w.clear();
}

void cml::ConvexHullGenerator::reset()
{

}

int cml::ConvexHullGenerator::isLeft(cml::Vector2f p0, cml::Vector2f p1, cml::Vector2f p2)
{
	//ifLeft e de fapt semnul diferentei a doua produse vectoriale
	//anume 
	return ( (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y) );    
}