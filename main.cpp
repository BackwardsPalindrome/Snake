#include <iostream>
#include <cstdlib>
#include <ctime>
#include <list>
#include <ncurses.h>

#define TIME_OUT 200

typedef struct point {
	int y, x;
} Point;

typedef struct fruit {
	Point pos;
	bool present = false;
} Fruit;

typedef struct snake {
	Point head;
	std::list<Point> body;
} Snake;

bool operator==(const Point& lhs, const Point& rhs) {
	return lhs.y == rhs.y && lhs.x == rhs.x;
}

void placeFruit(Fruit*);
void moveSnake(Snake&, Fruit&, int);
bool snakeCollision(Snake&);
int setSpeed(int);

int main() {
	initscr();	// Enter curses mode
	cbreak();
	noecho();
	keypad(stdscr, true);
	curs_set(0); // Hide cursor

	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);

	std::srand(std::time(nullptr));

	std::string msg = "Game Over...\n";
	std::string score = "Score: ";
	int direction, ch;
	direction = -1;

	Snake snake;
	snake.head = {(LINES - 1) / 2, (COLS - 1) / 2};
	Fruit fruit;

	mvaddch(snake.head.y, snake.head.x, ACS_BULLET);
	while((ch = getch()) != KEY_F(1)) {
		timeout(setSpeed(snake.body.size()));
		switch(ch) {
			case 'w':
			case KEY_UP:
				direction = 0;
			break;
			case 'a':
			case KEY_LEFT:
				direction = 1;
			break;
			case 's':
			case KEY_DOWN:
				direction = 2;
			break;
			case 'd':
			case KEY_RIGHT:
				direction = 3;
			break;
		}
		moveSnake(snake, fruit, direction);
		// make sure fruit is still in the window
		if (fruit.pos.y >= LINES || fruit.pos.x >= COLS)
			fruit.present = false;
		if (!fruit.present)
			placeFruit(&fruit);
		if (snakeCollision(snake))
			break;
		refresh();
	}
	clear();
	mvprintw(LINES/2, (COLS - msg.length())/2, "%s", msg.c_str());
	mvprintw(LINES/2 + 1, (COLS - msg.length())/2, "%s %lu", score.c_str(), snake.body.size());
	timeout(-1);
	refresh();
	getch();

	endwin();
	return 0;
}

void placeFruit(Fruit *fruit) {
	fruit->pos.y = std::rand()/((RAND_MAX + 1u)/LINES);
	fruit->pos.x = std::rand()/((RAND_MAX + 1u)/COLS);
	attron(COLOR_PAIR(1));
	mvaddch(fruit->pos.y, fruit->pos.x, ACS_DIAMOND);
	attroff(COLOR_PAIR(1));
	fruit->present = true;
}

void moveSnake(Snake &snake, Fruit &fruit, int direction) {
	snake.body.push_back(snake.head);
	switch(direction) {
		case 0:
			if (snake.head.y == 0)
				snake.head.y = LINES;
			else
				--snake.head.y;
			mvaddch(snake.head.y, snake.head.x, ACS_UARROW);
		break;
		case 1:
			if (snake.head.x == 0)
				snake.head.x = COLS;
			else
				--snake.head.x;
			mvaddch(snake.head.y, snake.head.x, ACS_LARROW);
		break;
		case 2:
			if (snake.head.y == LINES - 1)
				snake.head.y = 0;
			else
				++snake.head.y;
			mvaddch(snake.head.y, snake.head.x, 'v');
		break;
		case 3:
			if (snake.head.x == COLS - 1)
				snake.head.x = 0;
			else
				++snake.head.x;
			mvaddch(snake.head.y, snake.head.x, ACS_RARROW);
		break;
	}
	if (snake.head == fruit.pos) {
		fruit.present = false;
		snake.body.push_back(snake.body.back());
	}

	mvaddch(snake.body.front().y, snake.body.front().x, ' ');
	if (snake.body.size() > 1)
		mvaddch(snake.body.back().y, snake.body.back().x, ACS_BULLET);
	snake.body.pop_front();
}

bool snakeCollision(Snake &snake) {
	for (auto it = snake.body.begin(); it != snake.body.end(); ++it)
		if (snake.head == *it)
			return true;
	return false;
}

int setSpeed(int length) {
	if (TIME_OUT - 10 * length > 20)
		return TIME_OUT - 10 * length;
	return 20;
}
