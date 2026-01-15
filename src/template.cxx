module;
#include <SDL3/SDL.h>
module engine;

namespace tlt::window
{
window_impl::window_impl(std::string title, int w, int h)
    : title(std::move(title))
    , width(w)
    , height(h)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        std::cout << "Cant init SDL: " << SDL_GetError() << std::endl;
    }
}

bool window_impl::create() noexcept
{
    window =
        SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_FULLSCREEN);
    if (window)
    {
        running = true;
        std::cout << "Window created" << std::endl;
    }
    else
    {
        std::cout << "Cant create window:" << SDL_GetError() << std::endl;
    }
    return running;
}

void window_impl::close() noexcept
{
    SDL_DestroyWindow(window);
    window  = nullptr;
    running = false;

    SDL_Quit();
}

void window_impl::poll_event() noexcept
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_EVENT_QUIT:
                close();
                break;
            case SDL_EVENT_KEY_DOWN:
                std::cout << tlt::input::keycode_to_name(
                    static_cast<tlt::input::keycode>(event.key.key));
                break;
            default:
                break;
        }
    }
}

bool window_impl::is_running() const noexcept
{
    return running;
}

} // namespace tlt::window