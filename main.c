//HARD-CODE - line of code with this comment has hard-coded numbers
//code commented like /*this*/ are optional feature(s).

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define FPS 60
#define FRAME_TIME (1000 / FPS)

typedef struct {
    enum Direction {UP, DOWN, LEFT, RIGHT} direction;
    int velocity; //feature in the future, unussed as of now!
} SnakeVector;

void render_square(SDL_Renderer *renderer, int x, int y);
void render_text(SDL_Renderer *renderer, TTF_Font *font, bool game_over, int score, int x, int y);
bool snake_self_collided(int x_position, int y_position, SDL_Rect snake_body_part);
bool snake_window_collision(int x_position, int y_position);
int* generate_apple_position();
/*void render_grid(SDL_Renderer *renderer);*/

//width and height of window (in pixels)
const int WIDTH = 600, HEIGHT = 600;
//grid pixel size
const int GRID_PIXEL_HEIGHT = 20, GRID_PIXEL_WIDTH = 20;
//max length of snake
const int SNAKE_MAX_LENGTH = 500;
//snake length increase by this number
const int SNAKE_LENGTH_INC = 4;

int main(int argc, char *argv[]) {
    srand(time(NULL));
    SDL_Init(SDL_INIT_EVERYTHING);

    //create window
    SDL_Window *window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Could not create window: %s", SDL_GetError());
        return 1;
    }

    //create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Could not create renderer: %s", SDL_GetError());
        return 1;
    }

    //create ttf (text capabilities)
    if (TTF_Init() == -1) {
        printf("Could not initialize SDL_ttf: %s\n", TTF_GetError());
        return 1;
    }
    TTF_Font *font = TTF_OpenFont("src/font/rubik.ttf", 24);
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return 1;
    }

    //snake & apple move mechanics setup
    SnakeVector snake = {RIGHT, 1};
    int last_move_time = 0;
    int x_position = GRID_PIXEL_WIDTH * 3, y_position = HEIGHT / 2; //default snake position //HARD-CODE
    int apple_x_position = GRID_PIXEL_WIDTH * 20, apple_y_position = HEIGHT / 2; //default apple position //HARD-CODE

    //snake array, length, and score based on length
    SDL_Rect snake_body[SNAKE_MAX_LENGTH];
    int snake_length = 1;
    bool game_over = false;

    //window loop
    bool running = true;
    SDL_Event window_event;

    while (running) {
        int current_time = SDL_GetTicks();

        if (SDL_PollEvent(&window_event)) {
            switch (window_event.type) {
                //close window
                case SDL_QUIT:
                    running = false;
                    break;

                //keyboard input change direction enum
                case SDL_KEYDOWN:
                    switch (window_event.key.keysym.sym) {
                        case SDLK_w:
                        case SDLK_UP:
                            if (snake.direction == DOWN) {break;}
                            snake.direction = UP;
                            break;
                        case SDLK_a:
                        case SDLK_LEFT:
                            if (snake.direction == RIGHT) {break;}
                            snake.direction = LEFT;
                            break;
                        case SDLK_s:
                        case SDLK_DOWN:
                            if (snake.direction == UP) {break;}
                            snake.direction = DOWN;
                            break;
                        case SDLK_d:
                        case SDLK_RIGHT:
                            if (snake.direction == LEFT) {break;}
                            snake.direction = RIGHT;
                            break;
                    }

                    break;
            }
        }

        //game loop
        if (current_time - last_move_time >= 167) {  //sixth of a second
            last_move_time = current_time;

            //change snake head position based on direction
            switch (snake.direction) {
                case UP:
                    y_position -= GRID_PIXEL_HEIGHT * snake.velocity;
                    break;
                case LEFT:
                    x_position -= GRID_PIXEL_WIDTH * snake.velocity;
                    break;
                case DOWN:
                    y_position += GRID_PIXEL_HEIGHT * snake.velocity;
                    break;
                case RIGHT:
                    x_position += GRID_PIXEL_WIDTH * snake.velocity;
                    break;
            }

            //snake collides with itself
            for (int i = 1; i < snake_length; i++) {
                if (snake_self_collided(x_position, y_position, snake_body[i])) {
                    game_over = true;
                    break;
                }

                if (apple_x_position == snake_body[i].x && apple_y_position == snake_body[i].y) {

                }
            }

            //snake goes out of window viewport
            if (snake_window_collision(x_position, y_position)) {
                game_over = true;
            }

            //check snake/apple collision
            if (x_position == apple_x_position && y_position == apple_y_position) {
                if (snake_length < SNAKE_MAX_LENGTH) {
                    snake_length += SNAKE_LENGTH_INC; //one apple equals determined length increase
                }
                else {
                    //add prompt "YOU WIN!"
                }
        
                //generate new apple position
                int* new_apple_position = generate_apple_position();
                apple_x_position = new_apple_position[0];
                apple_y_position = new_apple_position[1];
                free(new_apple_position);
            }

            //shift snake body forward
            for (int i = snake_length; i > 0; i--) {
                snake_body[i] = snake_body[i - 1];
            }

            //update snake head
            snake_body[0] = (SDL_Rect){x_position, y_position, GRID_PIXEL_WIDTH, GRID_PIXEL_HEIGHT};
        }

        //clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        //render snake body
        for (int i = 0; i < snake_length; i++) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(renderer, &snake_body[i]);
        }

        //render visuals
        render_square(renderer, apple_x_position, apple_y_position);
        render_text(renderer, font, game_over, snake_length, 10, 10); //text x & y position
        /*render_grid(renderer);*/

        //refresh window
        SDL_RenderPresent(renderer);

        //check if snake game over'd
        if (game_over) {
            SDL_Delay(5000); //5 second pause
            running = false;
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_CloseFont(font);
    TTF_Quit();
    return EXIT_SUCCESS;
}

