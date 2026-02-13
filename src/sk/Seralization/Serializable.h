

#pragma once

#include <sk/Misc/Smart_Ptrs.h>

namespace sk
{
    class cSerializedObject;

    class iSerializable
    {
    public:
        virtual ~iSerializable() = default;
        
        virtual auto Serialize() -> cSerializedObject = 0;
    };
    
    template< class Ty >
    concept serializable = requires( Ty _object )
    {
        { _object.Serialize() } -> std::same_as< cSerializedObject >;
    };
} // sk::
