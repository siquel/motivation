#include "app.h"



App::App() {
    SDL_Init(SDL_INIT_VIDEO);
    m_wnd = SDL_CreateWindow("moti", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    m_context.create(m_wnd);
    glClearColor(1.f, 0.f, 0.f, 1.f);
}

App::~App() {

}

void App::frame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SDL_GL_SwapWindow(m_wnd);
}

void App::update() {

}

int App::exec() {
    SDL_Event e;
    while (SDL_WaitEvent(&e)) {
        if (e.type == SDL_QUIT) break;
        update();
        frame();
    }
    return 0;
}
