#ifndef SDL_WRAPPER_HPP
#define SDL_WRAPPER_HPP

#include <SDL2/SDL.h>
#include <iostream>

class SDLManager
{
public: 
    SDLManager(int width = 800, int height = 600);
    ~SDLManager();


    void PollEvent();
    void SimpleUpdateTexture(uint32_t* frame_buffer);
    void RenderTexture();
private:
    void HandleEvent(SDL_Event* event);
    


        


    int m_Height;
    int m_Width;
    SDL_Renderer* m_SDLRenderer;
    SDL_Window* m_SDLWindow; 
    SDL_Texture* m_Texture;
};



#endif