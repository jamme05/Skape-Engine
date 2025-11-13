#pragma once

#include <thread>

namespace sk
{
    class cPartialAsset;
}

namespace sk
{
    class Asset_Loader
    {
    public:
        void AddTask( const std::shared_ptr< cPartialAsset >& _asset );

    private:
    };

    struct sAsset_Loader
    {
        Asset_Loader* loader;
        std::thread   thread;
    };
} // sk::
