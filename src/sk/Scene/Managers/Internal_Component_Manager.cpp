

#include "Internal_Component_Manager.h"

size_t sk::Scene::cInternal_Component_Manager::GetComponentIndex( const type_info_t _type )
{
    if( const auto itr = m_type_index_map_.find( _type->hash ); itr != m_type_index_map_.end() )
        return itr->second;
    
    auto index = m_type_index_map_.size();
    
    m_type_index_map_.emplace( _type, index );
    
    return index;
}
