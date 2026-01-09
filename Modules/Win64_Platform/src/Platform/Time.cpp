
#include "Time.h"

#include <chrono>

namespace sk::Time
{
    namespace
    {
        std::chrono::steady_clock::time_point start;
        std::chrono::steady_clock::time_point prev;
        
        double real  = 0.0f;
        double delta = 0.0f;
        
        float real_f  = 0.0f;
        float delta_f = 0.0f;
        
        uint64_t frame = 0;
    } // ::
    
    const float  &Delta = delta_f;
    const double &Delta_D = delta;
    
    const float  &Real = real_f;
    const double &Real_D = real;
    
    const uint64_t &Frame = frame;
    
    // TODO: Maybe hide the init?
    void init()
    {
        prev = start = std::chrono::steady_clock::now();
    }
    
    void Update()
    {
        using duration_t = std::chrono::duration< double >;
        const auto now = std::chrono::steady_clock::now();
        const duration_t elapsed_total = now - start;
        const duration_t elapsed_frame = now - prev;
        
        real  = elapsed_total.count();
        delta = elapsed_frame.count();
        
        real_f  = static_cast< float >( real );
        delta_f = static_cast< float >( delta );
        
        prev = now;
        
        frame++;
    }
} // sk::Time::
