#include "sdl.hpp"

SDLManager::SDLManager(int width, int height) : m_Height(height), m_Width(width)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        exit(-1);
    }

    m_SDLWindow = SDL_CreateWindow(
        "Software Renderer",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        m_Width, m_Height,
        SDL_WINDOW_SHOWN
    );
    if (!m_SDLWindow) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(-1);
    }


    m_SDLRenderer = SDL_CreateRenderer(m_SDLWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!m_SDLRenderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(m_SDLWindow);
        SDL_Quit();
        exit(-1);
    }


     // 1. Create SDL texture that will display our framebuffer
    m_Texture = SDL_CreateTexture(
        m_SDLRenderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        m_Width, m_Height
    );



}

SDLManager::~SDLManager()
{
    SDL_DestroyTexture(m_Texture);
    SDL_DestroyRenderer(m_SDLRenderer);
    SDL_DestroyWindow(m_SDLWindow);
    SDL_Quit();
}

void SDLManager::PollEvent()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        HandleEvent(&event);
    }
}

void SDLManager::SimpleUpdateTexture(uint32_t *frame_buffer)
{
    SDL_UpdateTexture(m_Texture, nullptr, frame_buffer, m_Width * sizeof(uint32_t));
}

void SDLManager::RenderTexture()
{
    SDL_RenderClear(m_SDLRenderer);
    SDL_RenderCopy(m_SDLRenderer, m_Texture, nullptr, nullptr);
    SDL_RenderPresent(m_SDLRenderer);

}

void SDLManager::HandleEvent(SDL_Event *event)
{
    if (event->type == SDL_QUIT) exit(0);  // case dispatch later
    switch(event->type)
    {
        case SDL_KEYDOWN: HandleKeyDown(event->key.keysym.sym); break;
        default:
            return;
    }
}

void SDLManager::HandleKeyDown(SDL_Keycode key)
{
    switch (key)
    {
        case SDL_KeyCode::SDLK_UP: m_Camera->MoveForward(); break;
        case SDL_KeyCode::SDLK_DOWN: m_Camera->MoveBackward(); break;
        case SDL_KeyCode::SDLK_LEFT: m_Camera->MoveLeft(); break;
        case SDL_KeyCode::SDLK_RIGHT: m_Camera->MoveRight(); break;
        case SDL_KeyCode::SDLK_SPACE: m_Renderer->ToggleBackFaceCulling(); break;
        default:
            return;
    }
}
