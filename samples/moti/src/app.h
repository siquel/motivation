#pragma once

#include "moti/moti.h"
#include "moti/renderer/gl_context.h"
#include <SDL/SDL.h>

class App {
private:
    moti::gl::GLContext m_context;
    SDL_Window* m_wnd;
public:
    App();
    ~App();
    void frame();
    void update();
    int exec();
};