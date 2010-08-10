/*
 * GameTest.h
 *
 *  Created on: Jul 26, 2010
 *      Author: brof
 *
 *
 *  TODO This particular todo may belong in the Game class but oh well...
 *  TODO Continued: Make it so resources can be unloaded and reloaded (vid_restart style)
 *
 *	Who am I and what am I trying to do?
 *	The game tells the renderer what dynamic objects are in the scene.

 *	Those dynamic objects are likely animateable models.  In the game
 *	we will refer it to as an entity.  Entities have masses that can be worked
 *	on and flags, possibly some AI. Entities also have bounding boxes (need to figure out
 *	we're going to create the bounding box automatically.

 *	Collision detection will chick dynamic objects against the objects in the
 *	bsp area they're located in.
 *
 *
 */
#include "Game.h"
#include "Scene.h"
#include "objloader.h"
#include "shared.h"

#define CAM_MOVE_RATE 1

// TODO move this into the Game you extend
const vec3_t GRAVITY_EARTH = {0.0f, -9.81f, 0.0f};
// TODO Remove these 3 whence the game is up and going
//vec3_t startPos = {0.0, 0.0, 0.0};
//vec3_t startAngle = {10.0, 15.0, 0.0};



#define PITCH_RATE	1
#define YAW_RATE	1
#define ROLL_RATE	1

#define SKY_TEXTURE	"bright_clouds.bmp"

// TODO REMOVE THIS
extern int pitchSpeed;


// inherit the game class
class SpecialGame : public Game	{

	bsp_node_t* bspRoot;
	list<entity_t*> entityList;
	list<list<entity_t*> > entityLeafList;

	float timeElapsed;
	float slowMotionRatio;

	MotionUnderGravitation* motionUnderGravitation;


	bool loaded;

public:

	void throwPitch(int speed, vec3_t standing, vec3_t looking)	{
		entity_t* ent = createEntity();

		ent->mass = new Mass(1);

		vec3_t vel;
		VectorMA(standing, looking, speed, vel);
		VectorCopy(standing, ent->mass->pos);
		VectorCopy(vel, ent->mass->vel);

		// 3 seconds
		ent->setTTL(3000);
		ent->collisionType = COLLISION_SPHERE;
		ent->radius = 2;

		entityList.push_back(ent);
	}

	// call this function to load different maps
	virtual void load(string mapname)	{
		createBSP(mapname);
		getScene()->submitBSPTree(bspRoot);
		loaded = true;
	}

	// TODO make this return a leaf list when doing sphere and bounding box collisions
	// because the box or sphere may be laying in more than one partition
	bsp_node_t* findBSPLeaf(const vec3_t pos)	{
		bsp_node_t* curNode = bspRoot;

		while( !curNode->isLeaf() )	{
			if( curNode->partition != NULL )	{
				float result = classifyPoint(curNode->partition, pos);
				if( result > EPSILON )	{
					curNode = curNode->front;
				}
				else if(result < -EPSILON )	{
					curNode = curNode->back;
				}
				else	{
					//TODO HANDLE SPANNING OCCURANCE
					cout << "Ent position spanning two nodes" << endl;
					//FIXME for now just going down the front
					// Will need to go down both front and back!
					curNode = curNode->front;
				}
			}
			else	{
				cout << "NULL NODE REFERENCE" << endl;
			}
		}

		if( curNode->isLeaf() )
			return curNode;

		return NULL;
	}

	void entPolyCollision(entity_t* ent, polygon_t* poly)	{
		switch( ent->collisionType )	{
		case COLLISION_NONE:	// You said it chief
			break;
		case COLLISION_BOX:		// Bounding Box vs Polygon
			// TODO perform BOX + POLY COLLISION
			break;
		case COLLISION_SPHERE:	// Sphere vs Polygon
			float time;
			plane_t* plane = new plane_t;

			// TODO figure out what to do if no normals are provided for a polygon
			if( poly->hasNormals )	{
				VectorCopy(poly->normpts[0], plane->normal);
				VectorCopy(poly->points[0], plane->origin);

				vec3_t intersect;
				vec3_t rayEnd;

				VectorAdd(ent->mass->pos, ent->mass->vel, rayEnd);

				if( (findLinePlaneIntersect(plane, ent->mass->pos, rayEnd, intersect, &time)) == 1 )	{
					if( isPointInPolygon(poly, intersect) )	{

						vec3_t dist;
						VectorSubtract(ent->mass->pos, intersect, dist);
						float distSquared = DotProduct(dist, dist);
						float rad = ent->radius + 0;	// no radius for polygon
						if( distSquared <= (rad*rad) )	{
							vec3_t reflect;
							vec3_t incident;
							float len = VectorUnitVector(ent->mass->vel, incident);
							VectorReflect(incident, poly->normpts[0], reflect);
							VectorScale(reflect, (len/3), ent->mass->vel);
						}
					}
				}
			}
			break;
		}

	}

