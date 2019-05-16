#ifndef EDGE_H
#define EDGE_H

#include "Vector2.h"

namespace cml
{

class Edge
{
public:
	Vector2f a;
	Vector2f b;
	//nu trebuie decat un vecin (deocamdata)
	//deci vector<Edge> <=> lista circulara ce defineste o celula
	//std::vector<int> anb;
	//std::vector<int> bnb;
	int anb;
	int bnb;
	bool	enabled;
	//TODO: shimbat numele in partOfCellBoundary
	bool	partOfEdgeGraph;

	Edge()
	{
		enabled = true;
		partOfEdgeGraph = true;
		anb = bnb = -1;
	}

	Edge(Vector2f p1, Vector2f p2)
	{
		enabled = true;
		partOfEdgeGraph = true;
		a = p1;
		b = p2;
		anb = bnb = -1;
	}

	//doar dpdv geometric, nu obiectual.
	//de exemplu nu testeaza flag-uri 
	//enabled sau partOfEdgeGraph
	bool operator==(const cml::Edge& e) const
	{
		//edge direction does not matter
		return ((e.a == a) && (e.b == b))
				||
			   ((e.a == b) && (e.b == a));
	}
};

}

#endif