void render_square(SDL_Renderer *renderer, int x, int y) {    
    SDL_Rect square = {
        x,                //x position
        y,                //y position
        GRID_PIXEL_WIDTH, //width
        GRID_PIXEL_HEIGHT //height
    };

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &square); //render
}

void render_text(SDL_Renderer *renderer, TTF_Font *font, bool game_over, int score, int x, int y) {
    SDL_Color color = {255, 0, 0, 255};
    SDL_Surface *surface;
    char text[20];

    if (game_over) {
        sprintf(text, "Game Over! Your score: %d", score);
        surface = TTF_RenderText_Solid(font, text, color);
    }
    else {
        sprintf(text, "Score: %d", score);
        surface = TTF_RenderText_Solid(font, text, color);
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect text_rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &text_rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int* generate_apple_position() {
    //2 numbers between 0 and width/height of window
    int random_number_x = rand() % (WIDTH - GRID_PIXEL_WIDTH + 1);
    int random_number_y = rand() % (HEIGHT - GRID_PIXEL_HEIGHT + 1);

    //round random numbers to the nearest multiple of the grid pixel size; apple_position array contains x & y position of apple
    int* apple_position = (int*)malloc(2 * sizeof(2));

    int apple_x_position = ((random_number_x + GRID_PIXEL_WIDTH / 2) / GRID_PIXEL_WIDTH) * GRID_PIXEL_WIDTH;
    int apple_y_position = ((random_number_y + GRID_PIXEL_HEIGHT / 2) / GRID_PIXEL_HEIGHT) * GRID_PIXEL_HEIGHT;
    apple_position[0] = apple_x_position;
    apple_position[1] = apple_y_position;
    
    return apple_position;
}

bool snake_self_collided(int x_position, int y_position, SDL_Rect snake_body_part) {
    return x_position == snake_body_part.x && y_position == snake_body_part.y;
}

bool snake_window_collision(int x_position, int y_position) {
    return x_position < 0 || x_position >= WIDTH || y_position < 0 || y_position >= HEIGHT;
}

/*
void render_grid(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); //line color

    //verital lines
    for (int x = 0; x <= WIDTH; x += GRID_PIXEL_WIDTH) {
        SDL_RenderDrawLine(renderer, x, 0, x, HEIGHT);
    }

    //horizontal lines
    for (int y = 0; y <= HEIGHT; y += GRID_PIXEL_HEIGHT) { 
        SDL_RenderDrawLine(renderer, 0, y, WIDTH, y);
    }
}
*/