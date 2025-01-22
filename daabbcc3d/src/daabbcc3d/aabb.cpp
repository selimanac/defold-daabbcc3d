// SPDX-FileCopyrightText: 2023 Erin Catto
// SPDX-License-Identifier: MIT

#include "daabbcc3d/aabb.h"

namespace daabbcc3d
{
    bool b2IsValidAABB(b2AABB a)
    {
        b2Vec3 d = b2Sub(a.upperBound, a.lowerBound);
        bool   valid = d.x >= 0.0f && d.y >= 0.0f && d.z >= 0.0f;
        valid = valid && b2IsValidVec3(a.lowerBound) && b2IsValidVec3(a.upperBound);
        return valid;
    }

    // From Real-time Collision Detection, p179.
    b2CastOutput b2AABB_RayCast(b2AABB a, b2Vec3 p1, b2Vec3 p2)
    {
        // Radius not handled
        b2CastOutput output = { 0 };

        float        tmin = -FLT_MAX;
        float        tmax = FLT_MAX;

        b2Vec3       p = p1;
        b2Vec3       d = b2Sub(p2, p1);
        b2Vec3       absD = b2Abs(d);

        b2Vec3       normal = b2Vec3_zero;

        // x-coordinate
        if (absD.x < FLT_EPSILON)
        {
            // parallel
            if (p.x < a.lowerBound.x || a.upperBound.x < p.x)
            {
                return output;
            }
        }
        else
        {
            float inv_d = 1.0f / d.x;
            float t1 = (a.lowerBound.x - p.x) * inv_d;
            float t2 = (a.upperBound.x - p.x) * inv_d;

            // Sign of the normal vector.
            float s = -1.0f;

            if (t1 > t2)
            {
                float tmp = t1;
                t1 = t2;
                t2 = tmp;
                s = 1.0f;
            }

            // Push the min up
            if (t1 > tmin)
            {
                normal.y = 0.0f;
                normal.x = s;
                tmin = t1;
            }

            // Pull the max down
            tmax = b2MinFloat(tmax, t2);

            if (tmin > tmax)
            {
                return output;
            }
        }

        // y-coordinate
        if (absD.y < FLT_EPSILON)
        {
            // parallel
            if (p.y < a.lowerBound.y || a.upperBound.y < p.y)
            {
                return output;
            }
        }
        else
        {
            float inv_d = 1.0f / d.y;
            float t1 = (a.lowerBound.y - p.y) * inv_d;
            float t2 = (a.upperBound.y - p.y) * inv_d;

            // Sign of the normal vector.
            float s = -1.0f;

            if (t1 > t2)
            {
                float tmp = t1;
                t1 = t2;
                t2 = tmp;
                s = 1.0f;
            }

            // Push the min up
            if (t1 > tmin)
            {
                normal.x = 0.0f;
                normal.y = s;
                tmin = t1;
            }

            // Pull the max down
            tmax = b2MinFloat(tmax, t2);

            if (tmin > tmax)
            {
                return output;
            }
        }

        // z-coordinate
        if (absD.z < FLT_EPSILON)
        {
            // parallel
            if (p.z < a.lowerBound.z || a.upperBound.z < p.z)
            {
                return output;
            }
        }
        else
        {
            float inv_d = 1.0f / d.z;
            float t1 = (a.lowerBound.z - p.z) * inv_d;
            float t2 = (a.upperBound.z - p.z) * inv_d;

            // Sign of the normal vector.
            float s = -1.0f;

            if (t1 > t2)
            {
                float tmp = t1;
                t1 = t2;
                t2 = tmp;
                s = 1.0f;
            }

            // Push the min up
            if (t1 > tmin)
            {
                normal.x = 0.0f;
                normal.y = 0.0f;
                normal.z = s;
                tmin = t1;
            }

            // Pull the max down
            tmax = b2MinFloat(tmax, t2);

            if (tmin > tmax)
            {
                return output;
            }
        }

        // Does the ray start inside the box?
        // Does the ray intersect beyond the max fraction?
        if (tmin < 0.0f || 1.0f < tmin)
        {
            return output;
        }

        // Intersection.
        output.fraction = tmin;
        output.normal = normal;
        output.point = b2Lerp(p1, p2, tmin);
        output.hit = true;
        return output;
    }
} // namespace daabbcc3d
