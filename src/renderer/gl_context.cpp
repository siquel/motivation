#include "moti/renderer/gl_context.h"

namespace moti {
    namespace gl {

        GLContext::GLContext() 
            : m_context(nullptr) {
        }

        GLContext::~GLContext() {
            SDL_GL_DeleteContext(m_context);
        }

        void GLContext::create(SDL_Window* _wnd) {

            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            // 32bits
            SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

            m_context = SDL_GL_CreateContext(_wnd);

            glewExperimental = GL_TRUE;
            GLenum err = glewInit();
            glGetError(); // glew 

            SDL_GL_SetSwapInterval(0); // disable vsync
        }
    }
}