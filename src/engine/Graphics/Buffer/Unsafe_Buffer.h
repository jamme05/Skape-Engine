/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

// Will be overriden in Module.

class iUnsafe_Buffer
{
protected:
    virtual ~iUnsafe_Buffer( void ) = 0;

    virtual void*  Get    ( void ) = 0;
    [[ nodiscard ]]
    virtual size_t GetSize( void ) const = 0;
    virtual void   Resize ( size_t _byte_size ) = 0;

    virtual void  Lock    ( void ) = 0;
    virtual void  Unlock  ( void ) = 0;
    [[ nodiscard ]]
    virtual bool  IsLocked( void ) const = 0;
};
