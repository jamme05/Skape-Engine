
#include "StringID.h"

sk::cStringIDManager::cStringRegistry::cStringRegistry( sStringRegistry* _registry )
: m_registry_( _registry )
{
    inc();
}

sk::cStringIDManager::cStringRegistry::cStringRegistry( const cStringRegistry& _other )
: m_registry_( _other.m_registry_ )
{
    inc();
}

sk::cStringIDManager::cStringRegistry::cStringRegistry( cStringRegistry&& _other ) noexcept
: m_registry_( _other.m_registry_ )
{
}

sk::cStringIDManager::cStringRegistry::~cStringRegistry()
{
    dec();
}

auto sk::cStringIDManager::cStringRegistry::operator=( const cStringRegistry& _other )->cStringRegistry&
{
    if( this ==  &_other )
        return *this;

    dec();
    m_registry_ = _other.m_registry_;
    inc();
    
    return *this;
}

auto sk::cStringIDManager::cStringRegistry::operator=( cStringRegistry&& _other ) noexcept->cStringRegistry&
{
    dec();
    m_registry_ = _other.m_registry_;
    
    return *this;
}

void sk::cStringIDManager::cStringRegistry::inc() const
{
    ++m_registry_->ref_count;
}

void sk::cStringIDManager::cStringRegistry::dec() const
{
    if( --m_registry_->ref_count == 0 )
        cStringIDManager::get().destroyRegistry( m_registry_->hash );
}

auto sk::cStringIDManager::getRegistry( const std::string_view _str ) -> cStringRegistry
{
    const str_hash hash = _str;
    if( const auto itr = m_registry_lookup_.find( hash ); itr == m_registry_lookup_.end() )
    {
        size_t spot;
        if( m_available_spots_.empty() )
        {
            spot = m_registries_.size();
            // More efficient than resize.
            m_registries_.emplace_back();
        }
        else
        {
            spot = m_available_spots_.back();
            m_available_spots_.pop_back();
        }

        auto& registry  = m_registries_[ spot ];
        registry.ref_count.store( 0, std::memory_order_relaxed );
        registry.hash   = hash;
        registry.string = std::string_view( _str.data(), _str.size() );
        registry.index  = spot;

        m_registry_lookup_[ hash ] = &registry;
        
        return cStringRegistry{ &registry };
    }
    else
        return cStringRegistry{ itr->second };
}

auto sk::cStringIDManager::destroyRegistry( const str_hash& _registry )->void
{
    if( const auto itr = m_registry_lookup_.find( _registry ); itr != m_registry_lookup_.end() )
    {
        auto& registry = *itr->second;
        m_registry_lookup_.erase( _registry );
        
        registry.ref_count.store( 0, std::memory_order_relaxed );
        registry.hash  = {};
        registry.string = {};

        if( registry.index == m_registries_.size() )
            // Resize if last in list. This will not take into consideration the previous element also being removed.
            m_registries_.pop_back();
        else
        {
            m_available_spots_.emplace_back( registry.index );
            registry.index = 0;
        }
    }
}
