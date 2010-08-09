/*
 * BSPTree.h
 *
 *  Created on: Jun 4, 2010
 *      Author: Derek Brooks
 */

#include "ent.h"


#ifndef BSPTREE_H_
#define BSPTREE_H_
#include <list>


// MAYBE FIXME
// Recursion depth now works up to but not including 7
// don't know why, don't care at the moment.
#define BSP_RECURSION_DEPTH		5

#define		PLANE_NORMAL_X		0
#define 	PLANE_NORMAL_Y		1
#define		PLANE_NORMAL_Z		2


// TODO: Consider making BSPNode class instead
typedef struct bsp_node_s	{

	plane_t *partition;
	bsp_node_s *parent;
	bsp_node_s *front;
	bsp_node_s *back;

	private:
	list<polygon_t*> polygonList;
	list<entity_t*> entityList;


	public:

	// node helpers

	bool isLeaf()	{
		if(front)
			return false;

		return true;
	}


	// Encapsulate the polygon list so we aren't
	// tempted to modify it elsewhere.

	void addPolygon(polygon_t* poly)	{
		polygonList.push_back(poly);
	}

	void removePolygon(polygon_t* poly)	{
		polygonList.remove(poly);
	}

	list<polygon_t*>::iterator beginPolyListItr()	{
		return polygonList.begin();
	}

	list<polygon_t*>::iterator endPolyListItr()	{
		return polygonList.end();
	}

	list<polygon_t*> getPolygonList()	{
		return polygonList;
	}

	void setPolygonList(list<polygon_t*> polyList)	{
		polygonList = polyList;
	}

	int getPolygonCount()	{
		return polygonList.size();
	}

	void addEntity(entity_t* ent)	{
		entityList.push_back(ent);
	}

	list<entity_t*> getEntityList()	{
		return entityList;
	}

}bsp_node_t;


void buildTree(const float planeLen, const float nextCenter, plane_t* partition, bsp_node_t* parent_node);
void deleteTree(bsp_node_t* bspRoot);
void generateBSPTree(bsp_node_t* root, list<polygon_t*> polygonList);

#endif /* BSPTREE_H_ */