	void entEntCollision(entity_t* ent, entity_t* otherEnt)	{
		// TODO Use flags to switch? can you set a case to box+sphere flag?
	}

	void collideWithWorld(list<polygon_t*> polyList, entity_t* ent)	{
		list<polygon_t*>::iterator itr = polyList.begin();
		for(; itr != polyList.end(); itr++)	{
			entPolyCollision(ent, (*itr));
		}
	}

/*	void collideWithOtherEntities(list<entity_t*> entList, entity_t* ent)	{
		list<entity_t*>::iterator itr = entList.begin();
		for(; itr != entList.end(); itr++)	{
			if( entEntCollision(ent, (*itr)) )	{
				// TODO Take appropirate action
				cout << "ent collided with ent." << endl;
			}
		}
	}
*/
	void insertEntityIntoBSP(entity_t* ent)	{
		bsp_node_t* node = findBSPLeaf(ent->mass->pos);

		// TODO Check for world collisions
		collideWithWorld(node->getPolygonList(), ent);

		// TODO Check for other entity collisions
//		collideWithOtherEntities(node->getEntityList(), ent);

		node->addEntity(ent);
	}

	void insertEntitiesIntoBSPTree()	{
		list<entity_t*>::iterator itr = entityList.begin();

		for(;itr != entityList.end(); itr++)
			insertEntityIntoBSP((*itr));
	}


	// This is called once every time around the game loop.
	virtual void advance(long ms)	{

		if( !loaded )
			return;


		// Time work, used for Simulation work
		// dt Is The Time Interval (As Seconds) From The Previous Frame To The Current Frame.
		// dt Will Be Used To Iterate Simulation Values Such As Velocity And Position Of Masses.
		float dt = ms / 1000.0f;							// Let's Convert Milliseconds To Seconds
		dt /= slowMotionRatio;										// Divide dt By slowMotionRatio And Obtain The New dt
		timeElapsed += dt;											// Iterate Elapsed Time
		float maxPossible_dt = 0.1f;								// Say That The Maximum Possible dt Is 0.1 Seconds
																	// This Is Needed So We Do Not Pass Over A Non Precise dt Value
	  	int numOfIterations = (int)(dt / maxPossible_dt) + 1;		// Calculate Number Of Iterations To Be Made At This Update Depending On maxPossible_dt And dt
		if (numOfIterations != 0)									// Avoid Division By Zero
			dt = dt / numOfIterations;								// dt Should Be Updated According To numOfIterations


		// Simulation work from here down
		if( motionUnderGravitation != NULL )	{

			for (int a = 0; a < numOfIterations; ++a)					// We Need To Iterate Simulations "numOfIterations" Times
			{
				// remove all ents from BSP Tree
				removeEntitiesFromBSPTree();

				// for number of masses do
				vector<entity_t*> removalList;

				for( list<entity_t*>::iterator itr = entityList.begin(); itr != entityList.end(); itr++)	{
					if( (*itr)->checkTTL() )	{
						removalList.push_back((*itr));
					}
					else	{
						motionUnderGravitation->operate(dt, (*itr)->mass);					// Iterate motionUnderGravitation Simulation By dt Seconds
					}
				}

				for( int x=0; x < removalList.size(); x++)	{
					motionUnderGravitation->release(removalList[x]->mass);
					entity_t* cur = removalList[x];
					cur->hasExpired = true;
					entityList.remove(cur);
					delete cur;	// TODO don't delete, clean out and put into free ent list.
//					cout << "Entity removed from scene! (" << entityList.size() << ")."<< endl;
				}

				insertEntitiesIntoBSPTree();
			}


		}

		getScene()->setEntityList(entityList);
	}



