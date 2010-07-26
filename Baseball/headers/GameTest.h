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
 *
 *
 */
#include "Game.h"
#include "Scene.h"




// inherit the game class
class SpecialGame : public Game	{


public:

	SpecialGame() : Game()	{

	}

	~SpecialGame()	{

	}

	// This is called once every time around the game loop.
	virtual void advance(long ms)	{
/*		// Time work, used for Simulation work
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

		for (int a = 0; a < numOfIterations; ++a)					// We Need To Iterate Simulations "numOfIterations" Times
		{
			motionUnderGravitation->operate(dt);					// Iterate motionUnderGravitation Simulation By dt Seconds
		}
		*/
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
				curScene->doItAgain();
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
		Scene* curScene = getScene();
		curScene->doPick(button, state, x, y);
	}


};
