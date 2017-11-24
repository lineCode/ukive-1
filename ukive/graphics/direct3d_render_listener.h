#ifndef UKIVE_GRAPHICS_DIRECT3D_RENDER_LISTENER_H_
#define UKIVE_GRAPHICS_DIRECT3D_RENDER_LISTENER_H_


namespace ukive {

    class Direct3DRenderListener
    {
    public:
        virtual void onDirect3DClear() = 0;
        virtual void onDirect3DRender() = 0;
    };

}

#endif  // UKIVE_GRAPHICS_DIRECT3D_RENDER_LISTENER_H_