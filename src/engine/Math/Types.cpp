/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Types.h"

REGISTER_TYPE( sk::cVector2f, "Vector 2 ( float )" )
REGISTER_TYPE( sk::cVector2d, "Vector 2 ( double )" )
REGISTER_TYPE( sk::cVector2i32, "Vector 2 ( int )" )
REGISTER_TYPE( sk::cVector2u32, "Vector 2 ( unsigned int )" )

REGISTER_TYPE( sk::cVector3f, "Vector 3 ( float )" )
REGISTER_TYPE( sk::cVector3d, "Vector 3 ( double )" )
REGISTER_TYPE( sk::cVector3i32, "Vector 3 ( int )" )
REGISTER_TYPE( sk::cVector3u32, "Vector 3 ( unsigned int )" )

// TODO: Create a custom color class.
// REGISTER_TYPE( sk::cColor,    "Vector 3 ( float )" )
REGISTER_TYPE( sk::cVector4f, "Vector 4 ( float )" )
REGISTER_TYPE( sk::cVector4d, "Vector 4 ( double )" )
REGISTER_TYPE( sk::cVector4i, "Vector 4 ( int )" )
REGISTER_TYPE( sk::cVector4u, "Vector 4 ( unsigned int )" )

REGISTER_TYPE( sk::cMatrix4x4f, "Matrix 4x4 ( float )" )
REGISTER_TYPE( sk::cMatrix4x4d, "Matrix 4x4 ( double )" )