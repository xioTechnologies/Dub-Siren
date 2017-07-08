/**
 * @file MathHelpers.h
 * @author Seb Madgwick
 * @brief Math helper macros.
 */

#ifndef MATH_HELPERS_H
#define MATH_HELPERS_H

//------------------------------------------------------------------------------
// Includes

#include <math.h> // floorf

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Returns the smaller of a and b.
 */
#define MIN(a, b) ((a) < (typeof(a))(b) ? (a) : (typeof(a))(b))

/**
 * @brief Returns the greater of a and b.
 */
#define MAX(a, b) ((a) > (typeof(a))(b) ? (a) : (typeof(a))(b))

/**
 * @brief Clamps value to a specified range.
 */
#define CLAMP(value, minValue, maxValue) (MIN(MAX(value, minValue), maxValue))

/**
 * @brief Rounds float to nearest integer.
 */
#define ROUND(value) floorf((float)(value) + 0.5f)

/**
 * @brief Maps float x from range x1 to x2 to range y1 to y2.
 */
#define MAP(x, x1, x2, y1, y2) ((((float)(x)) - ((float)(x1))) / (((float)(x2)) - ((float)(x1))) * (((float)(y2)) - ((float)(y1))) + ((float)(y1)))

#endif

//------------------------------------------------------------------------------
// End of file
