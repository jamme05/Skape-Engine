/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Misc/UUID.h>

#include <random>

#include <Misc/Singleton.h>

namespace sk
{
    class cUUID_Helper : public cSingleton< cUUID_Helper >
    {
        std::mt19937_64 m_random_generator_;
    public:
        cUUID_Helper();

        cUUID GenerateRandomUUID();
    };
} // sk::
