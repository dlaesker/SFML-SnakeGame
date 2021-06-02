#include <SFML/Graphics.hpp>
#include <time.h>
#include <vector>
#include <stdio.h>

/* ******************* *
 Author: Denis Laesker *
 ********************* */

/*
 Description: Simple implementation of the well-known snake game with circles. Use the arrow keys to move the snake around and spacebar to start the game. As it stands, the borders are forbidden regions that will cause the window to close. Also, canibalism is not allowed. Thus, the snake biting its own tail will also terminate the game.
 
 Ideas to increase difficulty include:
 1) Adding obstacles
 2) Increasing the speed of the snake after consuming n food
 */

#define WIN_HEIGHT 	800
#define WIN_WIDTH 	800
#define RADIUS			10
#define BDR_WIDTH		20
#define SPEED				20
#define FRAME_RATE	60

typedef enum {NONE, NORTH, EAST, SOUTH, WEST} DIRECTION;

typedef struct POINT {
	int x; int y;
	POINT(int _x, int _y) : x(_x), y(_y){}
}POINT;

bool operator == (const POINT &a, const POINT &b){
	return ((a.x == b.x) && (a.y == b.y));
}

std::vector<sf::CircleShape> snake; // Faux linkedlist
std::vector<POINT> snakePos;				// To track the snake's position

// Generates a border around the playing area
void fillBorder(sf::RenderWindow *win){
	sf::RectangleShape bdr({BDR_WIDTH,BDR_WIDTH});
	bdr.setFillColor(sf::Color::Black);
	for(int i = 0; i < WIN_WIDTH; ++i){
		bdr.setPosition(0, i); 											// Left border
		win->draw(bdr);
		bdr.setPosition(WIN_WIDTH - BDR_WIDTH, i); // Right border
		win->draw(bdr);
		bdr.setPosition(i, 0);											// Upper border
		win->draw(bdr);
		bdr.setPosition(i, WIN_HEIGHT - BDR_WIDTH);	// Lower border
		win->draw(bdr);
	}
}

// The following functions are used to generate a random number with a step
int rnd(int min, int max){ return rand() % max + min; }

int rnd_range(int min, int max, int step){
	return rnd(min/step, max/step) * step;
}

int main(){
	srand(time(NULL));
	sf::ContextSettings settings;
  settings.antialiasingLevel = 16;
	
	sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "Snake", sf::Style::Default, settings);
	window.setFramerateLimit(FRAME_RATE);
		
	snake.push_back(sf::CircleShape(RADIUS, 10));
	snakePos.push_back(POINT(100,100)); // Initial snake position

	POINT foodPos(100,200);	// Food is placed here
	sf::CircleShape food(RADIUS, 10);
	food.setPosition(foodPos.x, foodPos.y);
	food.setFillColor(sf::Color::Red);
	
	DIRECTION preDirection = NONE;
	DIRECTION curDirection = SOUTH;
	
	sf::Event event;
	sf::Clock clock;
	
	int spaceHasBeenPressed = 0;
	float t = 50.0;
	
	while(window.isOpen()){
		while (window.pollEvent(event)){
			if(event.type == sf::Event::Closed)
				window.close();
	
			if(	(event.type == sf::Event::KeyPressed) &&
					(event.key.code == sf::Keyboard::Escape))
				window.close();
			
			if(	(event.type == sf::Event::KeyPressed) &&
					(	event.key.code == sf::Keyboard::Space))
				spaceHasBeenPressed = 1;
			
			/* USER INPUT */
			if(	(event.type == sf::Event::KeyPressed) &&
					(event.key.code == sf::Keyboard::Up)){
				preDirection = curDirection;
				curDirection = NORTH;
			}
			
			if(	(event.type == sf::Event::KeyPressed) &&
					(event.key.code == sf::Keyboard::Right)){
				preDirection = curDirection;
				curDirection = EAST;
			}
			
			if(	(event.type == sf::Event::KeyPressed) &&
					(event.key.code == sf::Keyboard::Down)){
				preDirection = curDirection;
				curDirection = SOUTH;
			}
			
			if(	(event.type == sf::Event::KeyPressed) &&
					(event.key.code == sf::Keyboard::Left)){
				preDirection = curDirection;
				curDirection = WEST;
			}
		}
		
		if(!spaceHasBeenPressed) continue;
		
		if(clock.getElapsedTime().asMilliseconds() >= t){
			/* COLLISION */
			// Check Borders
			if(snakePos[0].x < BDR_WIDTH) window.close();
			if(snakePos[0].x >= WIN_WIDTH - BDR_WIDTH) window.close();
			if(snakePos[0].y < BDR_WIDTH) window.close();
			if(snakePos[0].y >= WIN_HEIGHT - BDR_WIDTH) window.close();
			
			// Has the snake bit its tail?
			for(int i = 1; i < snake.size(); ++i){
				if(snakePos[0] == snakePos[i]){
					// Allow for horizontal and vertical switch
					if( (preDirection == WEST 	&& curDirection == EAST)	 	||
							(preDirection == EAST  	&& curDirection == WEST) 		||
							(preDirection == SOUTH 	&& curDirection == NORTH) 	||
							(preDirection == NORTH 	&& curDirection == SOUTH)){
								continue;
					}
					window.close();
				}
			}
			
			// Did the snake eat the food?
			if(foodPos == snakePos[0]){
				snake.push_back(sf::CircleShape(RADIUS, 10));
				snakePos.push_back(POINT(0,0));
				
				//If so...
				// ...where will the food appear next?
				foodPos.x = rnd_range(BDR_WIDTH + RADIUS, WIN_WIDTH - (BDR_WIDTH + RADIUS), SPEED);
				foodPos.y = rnd_range(BDR_WIDTH + RADIUS, WIN_HEIGHT - (BDR_WIDTH + RADIUS), SPEED);
			}
			
			/* Update the position of ea. segment of the snake, except the head, to the next segment. */
			for(int i = snake.size() - 1; i > 0; --i){
				snakePos[i] = snakePos[i-1];
				snake[i].setFillColor(sf::Color::Black);
				snake[i].setPosition(snakePos[i].x, snakePos[i].y);
			}
		
			// Switch the snake's head position based on the current direction.
			switch (curDirection) {
				case NORTH: snakePos[0].y -= SPEED; break;
				case EAST: 	snakePos[0].x += SPEED; break;
				case SOUTH: snakePos[0].y += SPEED; break;
				case WEST: 	snakePos[0].x -= SPEED; break;
			}
			clock.restart();
		}
			
		window.clear(sf::Color::White);
		
		snake[0].setPosition(snakePos[0].x, snakePos[0].y);
		snake[0].setFillColor(sf::Color::Black);
		window.draw(snake[0]);
	
		fillBorder(&window);
		
		food.setPosition(foodPos.x, foodPos.y);
		window.draw(food);
		
		for(int i = 1; i < snake.size(); ++i){
			window.draw(snake[i]);
		}
		
		window.display();
	}
}
