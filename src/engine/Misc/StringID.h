

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
            cStringRegistry() = default;
            explicit cStringRegistry( sStringRegistry* _registry );
            cStringRegistry( const cStringRegistry&  _other );
            cStringRegistry(       cStringRegistry&& _other ) noexcept;
            ~cStringRegistry();
            
            auto operator=( const cStringRegistry&  _other )          -> cStringRegistry&;
            auto operator=(       cStringRegistry&& _other ) noexcept -> cStringRegistry&;

            [[ nodiscard ]] auto string() const -> const std::string&; 

        private:
            void inc() const;
            void dec() const;
            
            sStringRegistry* m_registry_ = nullptr;
        };

    private:
        auto getRegistry    ( std::string_view _str      ) -> cStringRegistry;
        void destroyRegistry( const str_hash&  _registry );

        using index_vec_t    = std::vector< size_t >;
        using registry_vec_t = std::vector< sStringRegistry >;
        using registry_map_t = std::unordered_map< str_hash, sStringRegistry* >;
        
        index_vec_t    m_available_spots_;
        registry_vec_t m_registries_;
        registry_map_t m_registry_lookup_;
    };

////////////////////////////////////////////////
    
    class cStringID
    {
    public:
        cStringID() = default;
        cStringID( std::string_view _str );
        template< size_t N >
        cStringID( const char( &_str )[ N ] )
        : cStringID( std::string_view{ _str, N } )
        {
        } // cStringID

        constexpr cStringID( const cStringID&  _other )          = default;
        constexpr cStringID(       cStringID&& _other ) noexcept = default;

        ~cStringID() = default;

        // TODO: Rename to follow more closely to the naming convention
        [[ nodiscard ]] auto  hash  () const { return m_hash_; }
        [[ nodiscard ]] auto& string() const { return m_registry_.string(); }
        
        cStringID& operator=( const cStringID& ) = default;
        cStringID& operator=( cStringID&& _other ) noexcept = default;
        cStringID& operator=( const std::string_view& _str );
        
        bool operator== ( const cStringID& _other ) const { return m_hash_ ==  _other.m_hash_; }
        bool operator!= ( const cStringID& _other ) const { return m_hash_ !=  _other.m_hash_; }
        auto operator<=>( const cStringID& _other ) const { return m_hash_ <=> _other.m_hash_; }

        auto operator<<( std::ostream& _os ) const -> std::ostream&;

    private:
        using registry_t = cStringIDManager::cStringRegistry;
        
        str_hash   m_hash_     = {};
        registry_t m_registry_ = {};
        
    };
} // sk
