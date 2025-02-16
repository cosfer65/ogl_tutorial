// code created by David M. Bourg in the book Physics for Game Developers
// all units are in the english system

#ifndef __airplane__
#define __airplane__

#include "quaternion.h"

using namespace atlas;

#define Matrix3x3 mat3
#define clf_vector3D vec3
#define clf_quaternion quaternion
#define crossProduct cross
#define DegreesToRadians dtr
#define dotProduct dot
#define RadiansToDegrees rtd

#define	_DTHRUST	1.0f

//------------------------------------------------------------------------//
// Notes:
//------------------------------------------------------------------------//
/*

    Earth coordinates:	x points North
                        y points West
                        z points up

                        Z   X
                        |  /
                        | /
                        |/
                 Y------+

    Body coordinates:	x points to the front
                        y points to left
                        z points up

    Note:	Direct3D's coordinates system is a left handed one with the z-axis pointing
            into the screen, the x-axis pointing to the right and the y-axis pointing up

    Units:	English system,
                            distance	--> feet
                            time		-->	seconds
                            mass		--> slugs
                            force		--> pounds
                            moment		--> foot-pounds
                            velocity	--> feet-per-second
                            acceleration-->	feet-per-second^2
                            density		--> slugs-per-feet^3

*/

//------------------------------------------------------------------------//
// Rigid body structure
//------------------------------------------------------------------------//
struct RigidBody {
    float		fMass;			// total mass (constant)
    Matrix3x3	mInertia;		// mass moment of inertia in body coordinates (constant)
    Matrix3x3	mInertiaInverse;// inverse of mass moment of inertia matrix	(constant)

    clf_vector3D		vPosition;		// position in earth coordinates
    clf_vector3D		vVelocity;		// velocity in earth coordinates
    clf_vector3D		vVelocityBody;	// velocity in body coordinates
    clf_vector3D		vAngularVelocity;// angular velocity in body coordinates
    clf_vector3D		vEulerAngles;	// Euler angles in body coordinates
    float		fSpeed;			// speed (magnitude of the velocity)

    clf_quaternion	qOrientation;	// orientation in earth coordinates

    clf_vector3D		vForces;		// total force on body
    clf_vector3D		vMoments;		// total moment (torque) on body

    Matrix3x3	mIeInverse;		// inverse of moment of inertia in earth coordinates
};

struct BodyElement {
    float	fMass;
    clf_vector3D	vDCoords;
    clf_vector3D	vCGCoords;
    clf_vector3D	vLocalInertia;
    float	fIncidence;
    float	fDihedral;
    clf_vector3D	vNormal;
    float	fArea;
    float   iFlap;
};

class airplane {
public:
    airplane() {
    }
    void	InitializeAirplane(void);
    void	CalcAirplaneMassProperties(void);	// calcs total mass and inertia
    void	CalcAirplaneLoads(void);			// calcs total forces and moments
    void	StepSimulation(float dt);			// step dt time in the simulation
    clf_vector3D	GetBodyZAxisVector(void);
    clf_vector3D	GetBodyXAxisVector(void);
    Matrix3x3   MakeAngularVelocityMatrix(clf_vector3D u);
    float	LiftCoefficient(float angle, float flaps);
    float	DragCoefficient(float angle, float flaps);
    float	RudderLiftCoefficient(float angle);
    float	RudderDragCoefficient(float angle);
    void	IncThrust(float t = _DTHRUST);
    void	DecThrust(float t = _DTHRUST);
    void	SetThrust(float ratio = 0.25f);
    float GetThrust();
    void	LeftRudder(void);
    void	RightRudder(void);
    void	ZeroRudder(void);
    void	RollLeft(float a = 1.f);
    void	RollRight(float a = 1.f);
    void	PitchUp(float a = 1.f);
    void	PitchDown(float a = 1.f);
    void	ZeroAilerons(void);
    void	ZeroElevators(void);
    void	FlapsDown(float a = 1.f);
    void	ZeroFlaps(void);

    RigidBody	Airplane;		// Rigid body properties of our airplane
    // protected:
        // 0-3 flaps
        // 1-2 wings
        // 4-5 ailerons
        // 6 rudder
        // 7 body
    BodyElement	Element[8];		// Mass, inertia and lifting surface properties of our airplane

    clf_vector3D		Thrust;			// Thrust vector, assumed to act through the plane's CG
    float		ThrustForce;	// Magnitude of the thrust force
    bool		Stalling;		// Flag to let us know if we are in a stalled condition
    bool		Flaps;			// Flag to let us know if the flaps are down
    clf_vector3D      Ae;	            // acceleration
};

#endif / __airplane__
