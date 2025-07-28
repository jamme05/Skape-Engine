/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Model.h"

#include <Assets/Mesh.h>

REGISTER_CLASS( sk::Assets::Model )

namespace sk::Assets
{
    cModel::cModel( const std::string& _name )
    : cAsset( _name )
    {
    } // cModel
} // sk::Assets
