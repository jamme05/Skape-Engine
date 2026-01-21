/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#define REGISTER_ENUM( Type ) \

/**
 *
 * @param Type ENUM/ENUMCLASS ( Name, [ Size ] )
 * @param ...  Value/E( Name, [ value ], [ Display Name ] )
 *
**/
#define MAKE_ENUM( Type, ... ) \
MAKE_UNREFLECTED_ENUM( Type, __VA_ARGS__ )
    REGISTER_ENUM( Type )
