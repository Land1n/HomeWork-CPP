#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <conio.h>

constexpr int FIELD_WIDTH  = 30;
constexpr int FIELD_HEIGHT = 5;
constexpr int timeMs       = 100;
constexpr int obstacles    = 3;

std::mutex mtx;

struct object
{
	const char symbol[4];
	int x;
	int y;
};


int update_position(int x,int sdvig)
{
	x += sdvig;
	if(x < 0) {
		x += FIELD_WIDTH;
	};
	if(x >= FIELD_WIDTH) {
		x -= FIELD_WIDTH;
	};
	return x;
}

int update_position(int y)
{
	y += 1;
	return y;
}


// Функция отрисовки
void render_frame(const object object_array[obstacles+1])
{
	std::cout << std::string(FIELD_WIDTH + 2, '-') << std::endl;
	for (int i = 0; i != FIELD_HEIGHT;i++){
		std::cout << "|";
		for (int j = 0; j < FIELD_WIDTH;j++)
		{
			std::string symbol = " ";
			for (int k = 0; k != obstacles+1; k++ ){
				if (object_array[k].x == j && object_array[k].y == i) symbol = object_array[k].symbol;
			}
			std::cout << symbol;

		}
		std::cout << "|";
		std::cout << std::endl;
	}
	std::cout << std::string(FIELD_WIDTH + 2, '-') << std::endl;
}

void draw_loop(const bool& game, const int& sdvig,object object_array[obstacles+1] )
{

	while(game) {
		mtx.lock();
		for (int i = 0; i != obstacles+1; i++){
			if (i == 0)
			{
				int x = update_position(object_array[i].x,sdvig);
				object_array[i].x = x;
			}
			else {
				int y = update_position(object_array[i].y);
				object_array[i].y = y;
			}
		}
		render_frame(object_array);
		mtx.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(timeMs));
	}
}


void handle_input(int code, int& shift)
{
	if(code == 97) {
		shift = -1;
	}
	else if(code == 100) {
		shift = 1;
	}
}

int main()
{

	bool game{true};
	int shift{0}; //сдвиг
	object player = {"■", 0, FIELD_HEIGHT-1};
	object p = {"#", 10, -1};
	object object_array[obstacles+1] = {player,p};
	int obstacles_quantity{1};
	
	// auto functionGeneration = [&game,&object_array,&obstacles_quantity]() {
	// 	while(game) {
	// 		if (obstacles_quantity != obstacles){
	// 			int start = 1;
	// 			int end = 30;
	// 			std::srand(std::time(0));
	// 			int x = rand() % (end - start + 1) + start;
	// 			mtx.lock();
	// 			object obstacles = {"#",x,0};
	// 			object_array[obstacles_quantity] = obstacles;
	// 			obstacles_quantity += 1;
	// 			mtx.unlock();
	// 			std::this_thread::sleep_for(std::chrono::milliseconds(timeMs/3));
	// 		}
	// 	};
	// };

	auto functionDraw = [&game,&shift,&object_array]() {
		draw_loop(game, shift,object_array);
	};


	auto functionCin = [&game,&shift]() {
		char input;
		int code;
		while(game) {
			input = getch();
			code =  static_cast<int>(input);
			mtx.lock();
			if(code == 99) {
				game = false;
				break; //игра закончена
			}
			handle_input(code, shift);
			mtx.unlock();
		}
	};
	// std::thread GenerationThread(functionGeneration); // поток для препятствий 
	std::thread drawThread(functionDraw); // поток для отрисовки персонажа
	std::thread cinThread(functionCin); // поток для ввода направления движения

	// GenerationThread.join();
	drawThread.join();
	cinThread.join();

	return 0;
}