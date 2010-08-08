/*
 * scene.h
 *
 *  Created on: Feb 25, 2010
 *      Author: Derek Brooks
 */

#include "shared.h"
#include "MaterialManager.h"
#include "Console.h"
#include "Camera.h"
#include "bsptree.h"
#include <string>
#include <list>
#include <map>
#include <vector>

#ifndef SCENE_H_
#define SCENE_H_

extern void cleanExit();	// defined in main.cpp

class Scene	{
public:
	int frameRate;

	int sceneWidth;
	int sceneHeight;

	Console* con;

	Camera* cam;
	vector<Camera*> *cameras;

	unsigned int polygonCount;

	MaterialManager* matsManager;

	bsp_node_t* bspRoot;

	GLUquadric* sky;
	GLuint skyCacheID;

	Scene(int width, int height);
	~Scene(void);

	void drawEntity(entity_t* ent);
	void submitBSPTree(bsp_node_t* root);

	void renderBSPTree(bsp_node_t* tree);
	void renderPolygonList(list<polygon_t*> polygons);

	// GL dominated routines
	void render(void);
	void resizeSceneSize(int width, int height);
	void performLighting();
	void drawPolygon(polygon_t* poly);
	void nameAndCachePolygons(bsp_node_t* bspNode);
	void glCachePolygon(polygon_t* polygon);
	void cacheSky();

	void exit();

	//	void advance(clock_t milliseconds);		// TODO This needs to be pushed out to the game class
};



#endif /* SCENE_H_ */
