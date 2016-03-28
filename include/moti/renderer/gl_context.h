#pragma once

#include "moti/moti.h"
#if MOTI_PLATFORM_WINDOWS
#include <assert.h>
#include "moti/renderer/renderer_gl.h"
#include <gl/glew.h>
#include <gl/wglew.h>
#include <SDL/SDL.h>

namespace moti {
    namespace gl {

        struct GLContext {
            SDL_GLContext m_context;

            GLContext();
            ~GLContext();
            void create(SDL_Window* _wnd);
        };
    }
}

#endif