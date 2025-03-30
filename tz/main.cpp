#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>


constexpr int FIELD_WIDTH = 30;
constexpr int timeMs      = 300;
std::mutex mtx;


int update_position(int position, int sdvig)
{
	position += sdvig;

	if(position < 0) {
		position += FIELD_WIDTH;
	};
	if(position >= FIELD_WIDTH) {
		position -= FIELD_WIDTH;
	};

	return position;
}

// Функция отрисовки
void render_frame(int position)
{
	std::cout << std::string(FIELD_WIDTH + 2, '-') << "\n" << "|";

	for(int i = 0; i < FIELD_WIDTH; i++) {
		if(i == position) {
			std::cout << "■";
		}
		else {
			std::cout << " ";
		}
	}

	std::cout << "|" << "\n" << std::string(FIELD_WIDTH + 2, '-') << std::endl;
}


void draw_loop(const bool& game, const int& sdvig)
{
	int position = 0; // начальная позиция квадрата
	while(game) {
		mtx.lock();
		position = update_position(position, sdvig);
		render_frame(position);
		mtx.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(timeMs));
	}
}

void handle_input(char input, int& shift)
{
	if(input == 'a') {
		shift = -1;
	}
	else if(input == 'd') {
		shift = 1;
	}
}

int main()
{
	bool game{true};
	int shift{0}; //сдвиг

	auto functionDraw = [&game,&shift]() {
		draw_loop(game, shift);
	};

	auto functionCin = [&game,&shift]() {
		char input;
		while(game) {
			std::cin >> input;

			mtx.lock();
			if(input == 'c') {
				game = false;
				break; //игра закончена
			}
			handle_input(input, shift);

			mtx.unlock();
		}
	};

	std::thread drawThread(functionDraw); // поток для отрисовки персонажа
	std::thread cinThread(functionCin); // поток для ввода направления движения

	drawThread.join();
	cinThread.join();

	return 0;
}