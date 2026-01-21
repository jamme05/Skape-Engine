/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include "Assets/Asset.h"

namespace sk::Assets
{
    SK_ASSET_CLASS( Mesh )
    {
        SK_CLASS_BODY( Mesh )
    public:
        virtual void Update( void );
    };
} // sk::Assets

DECLARE_CLASS( sk::Assets::Mesh )
