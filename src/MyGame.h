#ifndef __MY_GAME_H__
#define __MY_GAME_H__

#include <iostream>
#include <vector>
#include <string>

#include "SDL.h"
#include "SDL_ttf.h"; 
#include "SDL_mixer.h"


static struct GameData {
    int player1Y = 0;
    int player2Y = 0;
    int ballX = 0;
    int ballY = 0;
} game_data;

static struct sco {
    int P1score = 0;
    int P2score = 0;
}sco;

class MyGame {

private:
    SDL_Rect player1 = { 200, 0, 20, 60 };
    SDL_Rect player2 = { 580, 0, 20, 60 };
    SDL_Rect Ball = { 0, 0, 30, 30 };
    TTF_Font* font;
    SDL_Surface* borisHead;
    SDL_Surface* Covid;
    Mix_Chunk* batHit;
    Mix_Chunk* wallHit;




public:
    MyGame(TTF_Font* font, SDL_Surface* borisHead, SDL_Surface* Covid, Mix_Chunk* batHit, Mix_Chunk* wallHit);

    std::vector<std::string> messages;
    void on_receive(std::string message, std::vector<std::string>& args);
    void send(std::string message);
    void input(SDL_Event& event);
    void render(SDL_Renderer* renderer);
    void update();

};

#endif