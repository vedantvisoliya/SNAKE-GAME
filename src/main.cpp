#include <raylib.h>
#include <iostream>
#include <random>
#include <deque>
#include <raymath.h>
using namespace std;

Color green = {173, 204, 96, 255};
Color dark_green = {43,51, 24, 255};

float cell_size = 30;
float cell_count = 25;

double last_updated_time = 0;

int score = 0;

bool start_window = true;

float random_num(int min, int max){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}

bool snakebody_food_overlapping(Vector2 food_pos, deque<Vector2> snake){
    for (unsigned int i = 0; i < snake.size(); i++){
        if(Vector2Equals(snake[i], food_pos)){
            return true;
        }
    }
    return false;
}

bool event_triggered(double interval) {
    double current_time = GetTime();
    if ((current_time - last_updated_time) >= interval){
        last_updated_time = current_time;
        return true;
    }
    return false;
}

class Snake
{
public: 
    deque<Vector2> body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
    Vector2 direction = {1,0};
    bool grow_snake = false;

    void draw(){
        for (unsigned int i = 0; i < body.size(); i++){
            int x = body[i].x;
            int y = body[i].y;
            DrawRectangle(x*cell_size, y*cell_size, cell_size-5, cell_size-5, dark_green);
        }
    }

    void update(){
        body.push_front(Vector2Add(body[0], direction));
        if(grow_snake == true){
            grow_snake = false;
        }
        else{
        body.pop_back();
        }
    }
};

class Food
{
public:

    void draw(Vector2 position){
        DrawCircle((position.x*cell_size)+15, (position.y*cell_size)+15, 14.5, dark_green);
        // DrawRectangle(position.x*cell_size, position.y*cell_size, cell_size, cell_size, dark_green);
    }

    Vector2 gen_rand_cell_pos(){
        float x = random_num(4, cell_count-4);
        float y = random_num(4, cell_count-4);
        return Vector2{x, y};
    }
};

class Grid 
{
public:
    void draw(){
        int a = 0;
        for (int i = 0; i < 25; i++){
            DrawLine(0, 30+a, GetScreenWidth(), 30+a, BLACK);
            DrawLine(30+a, 0, 30+a, GetScreenHeight(), BLACK);
            a += 30;
        }
    }
};

class Game 
{
public:
    Snake snake = Snake();
    Food food = Food();
    Vector2 position = get_rand_pos(snake.body);
    void draw(){
        snake.draw();
        food.draw(position);
    }

    void update(){
        snake.update();
        check_collision_with_food();
    }

    void check_collision_with_food(){
        if (Vector2Equals(snake.body[0], position)){
            // cout << "eating the food...." << endl;
            position = get_rand_pos(snake.body);
            snake.grow_snake = true;
            score++;
        }
    }

    Vector2 get_rand_pos(deque<Vector2> snake_body){
        Vector2 position = food.gen_rand_cell_pos();
        while (snakebody_food_overlapping(position, snake_body))
        {
            position = food.gen_rand_cell_pos();
        }
        return position;
    }

    bool check_snake_boundary_crossing(){
        for (unsigned int i = 2; i < 23; i++){
            if(Vector2Equals(snake.body[0], {2, (float)i})){
                return true;
            }
            else if(Vector2Equals(snake.body[0], {(float)i, 2})){
                return true;
            }

            if(Vector2Equals(snake.body[0], {22, (float)i})){
                return true;
            }
            else if(Vector2Equals(snake.body[0], {(float)i, 22})){
                return true;
            }
        }
        return false;
    }

    bool check_snakeHead_snake_body_collision(){
        for(unsigned int i = 1; i < snake.body.size(); i++){
            if(Vector2Equals(snake.body[0], snake.body[i])){
                return true;
            }
        }
        return false;
    }

    void start_screen(){
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("SNAKE", GetScreenWidth()/4+100, GetScreenHeight()/4, 60, WHITE);
        DrawText("GAME", GetScreenWidth()/4+75, GetScreenHeight()/4+55, 90, WHITE);
        DrawText("'enter'", GetScreenWidth()/2-75, (GetScreenHeight()-GetScreenHeight()/4), 40, WHITE);
        EndDrawing();
    }
};

Grid grid;

int main(){
    cout << "Starting the game..." << endl;

    int screen_width = cell_size*cell_count;
    int screen_height = cell_size*cell_count;

    InitWindow(screen_width, screen_height, "SNAKE GAME");
    SetTargetFPS(60);

    Game game = Game();

    while(WindowShouldClose() == false){
        BeginDrawing();
        // EVENT HANDLING
        if (start_window == true){
            while (true)
            {
                game.start_screen();
                if(IsKeyPressed(KEY_ENTER)){
                    start_window = false;
                    break;
                }
            }
        }

        if((game.check_snake_boundary_crossing() == true) || (game.check_snakeHead_snake_body_collision() == true)){
            while (true)
            {
                BeginDrawing();
                ClearBackground(BLACK);
                DrawText("GAME OVER!", screen_width/4-50, screen_height/4, 80, WHITE);
                DrawText(TextFormat("%i", score), screen_width/2-25, screen_height/2, 120, WHITE);
                DrawText("PRESS 'enter' FOR RESTART!", screen_width/2-225, screen_height/2+250, 30, WHITE);
                DrawText("press 'enter' then 'escape' to exit", screen_width/4-40, screen_height/2+290,30, WHITE);
                if(IsKeyPressed(KEY_ENTER)){
                    game.snake.body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
                    game.snake.direction = {1, 0};
                    game.position = game.get_rand_pos(game.snake.body);
                    score = 0;
                    break;
                }
                EndDrawing();
            }
        }
        // UPDATING POSITION
        if(event_triggered(0.2)){
            game.update();
        }

        if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1){
            game.snake.direction = {0, -1};
        }
        else if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1){
            game.snake.direction = {0, 1};
        }
        else if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1){
            game.snake.direction = {-1, 0};
        }
        else if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1){
            game.snake.direction = {1, 0};
        }
        
        // DRAWING OBJECTS
        ClearBackground(green);

        // GAME
        game.draw();
        // TOP LINE
        DrawLineEx({2*cell_size, 2*cell_size}, {23*cell_size, 2*cell_size}, 5, BLACK);
        // LEFT LINE
        DrawLineEx({2*cell_size, 2*cell_size}, {2*cell_size, 23*cell_size}, 5, BLACK);
        // RIGHT LINE
        DrawLineEx({23*cell_size, 2*cell_size}, {23*cell_size, 23*cell_size}, 5, BLACK);
        // BOTTOM LINE
        DrawLineEx({2*cell_size, 23*cell_size}, {23*cell_size, 23*cell_size}, 5, BLACK); 
        // SCORE
        DrawText(TextFormat("%i", score), 2*cell_size, 24*cell_size-15, 40, dark_green);
        // DRAW TEXT
        DrawText("RETRO SNAKE", 2*cell_size, cell_size-15, 30, dark_green);

        // GRID
        // grid.draw();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}