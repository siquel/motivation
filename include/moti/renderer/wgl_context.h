#pragma once

#include "moti/moti.h"
#if MOTI_PLATFORM_WINDOWS
#include <assert.h>
#include "moti/renderer/renderer_gl.h"
#include <gl/wglew.h>
#include <gl/glew.h>

namespace moti {
    namespace gl {
        struct SwapChainGL {
            HWND m_hwnd;
            HDC m_hdc;
            HGLRC m_context;
            SwapChainGL(void* _hwnd)
                : m_hwnd((HWND)_hwnd) {
                m_hdc = GetDC(m_hwnd);
            }

            ~SwapChainGL() {
                wglMakeCurrent(NULL, NULL);
                wglDeleteContext(m_context);
                ReleaseDC(m_hwnd, m_hdc);
            }

            void makeCurrent() {
                wglMakeCurrent(m_hdc, m_context);
                GLenum err = glGetError();
            }

            void swapBuffers() {
                SwapBuffers(m_hdc);
            }
        };

        struct GLContext {
            HDC m_hdc;
            HGLRC m_context;
            SwapChainGL* m_current;
            GLContext()
                : m_hdc(NULL), m_context(NULL), m_current(nullptr) {

            }
            void create();
        };

        static HGLRC createContext(HDC _hdc) {
            PIXELFORMATDESCRIPTOR pfd;
            ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
            pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
            pfd.nVersion = 1;
            pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
            pfd.iPixelType = PFD_TYPE_RGBA;
            pfd.cColorBits = 32;
            pfd.cAlphaBits = 8;
            pfd.cDepthBits = 24;
            pfd.cStencilBits = 8;
            pfd.iLayerType = PFD_MAIN_PLANE;

            int pxlFormat = ChoosePixelFormat(_hdc, &pfd);
            assert(pxlFormat != 0);

            DescribePixelFormat(_hdc, pxlFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

            int result = SetPixelFormat(_hdc, pxlFormat, &pfd);
            assert(result != 0);
            HGLRC ctx = wglCreateContext(_hdc);
            assert(ctx != NULL);
            result = wglMakeCurrent(_hdc, ctx);
            assert(result != 0);
            
            return ctx;
        }

        void GLContext::create() {
            // TODO
            void* nativeWindowHandle = nullptr;

            m_hdc = GetDC((HWND)nativeWindowHandle);
            HGLRC context = createContext();
        }

    }
}

#endif