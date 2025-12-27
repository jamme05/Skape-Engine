
#include "StringID.h"

#include <queue>

////////////////////////////////////////////////

sk::cStringIDManager::sStringRegistry::sStringRegistry( const sStringRegistry& _other )
{
    *this = _other;
}

sk::cStringIDManager::sStringRegistry::sStringRegistry( sStringRegistry&& _other ) noexcept
{
    *this = std::move( _other );
}

sk::cStringIDManager::sStringRegistry& sk::cStringIDManager::sStringRegistry::operator=( const sStringRegistry& _other )
{
    if( this == &_other )
        return *this;
    
    ref_count.store( _other.ref_count );
    string = _other.string;
    hash   = _other.hash;
    index  = _other.index;
    
    return *this;
}

sk::cStringIDManager::sStringRegistry& sk::cStringIDManager::sStringRegistry::operator=(
    sStringRegistry&& _other ) noexcept
{
    ref_count.store( _other.ref_count );
    _other.ref_count.store( 0 );
    string = std::move( _other.string );
    hash   = _other.hash;
    index  = _other.index;
    
    return *this;
}

sk::cStringIDManager::cStringRegistry::cStringRegistry( sStringRegistry* _registry )
: m_registry_( _registry )
{
    if( _registry != nullptr )
        m_string_ = _registry->string;
    inc();
}

void sk::cStringIDManager::cStringRegistry::inc() const
{
    if( m_registry_ == nullptr )
        return;
    
    ++m_registry_->ref_count;
}

void sk::cStringIDManager::cStringRegistry::dec() const
{
    if( m_registry_ == nullptr )
        return;

    if( --m_registry_->ref_count == 0 )
        cStringIDManager::get().destroyRegistry( m_registry_->hash );
}

////////////////////////////////////////////////

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

void sk::cStringIDManager::destroyRegistry( const str_hash& _registry )
{
    if( const auto itr = m_registry_lookup_.find( _registry ); itr != m_registry_lookup_.end() )
    {
        auto& to_destroy = *itr->second;
        m_registry_lookup_.erase( itr );

        // If it's in the back then we can remove it without adding it to the available list.
        if( to_destroy.index == m_registries_.size() - 1 )
            m_registries_.pop_back();
        else
        {
            m_available_spots_.emplace_back( to_destroy.index );
            std::ranges::sort( m_available_spots_, std::ranges::greater{} );
            
            to_destroy.ref_count.store( 0, std::memory_order_relaxed );
            to_destroy.string = {};
            to_destroy.hash  = {};
            // registry.index = 0;
        }

        // We can ignore the pruning if the spot that's available isn't in the back.
        if( m_available_spots_.front() != m_registries_.size() - 1 )
            return;
        
        // We will be going from back to front and purge everything that has been "destroyed"
        // The registry will always be larger than the available spots so this is fine.
        uint32_t prune = 0;
        for( size_t i = 0; i < m_available_spots_.size(); ++i )
        {
            if( m_registries_[ i ].hash != str_hash::kEmpty )
                break;
            
            prune = static_cast< uint32_t >( i );
        }
        
        m_available_spots_.erase( m_available_spots_.begin(), m_available_spots_.begin() + prune );
        m_registries_.erase( m_registries_.begin() + prune, m_registries_.end() );
    }
}

////////////////////////////////////////////////

auto sk::cStringID::operator<<( std::ostream& _os ) const -> std::ostream&
{
    return _os << view();
} // operator<<