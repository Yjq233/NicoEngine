#pragma once

#include "vector.h"


struct stPointMass
{
	stPointMass() : m(0) {}
	float m;
	CVector3 pos;
	CVector3 velocity;
	CVector3 force;
};


inline void ApplyForce(CVector3 force, stPointMass *m)
{
	if (m != 0) m->force += force;
}


inline void SimulateMass(float dt, stPointMass *m)
{
	// Calculate new velocity and position.
	if (m != 0)
	{
		m->velocity += (m->force / m->m) * dt;
		m->pos += m->velocity * dt;
	}
}
