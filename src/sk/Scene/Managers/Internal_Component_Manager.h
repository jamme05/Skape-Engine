

#pragma once

#include <sk/Misc/Singleton.h>
#include <sk/Reflection/Types.h>
#include <sk/Reflection/Type_Hash.h>

#include <unordered_map>

namespace sk::Scene
{
    class cInternal_Component_Manager : public cSingleton< cInternal_Component_Manager >
    {
    public:
        template< class Ty >
        size_t GetComponentIndex();
        size_t GetComponentIndex( type_info_t _type );
        
    private:
        using type_index_map_t = std::unordered_map< type_hash, size_t >;
        
        type_index_map_t m_type_index_map_;
    };

    template< class Ty >
    size_t cInternal_Component_Manager::GetComponentIndex()
    {
        return GetComponentIndex( kTypeInfo< Ty > );
    }
} // sk::Scene::
