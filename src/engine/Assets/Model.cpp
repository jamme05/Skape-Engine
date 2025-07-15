/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Model.h"

#include "Reflection/Interface.h"

namespace sk::Assets
{
    cModel::cModel( const std::string& _name )
    : cAsset( _name )
    {
    } // cModel

    void cModel::Save()
    {
        // Does nothing for now.
    }
} // sk::Assets
