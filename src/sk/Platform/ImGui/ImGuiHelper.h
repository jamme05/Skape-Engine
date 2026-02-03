
namespace sk::Platform
{
    class iWindow;
} // sk::Platform::

namespace sk::Gui
{
    extern void InitImGui( const Platform::iWindow* _target_window );
    extern void ImGuiUpdateWindows();
    extern void ImGuiNewFrame();
    extern void ImGuiRender();
    extern void ImGuiShutdown();
} // sk::ImGui