/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Misc/DerivedSingleton.h>

// Won't the graphics context have to live within each window?
// Figure out a way to either pick a context depending on the active window,
// or move part of the renderer singleton to the window. Like Window.GetRenderer()


namespace sk::Graphics
{
    class cPipeline;

    // Will be overridden by Module
    class cRenderer : public cBaseSingleton< cRenderer >
    {
    public:
        ~cRenderer() override;
        [[ nodiscard ]]
        auto GetPipeline() const -> cPipeline*;
        void SetPipeline( cPipeline* _pipeline );
    
        virtual void Update() = 0;
    private:
        cPipeline* m_active_pipeline_ = nullptr;
    };
    
    extern void InitRenderer();
} // sk::Graphics
