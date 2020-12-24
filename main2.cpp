#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <atomic>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include <termios.h>

#define STDIN_FILENO 0

#define KEY_UP 65
#define KEY_DOWN 66
#define KEY_RIGHT 67
#define KEY_LEFT 68

#define RESET       "\033[0m"
#define BLACK       "\033[30m"      
#define RED         "\033[31m"     
#define GREEN       "\033[32m"     
#define YELLOW      "\033[33m"     
#define BLUE        "\033[34m"     
#define MAGENTA     "\033[35m"     
#define CYAN        "\033[36m"     
#define WHITE       "\033[37m"     
#define BOLDBLACK   "\033[1m\033[30m"     
#define BOLDRED     "\033[1m\033[31m"     
#define BOLDGREEN   "\033[1m\033[32m"     
#define BOLDYELLOW  "\033[1m\033[33m"    
#define BOLDBLUE    "\033[1m\033[34m"     
#define BOLDMAGENTA "\033[1m\033[35m"    
#define BOLDCYAN    "\033[1m\033[36m"     
#define BOLDWHITE   "\033[1m\033[37m"      

#define EASY 300
#define MEDIUM 200
#define HARD 100

using namespace std;

class Page;
class Food;
class Snake;

//////////////////////////////////////////////////////////////////

class Food
{
public:
	Food(int x, int y, int val);
	bool is_existe(int x, int y);
	void change_coordinates(int x, int y);
	int get_x(){ return x;}
	int get_y(){ return y;}
	char get_shape(){ return shape;}
	int get_val(){ return value;}
	string get_color(){ return color;}

private:
	int x;
	int y;
	int value;
	char shape = '*';
	string color = BOLDYELLOW; 
};

Food::Food(int x, int y, int val)
:x(x)
,y(y)
,value(val)
{
}

bool Food::is_existe(int x, int y)
{
	if(this->x == x and this->y == y)
		return true;
	return false;
}

void Food::change_coordinates(int x, int y)
{
	this->x = x;
	this->y = y;
}

//////////////////////////////////////////////////////////////////

class Snake
{
public:
	Snake(int x, int y, string name, string color);
	bool check_crash_to_self_body();
	void move(string& move_type);
	void to_corruct_move_type(string& move_type);
	void add_new_head(string& move_type);
	void cut_tail();
	bool is_existe(int x, int y);
	void change_x_head(int x);
	void change_y_head(int y);
	void increase_size(int val);
	int get_score() {return score;}
	char get_shape() {return shape;}
	int get_size() {return coordinates.size();}
	int get_x_head() {return coordinates[0].first;}
	int get_y_head() {return coordinates[0].second;}
	string get_color() {return color;}
	string get_name() {return name;}
		
private:
	vector<pair<int, int>> coordinates;
	char shape = '+';
	int score = 0;
	int primitive_size = 5;
	int increase_lenght;
	string last_move = "left";
	string name;
	string color = BOLDBLUE;
};

Snake::Snake(int x, int y, string name, string color)
:color(color)
,name(name)
,increase_lenght(0)
{
	for(int i = 0; i < primitive_size; i++)
	{
		coordinates.push_back(pair<int, int>(x + i, y));
	}
}

void Snake::increase_size(int val)
{
	increase_lenght += val;
	score += val;
}

void Snake::change_x_head(int x)
{
	coordinates[0].first = x;
}

void Snake::change_y_head(int y)
{
	coordinates[0].second = y;
}

bool Snake::is_existe(int x, int y)
{
	for(int i = 0; i < coordinates.size(); i++)
	{
		if(x == coordinates[i].first and y == coordinates[i].second)
			return true;
	}
	return false; 
}

bool Snake::check_crash_to_self_body()
{
    int x_head_snake = get_x_head();
    int y_head_snake = get_y_head();

    for(size_t i = 1; i < coordinates.size(); i++)
    {
        if(x_head_snake == coordinates[i].first and y_head_snake == coordinates[i].second)
            return true;
    }
    return false;
}

void Snake::add_new_head(string& move_type)
{
	int x_head_snake = get_x_head();
    int y_head_snake = get_y_head();

    if(move_type == "left")
        coordinates.insert(coordinates.begin(), pair<int, int>(x_head_snake - 1, y_head_snake));

    if(move_type == "right")
        coordinates.insert(coordinates.begin(), pair<int, int>(x_head_snake + 1, y_head_snake));

    if(move_type == "up")
        coordinates.insert(coordinates.begin(), pair<int, int>(x_head_snake, y_head_snake - 1));

    if(move_type == "down")
        coordinates.insert(coordinates.begin(), pair<int, int>(x_head_snake, y_head_snake + 1));
}

void Snake::cut_tail()
{
	if(increase_lenght == 0)
		coordinates.pop_back();
	else
	{	
		if(increase_lenght != 0)
			increase_lenght --;
	}
}

void Snake::move(string& move_type)
{
	to_corruct_move_type(move_type);
	add_new_head(move_type);
	cut_tail();
}

void Snake::to_corruct_move_type(string& move_type)
{
    // RIGHT
    if(move_type == "left" and last_move == "right")
    {
        move_type = "right";
        return;
    }

    // LEFT
    if(move_type == "right" and last_move == "left")
    {
        move_type = "left";
        return;
    }

    // DOWN
    if(move_type == "up" and last_move == "down")
    {
        move_type = "down";
        return;
    }

    // UP
    if(move_type == "down" and last_move == "up")
    {
        move_type = "up";
        return;
    }
    last_move = move_type;
}

