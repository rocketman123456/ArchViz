/* Copyright (c) 2019-2022, Sascha Willems
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 the "License";
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include "runtime/core/math/math_type.h"
#include "runtime/core/math/matrix/matrix4.h"
#include "runtime/core/math/vector/vector4.h"

#include <array>

namespace ArchViz
{
    class Frustum
    {
    public:
        enum side
        {
            LEFT   = 0,
            RIGHT  = 1,
            TOP    = 2,
            BOTTOM = 3,
            BACK   = 4,
            FRONT  = 5
        };

        std::array<FVector4, 6> planes;

        /**
         * @brief Updates the frustums planes based on a matrix
         * @param matrix The matrix to update the frustum on
         */
        void update(const FMatrix4& matrix);

        /**
         * @brief Checks if a sphere is inside the Frustum
         * @param pos The center of the sphere
         * @param radius The radius of the sphere
         */
        bool checkSphere(const FVector3& pos, float radius);
    };
} // namespace ArchViz
