

#pragma once

#include <unordered_map>

#include <Misc/Hashing.h>
#include <Misc/Singleton.h>

namespace sk
{
    class cStringIDManager : public cSingleton< cStringIDManager >
    {
        friend class cStringID;
    public:
        struct sStringRegistry
        {
            std::atomic_uint64_t ref_count;
            str_hash             hash;
            std::string          string;
            size_t               index;
        };
        
        class cStringRegistry
        {
        public:
            explicit cStringRegistry( sStringRegistry* _registry );
            cStringRegistry( const cStringRegistry& _other );
            cStringRegistry( cStringRegistry&& _other ) noexcept;
            ~cStringRegistry();
            
            auto operator=( const cStringRegistry&  _other ) -> cStringRegistry&;
            auto operator=( cStringRegistry&& _other ) noexcept -> cStringRegistry&;

            auto& string() const { return m_registry_->string; }

        private:
            void inc() const;
            void dec() const;
            
            sStringRegistry* m_registry_;
        };

    private:
        auto getRegistry    ( std::string_view _str ) -> cStringRegistry;
        auto destroyRegistry( const str_hash& _registry ) -> void;

        std::vector< size_t >                            m_available_spots_;
        std::vector< sStringRegistry >                   m_registries_;
        std::unordered_map< str_hash, sStringRegistry* > m_registry_lookup_;
    };
    
    class cStringID
    {
    public:
        explicit cStringID( const std::string_view _str )
        : m_hash_( _str )
        , m_registry_( cStringIDManager::try_init().getRegistry( _str ) )
        {
        } // cStringID

        constexpr cStringID( const cStringID& _other ) = default;
        constexpr cStringID( cStringID&& _other ) noexcept = default;

        ~cStringID( void ) = default;
        
        auto  value () const { return m_hash_; }
        auto& string() const { return m_registry_.string(); }
        
        constexpr cStringID& operator=( const cStringID& ) = default;
        constexpr cStringID& operator=( cStringID&& _other ) noexcept = default;
        constexpr bool operator==( const cStringID& _other ) const { return m_hash_ == _other.m_hash_; }
        constexpr bool operator!=( const cStringID& _other ) const { return m_hash_ != _other.m_hash_; }
        constexpr auto operator<=>( const cStringID& _other ) const { return m_hash_ <=> _other.m_hash_; }

    private:
        str_hash m_hash_;
        cStringIDManager::cStringRegistry m_registry_;
        
    };
} // sk
