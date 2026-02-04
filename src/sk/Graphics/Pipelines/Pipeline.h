

#pragma once

#include <sk/Graphics/Passes/Render_Pass.h>
#include <sk/Graphics/Rendering/Render_Context.h>

#include <memory>
#include <vector>


namespace sk::Platform
{
    class iWindow;
} // sk::Platform::

namespace sk::Object::Components
{
    class cCameraComponent;
} // sk::Object::Components

namespace sk::Graphics
{
    class iSurface;

    class cPipeline
    {
    public:
        explicit cPipeline( iSurface* _surface );
        virtual ~cPipeline() = default;
        using pass_vec_t = std::vector< std::unique_ptr< Passes::iPass > >;

        virtual void Initialize();
        virtual void Execute   ();
        virtual void Destroy   ();
        
        [[ nodiscard ]] auto GetSurface() const { return m_surface_; }

        template< class Ty, class... Args >
        requires std::constructible_from< Ty, Args... >
        Ty&  AddPass( Args&&... _args );
        void AddPass( std::unique_ptr< Passes::iPass >&& _pass );
        [[ nodiscard ]]
        auto GetPass( size_t _index ) const -> Passes::iPass&;

    protected:
        iSurface* m_surface_;

    private:
        bool       m_initialized_ = false;
        pass_vec_t m_passes_      = {};
    };

    template< class Ty, class ... Args >
    requires std::constructible_from< Ty, Args... >
    Ty& cPipeline::AddPass( Args&&... _args )
    {
        auto pass = std::make_unique< Ty >( std::forward< Args >( _args )... );
        auto& pass_ref = *pass;
        AddPass( std::move( pass ) );
            
        return pass_ref;
    }
} // sk::Graphics
