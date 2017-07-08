/**
 * @file CascadeFilter.h
 * @author Seb Madgwick
 * @brief Cascaded first-order low-pass or high-pass filter.
 */

#ifndef CASCADE_FILTER_H
#define CASCADE_FILTER_H

//------------------------------------------------------------------------------
// Includes

#include "FirstOrderFilter.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Maximum number of cascaded filters.
 */
#define MAXIMUM_NUMBER_OF_CASCADED_FILTERS (3)

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Cascade filter structure.
 */
typedef struct {
    int numberOfFilters;
    FirstOrderFilter firstOrderFilter[MAXIMUM_NUMBER_OF_CASCADED_FILTERS];
} CascadeFilter;

//------------------------------------------------------------------------------
// Function prototypes

void CascadeFilterSetCornerFrequency(CascadeFilter * const cascadeFilter, const float cornerFrequency, const float sampleFrequency, const bool isHighPass, const int numberOfFilters);
float CascadeFilterUpdate(CascadeFilter * const cascadeFilter, const float input);

#endif

//------------------------------------------------------------------------------
// End of file
