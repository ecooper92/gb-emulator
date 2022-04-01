#pragma once

#include "Memory.h"

#include <SDL.h>
#undef main

class Display
{
public:
	Display(Memory& memory) : memory(memory) {}

	void Initialize()
	{
        initSDL();
	}

	void Update()
	{
        prepareScene();

        doInput();

        presentScene();
	}
private:
	Memory& memory;

    struct App
    {
        SDL_Renderer* renderer;
        SDL_Window* window;
    };

    App app;

    void initSDL()
    {
        int rendererFlags, windowFlags;

        rendererFlags = SDL_RENDERER_ACCELERATED;

        windowFlags = 0;

        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            std::cout << "Failed to open window: " << SDL_GetError() << std::endl;
            std::terminate();
        }

        app.window = SDL_CreateWindow("Shooter 01", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 160, 144, windowFlags);

        if (!app.window)
        {
            std::cout << "Failed to open window: " << SDL_GetError() << std::endl;
            std::terminate();
        }

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

        app.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);

        if (!app.renderer)
        {
            std::cout << "Failed to create renderer: " << SDL_GetError() << std::endl;
            std::terminate();
        }
    }

    void doInput()
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                std::terminate();
                break;

            default:
                break;
            }
        }
    }

    void prepareScene()
    {
        SDL_SetRenderDrawColor(app.renderer, 96, 128, 255, 255);
        SDL_RenderClear(app.renderer);
    }

    void presentScene()
    {
        SDL_RenderPresent(app.renderer);
    }
};