	// Event handlers
	// Handles keyboard input from normal text keys
	virtual void keyPressed(unsigned char key, int x, int y)	{
		Scene* curScene = getScene();

		switch(key)	{
			case '`':	// active console
				curScene->con->consoleActive = !curScene->con->consoleActive;
				break;
			case 'a':	// omg our first control over the scene!
				curScene->cam->moveCameraLeft(CAM_MOVE_RATE);
				break;
			case 'd':	// omg our first control over the scene!
				curScene->cam->moveCameraRight(CAM_MOVE_RATE);
				break;
			case 'w':	// omg our first control over the scene!
				curScene->cam->moveCameraForward(CAM_MOVE_RATE);
				break;
			case 's':	// omg our first control over the scene!
				curScene->cam->moveCameraBack(CAM_MOVE_RATE);
				break;
			case 'q':	// omg our first control over the scene!
				curScene->cam->moveCameraUp(CAM_MOVE_RATE);
				break;
			case 'e':	// omg our first control over the scene!
				curScene->cam->moveCameraDown(CAM_MOVE_RATE);
				break;
			case 'z':
				curScene->cam->rotateAboutY(-curScene->cam->yaw_rate);
				break;
			case 'x':
				curScene->cam->rotateAboutY(curScene->cam->yaw_rate);
				break;
			case 'c':
				curScene->cam->rotateAboutX(-curScene->cam->pitch_rate);
				break;
			case 'v':
				curScene->cam->rotateAboutX(curScene->cam->pitch_rate);
				break;
			case 'f':
				vec3_t r;
				VectorAdd(curScene->cam->origin, curScene->cam->normDir, r);
				throwPitch(pitchSpeed, r, curScene->cam->normDir);
				break;
			case 'g':
				vec3_t e1;
				vec3_t r1;
				r1[0] = -38;
				r1[1] = 4;
				r1[2] = 32;
				vec3_t d1;
				d1[0] = -42;
				d1[1] = 4;
				d1[2] = 36;

				VectorSubtract(d1, r1, e1);
				VectorUnitVector(e1, e1);

				throwPitch(pitchSpeed, r1, e1);


				break;


			case ESC_KEY:
				curScene->exit();
				break;
		}
	}

	// handles keyboard input from special keys
	virtual void specialKeyPressed(int key, int x, int y)	{

	}

	virtual void mouseEvent(int button, int state, int x, int y)	{

	}


	void createBSP(string mapName)	{
		Scene* curScene = getScene();
		ObjModel* obj = loadMap(mapName);
		bspRoot = new bsp_node_t;

		generateBSPTree(bspRoot, obj->polygonList);
		curScene->nameAndCachePolygons(bspRoot);

		chdir("images/");
		curScene->sky = gluNewQuadric();
		gluQuadricTexture(curScene->sky, true);
		gluQuadricOrientation(curScene->sky, GLU_INSIDE);
		getMaterialManager()->loadBitmap(SKY_TEXTURE);
		chdir("..");
		curScene->cacheSky();
	}

	ObjModel* loadMap(string map)	{
		// TODO FIX THIS SLOPPYNESS AND THE STUFF WHEN LOADING
		// BMP FILES TOO, THIS IS JUST A TEMP HACK TO CLEAN UP
		// THE ROOT FOLDER OF THIS PROJECT.
		chdir("..");
		ObjModel* obj = new ObjModel();
		obj->loadObjFile(map);
		return obj;
	}

	void createDynamicLeafList(bsp_node_t* root, bool start)	{
		if( start )
			entityLeafList.clear();

		if( root->isLeaf() )	{
			entityLeafList.push_back(root->getEntityList());
		}
		else	{
			createDynamicLeafList(root->front, false);
			createDynamicLeafList(root->back, false);
		}
	}

	void removeEntitiesFromBSPTree()	{
		list<list<entity_t*> >::iterator itr;
		for(itr=entityLeafList.begin(); itr != entityLeafList.end(); itr++)
			(*itr).clear();
	}

	void addEntity(entity_t* ent)	{
		entityList.push_back(ent);
	}

	void removeEntity(entity_t* ent)	{
		entityList.remove(ent);
	}


	SpecialGame() : Game()	{
		loaded = false;
		timeElapsed = 0.0;
		slowMotionRatio = 1.0;

		motionUnderGravitation = new MotionUnderGravitation(GRAVITY_EARTH);
	}

	~SpecialGame()	{

	}
};
