

#pragma once

#include <Misc/Smart_Ptrs.h>

namespace sk
{
    class cSerializedObject;

    class cSerializable
    {
    public:
        virtual ~cSerializable() = default;
        
        virtual auto Serialize() -> cShared_ptr< cSerializedObject > = 0; 
    };
    
    template< class Ty >
    concept serializable = requires( Ty _object )
    {
        { _object.Serialize() } -> std::same_as< cShared_ptr< cSerializedObject > >;
    };
} // sk::
