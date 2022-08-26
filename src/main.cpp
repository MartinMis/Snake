#include <iostream>
#include <SFML/Graphics.hpp>

#define GAME_WIDTH 20
#define GAME_HEIGHT 20

typedef struct snake_part {
    int x;
    int y;
    struct snake_part* next;
} SNAKE_PART;

bool move_snake(SNAKE_PART* snake_head, int heading, bool extend) {
    if (snake_head == NULL) {
        return false;
    }

    SNAKE_PART* current_part = snake_head;
    int transfer_x = 0;
    int transfer_y = 0;

    SNAKE_PART* last_part = NULL;
    
    while (current_part != NULL) {
        int old_x = current_part->x;
        int old_y = current_part->y;

        if (current_part == snake_head){
            switch (heading){
                case 0:
                    snake_head->y--;
                    break;
                case 1:
                    snake_head->x++;
                    break;
                case 2:
                    snake_head->y++;
                    break;
                case 3:
                    snake_head->x--;
                    break;
                default:
                    return false;
            }
        }
        else {
            current_part->x = transfer_x;
            current_part->y = transfer_y;
        }
        transfer_x = old_x;
        transfer_y = old_y;

        if (current_part->next == NULL){
            last_part = current_part;
        }

        current_part = current_part->next;
    }

    if (extend){
        SNAKE_PART* new_part = new SNAKE_PART;
        new_part->x = transfer_x;
        new_part->y = transfer_y;
        new_part->next = NULL;

        last_part->next = new_part;
    }

    return true;
}

void draw_game(sf::RenderWindow* window, SNAKE_PART* snake_head, std::vector<int *> fruits){
    window->clear();

    sf::RectangleShape background(sf::Vector2f(800.0f,800.0f));
    background.setFillColor(sf::Color(130, 200, 70));

    window->draw(background);

    int cell_width = 800/GAME_WIDTH;
    int cell_height = 800/GAME_HEIGHT;

    // Draw verticla lines
    for (int i = 0; i < GAME_WIDTH; i++){
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f((float)cell_width*i, 0.0f)),
            sf::Vertex(sf::Vector2f((float)cell_width*i, 800.0f)),
        };

        line[0].color = sf::Color(75, 115, 40);
        line[1].color = sf::Color(75, 115, 40);

        window->draw(line, 2, sf::Lines);
    }

    // Draw horizontal lines
    for (int i = 0; i < GAME_HEIGHT; i++){
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(0.0f, (float)cell_height*i)),
            sf::Vertex(sf::Vector2f(800.0f, (float)cell_height*i)),
        };

        line[0].color = sf::Color(75, 115, 40);
        line[1].color = sf::Color(75, 115, 40);

        window->draw(line, 2, sf::Lines);
    }

    for (auto fruit = fruits.begin(); fruit != fruits.end(); ++fruit){
        sf::RectangleShape f_shape(sf::Vector2f((float)cell_width, (float)cell_height));
        f_shape.setFillColor(sf::Color::Red);
        f_shape.setPosition(sf::Vector2f((float)(*fruit)[0] * cell_width, (float)(*fruit)[1] * cell_height));

        window->draw(f_shape);
    }

    SNAKE_PART* current_part = snake_head;
    while (current_part != NULL){
        sf::RectangleShape part(sf::Vector2f((float)cell_width, (float)cell_height));
        part.setFillColor(sf::Color::Yellow);
        part.setPosition(sf::Vector2f((float)current_part->x * cell_width, (float)current_part->y * cell_height));
        
        window->draw(part);

        current_part = current_part->next;
    }

    window->display();
}

bool check_for_collision(SNAKE_PART* snake_head, int heading){
    int new_x = snake_head->x;
    int new_y = snake_head->y;

    switch (heading){
        case 0:
            new_y--;
            break;
        case 1:
            new_x++;
            break;
        case 2:
            new_y++;
            break;
        case 3:
            new_x--;
            break;
    }

    if (new_x >= GAME_WIDTH || new_x < 0) {
        return true;
    }
    if (new_y >= GAME_HEIGHT || new_y < 0) {
        return true;
    }

    SNAKE_PART* current_part = snake_head;
    while(current_part != NULL){
        if(current_part != snake_head && current_part->x == new_x && current_part->y == new_y){
            return true;
        }
        current_part = current_part->next;
    }

    return false;

}

bool check_fruit(SNAKE_PART* snake_head, int heading, std::vector<int*>* fruits) {
    int new_x = snake_head->x;
    int new_y = snake_head->y;

    switch (heading){
        case 0:
            new_y--;
            break;
        case 1:
            new_x++;
            break;
        case 2:
            new_y++;
            break;
        case 3:
            new_x--;
            break;
    }

    for (auto fruit = fruits->begin(); fruit != fruits->end(); ++fruit){
        if ((*fruit)[0] == new_x && (*fruit)[1] == new_y){
            fruits->erase(fruit);
            return true;
        }
    }
    return false;
}

int main() {
    bool game_over = false;

    SNAKE_PART* snake_head = new SNAKE_PART;
    snake_head->x = 10;
    snake_head->y = 10;
    snake_head->next = NULL;

    move_snake(snake_head, 1, true);
    move_snake(snake_head, 1, true);
    move_snake(snake_head, 1, true);
    move_snake(snake_head, 1, true);
    move_snake(snake_head, 1, true);

    
    sf::RenderWindow window(sf::VideoMode(800, 800), "Snake");
    std::vector<int*> fruits;

    sf::Clock move_clock;
    sf::Clock fruit_clock;
    int current_heading = 0;

    while (window.isOpen()){
        sf::Event event;

        while (window.pollEvent(event)) {

            if(event.type == sf::Event::Closed){
                window.close();
            }
        }

        if (!game_over){
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && current_heading != 1){
                current_heading = 3;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && current_heading != 3){
                current_heading = 1;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && current_heading != 2){
                current_heading = 0;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && current_heading != 0){
                current_heading = 2;
            }

            if (fruit_clock.getElapsedTime().asMilliseconds() >= 5000){
                fruit_clock.restart();
                int* new_fruit = new int[2];
                new_fruit[0] = rand()%21;
                new_fruit[1] = rand()%21;
                fruits.push_back(new_fruit);
            }

            if (move_clock.getElapsedTime().asMilliseconds() >= 100){
                move_clock.restart();
                if (!check_for_collision(snake_head, current_heading)){
                    if(check_fruit(snake_head, current_heading, &fruits)){
                        move_snake(snake_head, current_heading, true);
                    }
                    else{
                        move_snake(snake_head, current_heading, false);
                    }
                }
                else {
                    game_over = true;
                }  
            }
            draw_game(&window, snake_head, fruits);
        }
        else {
            sf::Text text;
            sf::Font font;

            font.loadFromFile("./res/Silkscreen-Regular.ttf");

            text.setString("Game Over!");

            text.setFont(font);
            text.setCharacterSize(100);
            text.setFillColor(sf::Color::Red);
            text.setPosition(sf::Vector2f(50.0f, 330.0f));

            window.draw(text);
            window.display();
        }

        
    }

    return 0;
}