//////////////////////////////////////////////////////////////////

class Page
{
public:
	Page(Snake *snake, Food *food);
	void print();
	void check_eat_food();
	void message_game_over(string player);
	void game_over(string player, atomic<bool>& end_game, thread& thread_for_read_input);
	void insert_food(); 
	int get_lenght(){ return lenght;}
	int get_width(){ return width;}
	bool move_once(string& move_type);
	void check_crash_wall();	

private:
	Snake *snake = nullptr;
	Food *food = nullptr;
	int lenght = 30;
	int width = 20;
	char margins_shape = '#';
	char empty_shape = '.';
};

Page::Page(Snake *snake, Food *food)
:snake(snake),food(food)
{
}	 

bool Page::move_once(string& move_type)
{
	snake->move(move_type);	
	check_crash_wall();
	check_eat_food();
	return snake->check_crash_to_self_body();
}

void Page::check_eat_food()
{
	if(snake->get_x_head() == food->get_x() and snake->get_y_head() == food->get_y())
	{	
		snake->increase_size(food->get_val());
		insert_food();
	}
}

void Page::check_crash_wall()
{
    int margins_up = 0;
    int margins_down = width - 1;
    int margins_left = 0;
    int margins_right = lenght - 1;

    // Wall left
    if(snake->get_x_head() == margins_left)
		snake->change_x_head(margins_right - 1);

    // Wall right
    if(snake->get_x_head() == margins_right)
		snake->change_x_head(margins_left + 1);

    // Wall up
    if(snake->get_y_head() == margins_up)
		snake->change_y_head(margins_down - 1);

    // Wall down
    if(snake->get_y_head() == margins_down)
		snake->change_y_head(margins_up + 1);
}

void Page::print()
{
	system("clear");
	cout << snake->get_color() << snake->get_name() << RESET << endl;
	cout << snake->get_color() << "score: " << snake->get_score() << RESET << endl;

	for(int y = 0; y < width; y++)
	{
		for(int x = 0; x < lenght; x++)
		{
			bool print_empty = true;
			// Margins
			if(x == 0 or y == 0 or x == lenght - 1 or y == width - 1)
			{
				cout << margins_shape << " ";
				continue;
			}

			// Snake	
			if(snake->is_existe(x, y))
			{	
				cout << snake->get_color() << snake->get_shape() << RESET << " ";			
				continue;
			}

			// Food
			if(food->is_existe(x, y))
			{	
				cout << food->get_color() << food->get_shape() << RESET << " ";			
				continue;
			}

			cout << empty_shape << " ";
		}
		cout << endl;
	}
}

void Page::insert_food() 
{
	while(true)
	{
		int x_food = rand() % (lenght - 3) + 2;
		int y_food = rand() % (width - 3) + 2;
						
		if(!snake->is_existe(x_food, y_food))
		{	
			food->change_coordinates(x_food, y_food);
			break;
		}
	}				
}

void Page::message_game_over(string player)
{
    cout << endl;
    if(player == "EQUAL")
        cout << "\t\t\tEQUAL GAME\t\t\t" << endl;
    else     
        cout << "\t\t\t" << player << " GAME OVER\t\t\t" << endl;
}

void Page::game_over(string player, atomic<bool>& end_game, thread& thread_for_read_input)
{
    message_game_over(player);
    end_game = true;
    thread_for_read_input.join();
}

//////////////////////////////////////////////////////////////////

void change_move_type_player1(string& move_type, char input_move_type)
{
    if(input_move_type == KEY_UP)
        move_type = "up";

    if(input_move_type == KEY_DOWN)
        move_type = "down";

    if(input_move_type == KEY_LEFT)
        move_type = "left";

    if(input_move_type == KEY_RIGHT)
        move_type = "right";
}

void read_input(string& move_type1, atomic<bool>& END_GAME)
{
    while(!END_GAME)
    {
        // Black magic to prevent Linux from buffering keystrokes.
        struct termios t;
        tcgetattr(STDIN_FILENO, &t);
        t.c_lflag &= ~ICANON;
        tcsetattr(STDIN_FILENO, TCSANOW, &t);


        char input_char1, input_char2, input_char3;
        cin >> input_char1;

        if(input_char1 == 27)
        {
            cin >> input_char2;
            cin >> input_char3;
            if (input_char2 == 91) 
                change_move_type_player1(move_type1, input_char3);
        }
    }
}

int main()
{
	srand(time(0));	

    // Setting
	atomic<bool> END_GAME {false};
	string move_type_player1 = "left";
	int LEVEL = EASY;


	Snake snake(15, 5, "SOROOSH", BOLDBLUE);
	Food food(15, 10, 1);
	Page page(&snake, &food);
	
	page.print();

	thread thread_for_read_input(read_input, ref(move_type_player1), ref(END_GAME));

	while(true)
	{
        this_thread::sleep_for(chrono::milliseconds(LEVEL));
		if(page.move_once(move_type_player1))
		{
	       	page.game_over(snake.get_name(), END_GAME, thread_for_read_input);
            return 0;
		}
		page.print();	
	}	
}
