#pragma once

#include <chrono>

class Timer {
	std::chrono::high_resolution_clock::time_point time1 = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point time2 = std::chrono::high_resolution_clock::now();

	int current_frame = 0;
	bool is_frame_new = true;


public:


	void processTime() {
		time2 = time1;
		time1 = std::chrono::high_resolution_clock::now();
	}

	int getTimeDelay() {
		return std::chrono::duration_cast<std::chrono::microseconds>(time1 - time2).count();
	}

}timer;