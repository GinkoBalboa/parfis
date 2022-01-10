#ifndef PARFIS_DATASTRUCT_H
#define PARFIS_DATASTRUCT_H

/**
 * @file datastruct.h
 * @brief Data objects of Parfis.
 */

#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <list>
#include <vector>
#include <math.h>
#include <functional>
#include "global.h"
#include "parfis.h"

namespace parfis {

    /**
     * @addtogroup data
     * @{
     */
    /** 
     * @struct Vec3D
     * @brief Struct that represents a 3d vector with components of type T.
     * @details The struct has three member variables x, y and z of type T.
     */
    template <class T>
    struct Vec3D
    {
        /// Variable x
        T x;
        /// Variable y
        T y;
        /// Variable z
        T z;

        /**
         * @brief Compares if vectors are equal, that is if every component equals.
         */
        friend bool operator==(const Vec3D<T>& lhs, const Vec3D<T>& rhs) {
            return (lhs.x == rhs.x && lhs.y == rhs.y && rhs.z == rhs.z);
        }

        /** 
         * @brief Prints a string representation of the object value
         * @details The string representation is of the form "(x,y,z)" in which a
         * Global::to_string() function is used for the conversion of the type T to string.
         */
        friend std::ostream& operator<<(std::ostream& os, const Vec3D<T>& vec) {
            os << "(" << Global::to_string(vec.x) << ", " <<
                Global::to_string(vec.y) << ", " << Global::to_string(vec.z) << ")";
            return os;
        }

        /** 
         * @brief Returns the vector length in Eucledian metric.
         * @details The length is calculated as @f$ \sqrt{x^2 + y^2 + z^2} @f$
         */
        T len() { return sqrt(x*x + y * y + z * z); }

        /** 
         * @brief Returns the squared vector length in Eucledian metric.
         * @details The length is calculated as @f$ x^2 + y^2 + z^2 @f$
         */
        T lenSqr() { return x * x + y * y + z * z; }
    };
    /**
     * @brief Configuration parameters data
     */
    struct CfgData : public DataBase {
        /// Timestep for the system in seconds
        double timestep;
        /// Geometry size in meters (bounding box of the simulation space)
        Vec3D<double> geometrySize;
        /// Single cell size in meters
        Vec3D<double> cellSize;
        /// Periodic boundary 0-no, 1-yes
        Vec3D<int> periodicBoundary;
        /// Number of cells in every direction
        Vec3D<int> cellCount;
    };
    /** @} data */
}

#endif // PARFIS_DATASTRUCT_H