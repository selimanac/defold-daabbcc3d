// SPDX-FileCopyrightText: 2023 Erin Catto
// SPDX-License-Identifier: MIT

#include "daabbcc3d/math_functions.h"

namespace daabbcc3d
{

    bool b2IsValidFloat(float a)
    {
        if (isnan(a))
        {
            return false;
        }

        if (isinf(a))
        {
            return false;
        }

        return true;
    }

    bool b2IsValidVec3(b2Vec3 v)
    {
        if (isnan(v.x) || isnan(v.y) || isnan(v.z))
        {
            return false;
        }

        if (isnan(v.x) || isnan(v.y) || isnan(v.z))
        {
            return false;
        }

        return true;
    }
} // namespace daabbcc3d
