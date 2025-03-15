#include "snake.h"

int main()
{
    int repeat = 1;
    while(repeat)
    {
        game *g = new game;
        repeat = g->snake_game();
        delete g;
    }
    return 0;
}