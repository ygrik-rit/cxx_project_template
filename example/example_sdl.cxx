/*
 * Logic implementation of the Snake game. It is designed to efficiently
 * represent the state of the game in memory.
 *
 * This code is public domain. Feel free to use it for any purpose!
 */

#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

using AppState = struct
{
    SDL_Window*   window;
    SDL_Renderer* renderer;
    Uint64        last_step;
};

SDL_AppResult SDL_AppIterate(void* appstate)
{
    AppState* as = (AppState*)appstate;

    SDL_SetRenderDrawColor(as->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(as->renderer);

    SDL_SetRenderDrawColor(
        as->renderer, 255, 255, 0, SDL_ALPHA_OPAQUE); /*head*/
    SDL_RenderPresent(as->renderer);
    return SDL_APP_CONTINUE;
}

static const struct
{
    const char* key;
    const char* value;
} extended_metadata[] = { { SDL_PROP_APP_METADATA_URL_STRING,
                            "https://examples.libsdl.org/SDL3/demo/01-snake/" },
                          { SDL_PROP_APP_METADATA_CREATOR_STRING, "SDL team" },
                          { SDL_PROP_APP_METADATA_COPYRIGHT_STRING,
                            "Placed in the public domain" },
                          { SDL_PROP_APP_METADATA_TYPE_STRING, "game" } };

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    size_t i;

    if (!SDL_SetAppMetadata("Example Snake game", "1.0", "com.example.Snake"))
    {
        return SDL_APP_FAILURE;
    }

    for (i = 0; i < SDL_arraysize(extended_metadata); i++)
    {
        if (!SDL_SetAppMetadataProperty(extended_metadata[i].key,
                                        extended_metadata[i].value))
        {
            return SDL_APP_FAILURE;
        }
    }

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    AppState* as = (AppState*)SDL_calloc(1, sizeof(AppState));
    if (!as)
    {
        return SDL_APP_FAILURE;
    }

    *appstate = as;

    if (!SDL_CreateWindowAndRenderer("examples/demo/snake",
                                     100,
                                     100,
                                     SDL_WINDOW_RESIZABLE,
                                     &as->window,
                                     &as->renderer))
    {
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(
        as->renderer, 100, 100, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    as->last_step = SDL_GetTicks();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{

    switch (event->type)
    {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        default:
            break;
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    if (appstate != NULL)
    {
        AppState* as = (AppState*)appstate;
        SDL_DestroyRenderer(as->renderer);
        SDL_DestroyWindow(as->window);
        SDL_free(as);
    }
}
