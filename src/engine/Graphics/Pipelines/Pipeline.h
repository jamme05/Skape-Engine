#pragma once
#include <functional>
#include <memory>
#include <string>

#include <Graphics/Passes/Render_Pass.h>

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
    class cPipeline
    {
    public:
        explicit cPipeline( Platform::iWindow* _window );
        virtual ~cPipeline() = default;
        using pass_vec_t = std::vector< std::unique_ptr< Passes::iPass > >;

        virtual void Initialize();
        virtual void Execute   ();
        virtual void Destroy   ();
        
        auto GetWindow() const { return m_window_; }

        template< class Ty, class... Args >
        Ty&  AddPass( Args&&... _args );
        void AddPass( std::unique_ptr< Passes::iPass >&& _pass );
        [[ nodiscard ]]
        auto GetPass( size_t _index ) const -> Passes::iPass&;

    protected:
        Platform::iWindow* m_window_;
        
    private:
        bool       m_initialized_ = false;
        pass_vec_t m_passes_      = {};
    };

    template< class Ty, class ... Args >
    Ty& cPipeline::AddPass( Args&&... _args )
    {
        auto pass = std::make_unique< Ty >( std::forward< Args >( _args )... );
        auto& pass_ref = *pass;
        AddPass( std::move( pass ) );
            
        return pass_ref;
    }
} // sk::Graphics
