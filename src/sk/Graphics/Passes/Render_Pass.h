

#pragma once

// TODO: Rename this file to "Pass"

#include <string>

namespace sk::Graphics
{
    class cPipeline;
} // sk::Graphics

namespace sk::Graphics::Passes
{
    class iPass
    {
        friend class sk::Graphics::cPipeline;
        cPipeline*  m_current_pipeline_;
        
    protected:
        [[ nodiscard ]] auto getPipeline() const -> cPipeline&;
        
    public:
        iPass() = default;
        virtual ~iPass() = default;
        iPass( const iPass& ) = delete;
        iPass( iPass&& ) = delete;
        iPass& operator=( const iPass& ) = delete;
        iPass& operator=( iPass&& ) = delete;
        
        std::string name;
        
        virtual void Init   () = 0;
        virtual bool Begin  () = 0;
        virtual void End    () = 0;
        virtual void Destroy() = 0;
    };
} // sk::Graphics

