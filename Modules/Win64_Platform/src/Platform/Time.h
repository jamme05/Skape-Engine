/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <cstdint>

namespace sk::Time
{
    extern const float  &Delta;
    extern const double &Delta_D;
    
    extern const float  &Real;
    extern const double &Real_D;
    
    extern const uint64_t &Frame;
    
    void init();
    void Update();
} // Time
