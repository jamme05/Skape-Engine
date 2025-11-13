#pragma once

#include <unordered_map>

#include <Misc/Singleton.h>
#include <Misc/Hashing.h>

namespace sk::Scene
{
    class cLayer_Manager : public cSingleton< cLayer_Manager >
    {
    public:
        cLayer_Manager();

        void AddLayer   ( const std::string& _name );
        void RemoveLayer( const std::string& _name );

    private:
        std::unordered_map< str_hash, std::string > m_layers_;
    };
} // sk::Scene
