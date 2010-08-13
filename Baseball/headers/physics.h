/**************************************************************************

  File: physics.h
	Portions of the code in this file was derived from the works of:
	Erkin Tunca for http://nehe.gamedev.net/
 	More specifically the introduction to physical simulations lesson.
	Thanks for the lesson.

**************************************************************************/


#ifndef PHYSICS_H_
#define PHYSICS_H_

#include <math.h>
#include "shared.h"

// class Mass			---> An object to represent a mass
class Mass	{
public:
	float m;									// The mass value
	vec3_t prevPos;							// previous position
	vec3_t pos;								// Position in space
	vec3_t vel;								// Velocity
	vec3_t force;								// Force applied on this mass at an instance

	Mass(float m)	{
		this->m = m;
	}

	void applyForce(vec3_t force)	{
		VectorAdd(this->force, force, this->force);
	}

	void init()	{
		force[0] = 0;
		force[1] = 0;
		force[2] = 0;
	}

//	  void simulate(float dt) method calculates the new velocity and new position of
//	  the mass according to change in time (dt). Here, a simulation method called
//	  "The Euler Method" is used. The Euler Method is not always accurate, but it is
//	  simple. It is suitable for most of physical simulations that we know in common
//	  computer and video games.
	void simulate(float dt)	{
		vec3_t velocityDelta;

		VectorDivide(force, m, velocityDelta);
		VectorMA(vel, velocityDelta, dt, vel);

		VectorCopy(pos, prevPos);		// save old position

		VectorMA(pos, vel, dt, pos);	// get new one!
	}

};

// Abstract class Simulation
class Simulation	{
public:
	
	Simulation()	{
	}

	virtual void release(Mass* mass)	{
		delete(mass);
		mass = NULL;
	}

	virtual void init(Mass* mass)	{
		mass->init();
	}

	virtual void solve(Mass* mass)	{
	}

	virtual void simulate(float dt, Mass* mass)	{
		mass->simulate(dt);
	}

	virtual void operate(float dt, Mass* mass)	{
		init(mass);
		solve(mass);
		simulate(dt, mass);
	}

};

//  class ConstantVelocity is derived from class Simulation
//  It creates 1 mass with mass value 1 kg and sets its velocity to (1.0f, 0.0f, 0.0f)
//  so that the mass moves in the x direction with 1 m/s velocity.
/*
class ConstantVelocity : public Simulation	{
public:
	ConstantVelocity() : Simulation(1, 1.0f)	{
		vec3_t startPosition = {0.0f, 0.0f, 0.0f};
		vec3_t startVelocity = {1.0f, 0.0f, 0.0f};

		VectorCopy(startPosition, masses[0]->pos);
		VectorCopy(startVelocity, masses[0]->vel);
	}

};
*/
//	class MotionUnderGravitation is derived from class Simulation
//	It creates 1 mass with mass value 1 kg and sets its velocity to (10.0f, 15.0f, 0.0f) and its position to
//	(-10.0f, 0.0f, 0.0f). The purpose of this application is to apply a gravitational force to the mass and
//	observe the path it follows. The above velocity and position provides a fine projectile path with a
//	9.81 m/s/s downward gravitational acceleration. 9.81 m/s/s is a very close value to the gravitational
//	acceleration we experience on the earth.
class MotionUnderGravitation : public Simulation	{
public:
	vec3_t gravitation;													//the gravitational acceleration

	MotionUnderGravitation(const vec3_t gravitation) : Simulation()	{																		//Vector3D gravitation, is the gravitational acceleration
		VectorCopy(gravitation, this->gravitation);
	}

	virtual void solve(Mass* mass)	{
		vec3_t force;
		VectorScale(gravitation, mass->m, force);
		mass->applyForce(force);
	}
	
};

//	class MassConnectedWithSpring is derived from class Simulation
//	It creates 1 mass with mass value 1 kg and binds the mass to an arbitrary constant point with a spring.
//	This point is refered as the connectionPos and the spring has a springConstant value to represent its
//	stiffness.
/*
class MassConnectedWithSpring : public Simulation	{
public:
	float springConstant;
	vec3_t connectionPos;

	MassConnectedWithSpring(const float springConstant) : Simulation(1, 1.0f)	{
		this->springConstant = springConstant;

		vec3_t attachmentPosition = {0.0f, -5.0f, 0.0f};

		VectorCopy(attachmentPosition, connectionPos);

		vec3_t pullBackDistance = {10.0f, 0.0f, 0.0f};
		vec3_t startVelocity = {0.0, 0.0, 0.0};

		VectorAdd(connectionPos, pullBackDistance, masses[0]->pos);
		VectorCopy(startVelocity, masses[0]->vel);

	}

	virtual void solve()	{
		vec3_t springVector;
		vec3_t springForce;

		for (int a = 0; a < numOfMasses; ++a)	{
			VectorSubtract(masses[a]->pos, connectionPos, springVector);
			VectorNegate(springVector, springVector);
			VectorScale(springVector, springConstant, springForce);
			masses[a]->applyForce(springForce);
		}
	}
};
*/

#endif	// ENDIF PHYSICS_H_


