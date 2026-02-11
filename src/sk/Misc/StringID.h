

#pragma once

#include <sk/Misc/Hashing.h>
#include <sk/Misc/Singleton.h>

#include <atomic>
#include <unordered_map>
#include <vector>

namespace sk
{
    class cStringIDManager : public cSingleton< cStringIDManager >
    {
        friend class cStringID;
    public:
        struct sStringRegistry
        {
            sStringRegistry() = default;
            sStringRegistry( const sStringRegistry& _other );
            sStringRegistry( sStringRegistry&& _other ) noexcept;
            sStringRegistry& operator=( const sStringRegistry& _other );
            sStringRegistry& operator=( sStringRegistry&& _other ) noexcept;
            ~sStringRegistry() = default;
            
            std::atomic_uint64_t ref_count;
            std::string          string;
            str_hash             hash;
            size_t               index;
        };
        
        class cStringRegistry
        {
        public:
            constexpr cStringRegistry() = default;
            explicit  cStringRegistry( sStringRegistry* _registry );
            constexpr cStringRegistry( std::string_view _str );
            constexpr cStringRegistry( const cStringRegistry&  _other );
            constexpr cStringRegistry(       cStringRegistry&& _other ) noexcept;
            constexpr ~cStringRegistry();
            
            constexpr auto operator=( const cStringRegistry&  _other )          -> cStringRegistry&;
            constexpr auto operator=(       cStringRegistry&& _other ) noexcept -> cStringRegistry&;

            [[ nodiscard ]]
            constexpr auto view() const -> const std::string_view&;
            [[ nodiscard ]]
            constexpr auto string() const -> const std::string&;

        private:
            void inc() const;
            void dec() const;
            
            std::string_view m_string_;
            sStringRegistry* m_registry_ = nullptr;
        };

    private:
        auto getRegistry    ( std::string_view _str      ) -> cStringRegistry;
        auto getRegistry    ( const str_hash&  _hash );
        void destroyRegistry( const str_hash&  _registry );

        using index_vec_t    = std::vector< size_t >;
        using registry_vec_t = std::vector< std::unique_ptr< sStringRegistry > >;
        using registry_map_t = std::unordered_map< str_hash, sStringRegistry* >;
        
        index_vec_t    m_available_spots_;
        registry_vec_t m_registries_;
        registry_map_t m_registry_lookup_;
    };

////////////////////////////////////////////////
    
    class cStringID
    {
    public:
        constexpr cStringID() = default;
        constexpr cStringID( const std::string_view& _str );
        template< size_t N >
        constexpr cStringID( const char( &_str )[ N ] )
        : cStringID( std::string_view{ _str, N - 1 } )
        {
        } // cStringID
        
        // Only used to create temporary StringIDs. Don't store anything created by this.
        static constexpr auto FromHash( const str_hash& _hash )
        {
            cStringID id;
            id.m_hash_ = _hash;
            return id;
        }

        constexpr cStringID( const cStringID&  _other )          = default;
        constexpr cStringID(       cStringID&& _other ) noexcept = default;

        constexpr ~cStringID() = default;

        // TODO: Rename to follow more closely to the naming convention
        [[ nodiscard ]]
        constexpr auto hash() const { return m_hash_; }
        [[ nodiscard ]]
        constexpr auto& view() const { return m_registry_.view(); }
        [[ nodiscard ]]
        constexpr auto c_str() const { return m_registry_.view().data(); }
        [[ nodiscard ]]
        auto& string() const { return m_registry_.string(); }
        
        constexpr operator str_hash        () const noexcept { return hash(); }
        constexpr operator std::string_view() const noexcept { return view(); }
        
        constexpr cStringID& operator=( const cStringID& ) = default;
        constexpr cStringID& operator=( cStringID&& _other ) noexcept = default;
        constexpr cStringID& operator=( const std::string_view& _str );
        
        constexpr bool operator== ( const cStringID& _other ) const { return m_hash_ ==  _other.m_hash_; }
        constexpr auto operator<=>( const cStringID& _other ) const { return m_hash_ <=> _other.m_hash_; }

        auto operator<<( std::ostream& _os ) const -> std::ostream&;

    private:
        using registry_t = cStringIDManager::cStringRegistry;
        
        str_hash   m_hash_     = {};
        registry_t m_registry_ = {};
    };
} // sk

template<>
struct std::hash< sk::cStringID >
{
    uint64_t operator()( const sk::cStringID& _value ) const noexcept
    {
        return _value.hash().value();
    }
};

////////////////////////////////////////////////
// I hate forced inline :)

constexpr sk::cStringIDManager::cStringRegistry::cStringRegistry( const std::string_view _str )
: m_string_( _str )
{
}

constexpr sk::cStringIDManager::cStringRegistry::cStringRegistry( const cStringRegistry& _other )
: m_string_( _other.m_string_ )
, m_registry_( _other.m_registry_ )
{
    if not consteval
    {
        inc();
    }
}

constexpr sk::cStringIDManager::cStringRegistry::cStringRegistry( cStringRegistry&& _other ) noexcept
: m_string_( _other.m_string_ )
, m_registry_( _other.m_registry_ )
{
    _other.m_registry_ = nullptr;
}

constexpr sk::cStringIDManager::cStringRegistry::~cStringRegistry()
{
    if not consteval
    {
        dec();
    }
}

constexpr auto sk::cStringIDManager::cStringRegistry::operator=( const cStringRegistry& _other ) -> cStringRegistry&
{
    if( this ==  &_other )
        return *this;

    if not consteval
    {
        dec();
    }
    m_string_   = _other.m_string_;
    m_registry_ = _other.m_registry_;
    if not consteval
    {
        inc();
    }
    
    return *this;
}

constexpr auto sk::cStringIDManager::cStringRegistry::operator=( cStringRegistry&& _other ) noexcept -> cStringRegistry&
{
    if not consteval
    {
        dec();
    }
    
    m_string_   = _other.m_string_;
    m_registry_ = _other.m_registry_;
    _other.m_registry_ = nullptr;
    
    return *this;
}

constexpr auto sk::cStringIDManager::cStringRegistry::view() const -> const std::string_view&
{
    return m_string_;
}

constexpr auto sk::cStringIDManager::cStringRegistry::string() const -> const std::string&
{
    // Due to ease of use, we force this function to be const.
    if( m_registry_ == nullptr )
        *const_cast< cStringRegistry* >( this ) = cStringIDManager::get().getRegistry( m_string_ );
    
    return m_registry_->string;
}

////////////////////////////////////////////////

constexpr sk::cStringID::cStringID( const std::string_view& _str )
: m_hash_( _str )
{
    if consteval
    {
        m_registry_ = _str;
    }
    else
    {
        m_registry_ = cStringIDManager::try_init().getRegistry( _str );
    }
} // cStringID

constexpr sk::cStringID& sk::cStringID::operator=( const std::string_view& _str )
{
    m_hash_ = _str;
    
    if consteval
    {
        m_registry_ = _str;
    }
    else
    {
        m_registry_ = cStringIDManager::try_init().getRegistry( _str );
    }
    
    return *this;
}
