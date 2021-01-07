#include "MyGame.h"

MyGame::MyGame(TTF_Font*font, SDL_Surface* image, Mix_Chunk* batHit, Mix_Chunk* wallHit) {
    this-> font = font;
    this->image = image;
    this->batHit = batHit;
    this->wallHit = wallHit;

    int x = 0;
    int y = 0;

    for (size_t i = 0; i < 1000; i++)
    {
        double random_Value = rand() * 1.0 / RAND_MAX;

        particles.push_back(new Particle(x, y, 1, {(Uint8) (255 * random_Value), 122, 30, 225 }));

        x++;

        if (x == 50) {
            y++;
        }
    }
}

Particle::Particle(int x, int y, int size, SDL_Color color) {
    this->x = x;
    this->y = y;
    this->size = size;
    this->color = color;
}

void MyGame::on_receive(std::string cmd, std::vector<std::string>& args) {
    if (cmd == "GAME_DATA") {
        // we should have exactly 4 arguments
        if (args.size() == 4) {
            game_data.player1Y = stoi(args.at(0));
            game_data.player2Y = stoi(args.at(1));
            game_data.ballX = stoi(args.at(2));
            game_data.ballY = stoi(args.at(3));

        }
    }
    else if (cmd == "SCORES") {
        
        if (args.size() == 2) {
            sco.P1score = stoi(args.at(0));
            sco.P2score = stoi(args.at(1));
            std::cout << "P2Score: " << sco.P2score << std::endl;
            std::cout << "P1Score: " << sco.P1score << std::endl;
        }
    }
    else if (cmd == "BALL_HIT_BAT1"| cmd == "BALL_HIT_BAT2") {
        Mix_PlayChannel(-1, batHit, 0);
    }
    else if (cmd == "HIT_WALL_LEFTGAME_DATA"| cmd == "HIT_WALL_RIGHTGAME_DATA")
    {
        Mix_PlayChannel(-1, wallHit, 0);
    }
    else {
        std::cout << "Received: " << cmd << std::endl;
    }
}

void MyGame::send(std::string message) {
    messages.push_back(message);
}

void MyGame::input(SDL_Event& event) {
    switch (event.key.keysym.sym) {
        case SDLK_w:
            send(event.type == SDL_KEYDOWN ? "W_DOWN" : "W_UP");
            break;
        case SDLK_s:
            send(event.type == SDL_KEYDOWN ? "S_DOWN" : "S_UP");
            break;
        case SDLK_i:
            send(event.type == SDL_KEYDOWN ? "I_DOWN" : "I_UP");
            break;
        case SDLK_k:
            send(event.type == SDL_KEYDOWN ? "K_DOWN" : "K_UP");
            break;
        
    }
}

void MyGame::update() {
    player1.y = game_data.player1Y;
    player2.y = game_data.player2Y;
    Ball.y = game_data.ballY;
    Ball.x = game_data.ballX;
    
}

void MyGame::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &player1);
    SDL_RenderFillRect(renderer, &player1);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &player2);
    SDL_RenderFillRect(renderer, &player2);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    //SDL_RenderDrawRect(renderer, &Ball);
    //SDL_RenderFillRect(renderer, &Ball);

    SDL_Rect dst = { 100,100,64,64 };
    auto texture = SDL_CreateTextureFromSurface(renderer, image);

    SDL_RenderCopy(renderer, texture, NULL, &Ball);

    SDL_RenderPresent(renderer);

    std::string score_textp1 = std::to_string(sco.P1score);
    std::string score_textp2 = std::to_string(sco.P2score);

    SDL_Color text_color = { 255, 255, 255, 255 };

    SDL_Surface* text_surfacep1 = TTF_RenderText_Blended(font, score_textp1.c_str(), text_color);
    SDL_Surface* text_surfacep2 = TTF_RenderText_Blended(font,score_textp2.c_str(), text_color);

    if (text_surfacep1 != nullptr || text_surfacep2 != nullptr) {
        SDL_Texture* text_texturep1 = SDL_CreateTextureFromSurface(renderer, text_surfacep1);
        SDL_Texture* text_texturep2 = SDL_CreateTextureFromSurface(renderer, text_surfacep2);

        if (text_texturep1 != nullptr || text_texturep2 != nullptr) {
            int w, h;
            int ww, hh;
            SDL_QueryTexture(text_texturep1, NULL, NULL, &w, &h);
            SDL_QueryTexture(text_texturep2, NULL, NULL, &ww, &hh);

            SDL_Rect dst1 = {0, 50, w, h};
            SDL_Rect dst2 = {680, 50, ww, hh};

            //NULL to draw entire texture

            SDL_RenderCopy(renderer, text_texturep1, NULL, &dst1);
            SDL_RenderCopy(renderer, text_texturep2, NULL, &dst2);

            SDL_DestroyTexture(text_texturep1);
            SDL_DestroyTexture(text_texturep2);
        }
    }

    for (auto p : particles) 
    {
        SDL_Rect rect = { p->x, p->y, p->size, p->size };

        SDL_SetRenderDrawColor(renderer, p->color.r, p->color.g, p->color.b, p->color.a);

        SDL_RenderFillRect(renderer, &rect);
    }


}