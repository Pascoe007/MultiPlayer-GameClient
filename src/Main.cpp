#include "SDL_net.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "MyGame.h"

using namespace std;

const char* IP_NAME = "localhost";
const Uint16 PORT = 55555;

bool is_running = true;

MyGame* game;



static int on_receive(void* socket_ptr) {
    TCPsocket socket = (TCPsocket)socket_ptr;

    const int message_length = 1024;

    char message[message_length];
    int received;

    // TODO: while(), rather than do
    do {
        received = SDLNet_TCP_Recv(socket, message, message_length);
        message[received] = '\0';

        char* pch = strtok(message, ",");

        // get the command, which is the first string in the message
        string cmd(pch);

        // then get the arguments to the command
        vector<string> args;

        while (pch != NULL) {
            pch = strtok(NULL, ",");

            if (pch != NULL) {
                args.push_back(string(pch));
            }
        }

        game->on_receive(cmd, args);

        if (cmd == "exit") {
            break;
        }

    } while (received > 0 && is_running);

    return 0;
}

static int on_send(void* socket_ptr) {
    TCPsocket socket = (TCPsocket)socket_ptr;

    while (is_running) {
        if (game->messages.size() > 0) {
            string message = "CLIENT_DATA";

            for (auto m : game->messages) {
                message += "," + m;
            }

            game->messages.clear();

            cout << "Sending_TCP: " << message << endl;

            SDLNet_TCP_Send(socket, message.c_str(), message.length());
        }

        SDL_Delay(1);
    }

    return 0;
}

void loop(SDL_Renderer* renderer) {
    SDL_Event event;

    while (is_running) {
        // input
        while (SDL_PollEvent(&event)) {
            if ((event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) && event.key.repeat == 0) {
                game->input(event);

                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    is_running = false;
                    break;

                default:
                    break;
                }
            }

            if (event.type == SDL_QUIT) {
                is_running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        game->update();

        game->render(renderer);

        SDL_RenderPresent(renderer);

        SDL_Delay(17);
    }
}

int run_game() {
    SDL_Window* window = SDL_CreateWindow(
        "Multiplayer Pong Client",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN
    );

    if (nullptr == window) {
        std::cout << "Failed to create window" << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);



    if (nullptr == renderer) {
        std::cout << "Failed to create renderer: " << SDL_GetError() << std::endl;
        return -1;
    }



    loop(renderer);

    return 0;
}

int main(int argc, char** argv) {



    // Initialize SDL
    if (SDL_Init(0) == -1) {
        printf("SDL_Init: %s\n", SDL_GetError());
        exit(1);
    }

    // Initialize TTF
    if (TTF_Init() == -1) {
        printf("TTF_Init: %s\n", TTF_GetError());
        exit(2);
    }

    // Loading Font
    TTF_Font* font = TTF_OpenFont("Font/arial.ttf", 72);

    // Checking if font is loaded
    if (font == nullptr) {
        std::cout << "Failed to create font: " << TTF_GetError() << std::endl;
    }
    else {
        std::cout << "Font Loaded" << std::endl;
    }

    // load support for the JPG and PNG image formats
    int flags = IMG_INIT_JPG | IMG_INIT_PNG;
    int initted = IMG_Init(flags);
    if ((initted & flags) != flags) {
        printf("IMG_Init: Failed to init required jpg and png support!\n");
        printf("IMG_Init: %s\n", IMG_GetError());
        // handle error
    }
    // load sample.png into image
    SDL_Surface* borisHead = IMG_Load("Images/borisHead.png");
    SDL_Surface* backGround = IMG_Load("Images/Background.jpg");
    SDL_Surface* Covid = IMG_Load("Images/Covid.jpg");
    if (!borisHead || !Covid) {
        printf("IMG_Load: %s\n", IMG_GetError());
        // handle error
    }
    if (Covid == nullptr) {
        std::cout << "CovidNotLoaded" << std::endl;
    }

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("Failed to init SDL\n");
        exit(1);
    }
    int result = 0;
    int flag = MIX_INIT_MP3;
    if (flag != (result = Mix_Init(flag))) {
        printf("Could not initialize mixer (result: %d).\n", result);
        printf("Mix_Init: %s\n", Mix_GetError());
        exit(1);
    }
    Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
    Mix_Music* music = Mix_LoadMUS("Sounds/BackGround.mp3");
    Mix_Chunk* batHit = Mix_LoadWAV("Sounds/pingpong.wav");
    Mix_Chunk* wallHit = Mix_LoadWAV("Sounds/yes.wav");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(64);

    if (!music | !batHit | !wallHit) {
        std::cout << "Music Not Loaded" << std::endl;
    }

    // Initialize SDL_net
    if (SDLNet_Init() == -1) {
        printf("SDLNet_Init: %s\n", SDLNet_GetError());
        exit(3);
    }

    IPaddress ip;

    // Resolve host (ip name + port) into an IPaddress type
    if (SDLNet_ResolveHost(&ip, IP_NAME, PORT) == -1) {
        printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        exit(4);
    }

    // Open the connection to the server
    TCPsocket socket = SDLNet_TCP_Open(&ip);

    if (!socket) {
        printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        exit(5);
    }

    game = new MyGame(font, borisHead,backGround, Covid, batHit, wallHit);



    SDL_CreateThread(on_receive, "ConnectionReceiveThread", (void*)socket);
    SDL_CreateThread(on_send, "ConnectionSendThread", (void*)socket);



    run_game();

    delete game;

    // Close connection to the server
    SDLNet_TCP_Close(socket);

    // Shutdown SDL_net
    SDLNet_Quit();

    //Shutdown Font
    TTF_CloseFont(font);

    // Shutdown TTF
    TTF_Quit();

    SDL_FreeSurface(borisHead);
    SDL_FreeSurface(backGround);
    SDL_FreeSurface(Covid);

    Mix_FreeMusic(music);
    Mix_FreeChunk(batHit);
    Mix_FreeChunk(wallHit);

    IMG_Quit();

    // Shutdown SDL
    SDL_Quit();

    return 0;
}