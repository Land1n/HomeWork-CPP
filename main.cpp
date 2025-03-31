#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <conio.h>

constexpr int FIELD_WIDTH  = 30;
constexpr int FIELD_HEIGHT = 5;
constexpr int timeMs       = 1300;
constexpr int obstacles    = 3;

std::mutex mtx;


int update_position(int position[2], int sdvig)
{
	position[0] += sdvig;

	if(position[0] < 0) {
		position[0] += FIELD_WIDTH;
	};
	if(position[0] >= FIELD_WIDTH) {
		position[0] -= FIELD_WIDTH;
	};

	return position[0];
}

void update_position(int position[2])
{
	position[1] += 1;
}


// Функция отрисовки
void render_frame()
{
	// std::string pole[(FIELD_WIDTH+3)*FIELD_HEIGHT];
	std::cout << std::string(FIELD_WIDTH + 2, '-') << std::endl;
	for (int i = 0; i != FIELD_HEIGHT;i++){
		
		for (int j = 0; j != FIELD_WIDTH+2;j++)
		{
			if (j == 0 || j == FIELD_WIDTH+1) std::cout << '|';
			
			
			else std::cout << ' ';
		}
		std::cout << std::endl;
	}
	std::cout << std::string(FIELD_WIDTH + 2, '-') << std::endl;
	// for (int i = 0; i <= (FIELD_WIDTH+3)*FIELD_HEIGHT; i++){
	// 	if (i == 0 || i%31==0 || i%34==0){
	// 		std::cout << "|";
	// 	}
	// 	if (i != 0 && i%32==0){
	// 		std::cout << "\n";
	// 	}
	// 	std::cout << " ";
		
	// }
	// std::cout << std::string(FIELD_WIDTH + 2, '-') << "\n" << "|" ; 
	// for (int i = 0; i <= (FIELD_WIDTH+2)*FIELD_HEIGHT; i++){
	// 	if (i%31==0 && i != 0) {
	// 		std::cout << "|" << "\n";
	// 	}
	// 	else std::cout << " ";
	// }

	// std::cout << pole;
}

// 	std::cout << std::string(FIELD_WIDTH + 2, '-') << std::endl; 
	
// 	for(int j = 0;j <= 3;j++){
// 		if (j != 3){
// 			std::cout << "|";

// 			for(int i = 0; i < FIELD_WIDTH; i++) {
// 				for(int k = 0;k!=obstacles_quantity;k++){
// 					if (obstacles_array[k][0] == i && obstacles_array[k][1] == j) std::cout << "#";
// 					else std::cout << " ";
// 				}
// 			}
			
// 			std::cout << "|" << "\n";
// 		} else {
// 			std::cout << "|";
			
// 			for(int i = 0; i < FIELD_WIDTH; i++) {
// 				for(int k = 0;k!=obstacles_quantity;k++){
// 					if (obstacles_array[k][0] == i && obstacles_array[k][1] == j) std::cout << "#";
// 				}
// 				if(i == position_player) {
// 					std::cout << "■";
// 				}
// 				else {
// 					std::cout << " ";
// 				}
// 			}

// 			std::cout << "|" << "\n" << std::string(FIELD_WIDTH + 2, '-') << std::endl;
// 		}
// 	}
// }



void draw_loop(const bool& game, const int& sdvig, int obstacles_array[obstacles][2],int obstacles_quantity)
{
	int position = 0; // начальная позиция квадрата
	while(game) {
		mtx.lock();
		// std::cout << obstacles_quantity;  
		// position = update_position(position, sdvig);
		for (int i = 0; i != obstacles_quantity; i++)
			update_position(obstacles_array[i]);
		render_frame();
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

	std::srand(std::time(0));

	bool game{true};
	int shift{0}; //сдвиг
	int obstacles_array[obstacles][2];
	int obstacles_quantity{0};
	
	auto functionGeneration = [&game,&obstacles_array,&obstacles_quantity]() {
		while(game) {
			if (obstacles_quantity != obstacles){
				int start = 0;
				int end = 30;
				int x = rand() % (end - start + 1) + start;
				mtx.lock();
				obstacles_array[obstacles_quantity][0] = x;
				obstacles_array[obstacles_quantity][1] = -1;
				obstacles_quantity += 1;
				mtx.unlock();
				std::this_thread::sleep_for(std::chrono::milliseconds(timeMs/3));
			}
		};
	};

	auto functionDraw = [&game,&shift,&obstacles_array,&obstacles_quantity]() {
		draw_loop(game, shift, obstacles_array,obstacles_quantity);
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
	std::thread GenerationThread(functionGeneration); // поток для препятствий 
	std::thread drawThread(functionDraw); // поток для отрисовки персонажа
	std::thread cinThread(functionCin); // поток для ввода направления движения

	GenerationThread.join();
	drawThread.join();
	cinThread.join();

	return 0;
}