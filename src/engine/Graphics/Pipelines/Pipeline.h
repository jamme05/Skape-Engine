#pragma once
#include <functional>
#include <memory>
#include <string>

namespace sk::Object::Components
{
    class cCameraComponent;
} // sk::Object::Components

namespace sk::Graphics::Passes
{
    class iPass;
} // sk::Graphics

namespace sk::Graphics
{
    class cPipeline
    {
    public:
        using pass_vec_t = std::vector< std::unique_ptr< Passes::iPass > >;

        virtual void Initialize();
        virtual void Begin     ();
        virtual void End       ();
        virtual void Destroy   ();

        template< class Ty, class... Args >
        Ty&  AddPass( Args&&... _args );
        void AddPass( std::unique_ptr< Passes::iPass >&& _pass );
        [[ nodiscard ]]
        auto GetPass( size_t _index ) const -> Passes::iPass&;

    private:
        pass_vec_t m_passes_      = {};
        bool       m_initialized_ = false;
    };

    template< class Ty, class ... Args >
    Ty& cPipeline::AddPass( Args&&... _args )
    {
        auto pass = std::make_unique< Passes::iPass >( std::forward< Args >( _args )... );
        auto& pass_ref = *pass;
        AddPass( pass );
            
        return pass_ref;
    }
} // sk::Graphics
