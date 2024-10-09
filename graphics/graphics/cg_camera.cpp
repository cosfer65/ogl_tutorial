#include "pch.h"
#include "cg_camera.h"
#include "cg_matrix.h"

namespace atlas {
    void cg_camera::rotate_locX(float fAngle)
    {
        mat4 mRotation;
        vec3 vCross;

        vCross = cross(vForward, vUp);
        mRotation = rotation_matrix(fAngle, vCross.x, vCross.y, vCross.z);

        vec3 vNewVect;
        // Inline 3x3 matrix multiply for rotation only
        vNewVect.x = mRotation[0] * vForward.x + mRotation[4] * vForward.y + mRotation[8] * vForward.z;
        vNewVect.y = mRotation[1] * vForward.x + mRotation[5] * vForward.y + mRotation[9] * vForward.z;
        vNewVect.z = mRotation[2] * vForward.x + mRotation[6] * vForward.y + mRotation[10] * vForward.z;
        memcpy(vForward, vNewVect, sizeof(float) * 3);

        // Update pointing up vector
        vNewVect.x = mRotation[0] * vUp.x + mRotation[4] * vUp.y + mRotation[8] * vUp.z;
        vNewVect.y = mRotation[1] * vUp.x + mRotation[5] * vUp.y + mRotation[9] * vUp.z;
        vNewVect.z = mRotation[2] * vUp.x + mRotation[6] * vUp.y + mRotation[10] * vUp.z;
        memcpy(vUp, vNewVect, sizeof(float) * 3);
    }

    void cg_camera::rotate_locY(float angle)
    {
        mat4 mRotation;

        // mRotation.RotationMatrix(angle, 0, 1, 0);
        // mRotation.RotationMatrix(angle, vUp.x, vUp.y, vUp.z);
        mRotation = rotation_matrix(angle, vUp.x, vUp.y, vUp.z);

        vForward = rotate_vector(mRotation, vForward);
    }

    void cg_camera::rotate_locZ(float fAngle)
    {
        mat4 mRotation;

        // Only the up vector needs to be rotated
        mRotation = rotation_matrix(-fAngle, vForward.x, vForward.y, vForward.z);

        vec3 vNewVect;
        vNewVect.x = mRotation[0] * vUp.x + mRotation[4] * vUp.y + mRotation[8] * vUp.z;
        vNewVect.y = mRotation[1] * vUp.x + mRotation[5] * vUp.y + mRotation[9] * vUp.z;
        vNewVect.z = mRotation[2] * vUp.x + mRotation[6] * vUp.y + mRotation[10] * vUp.z;
        memcpy(vUp, vNewVect, sizeof(float) * 3);
    }

    void cg_camera::setup(const vec3& pos, const vec3& lookat, const vec3& up)
    {
        vLocation = pos;
        vForward = lookat - pos;
        vUp = up;
        vForward.normalize();

        vec3 right = cross(vForward, vUp);
        vUp = cross(right, vForward);
        vUp.normalize();
    }
}