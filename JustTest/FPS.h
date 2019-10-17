#pragma once

#include <chrono>

class FPS {
	// FPS counter
	int frame_stamp = 0;
	std::chrono::time_point<std::chrono::system_clock> time_stamp;
	int fps = 240;
	int update_time = 100;  // in milliseconds
	const float percent = 0.2;

public:

	FPS() {
		time_stamp = std::chrono::system_clock::now();
	}

	void processFrame(int frame_num) {
		std::chrono::time_point<std::chrono::system_clock> cur_time_stamp = std::chrono::system_clock::now();
		float time_step = std::chrono::duration_cast<std::chrono::milliseconds>(cur_time_stamp - time_stamp).count();
		if (time_step >= update_time) {
			fps = fps * percent + ((frame_num - frame_stamp) * (1000.0 / time_step)) * (1 - percent);
			time_stamp = cur_time_stamp;
			frame_stamp = frame_num;
			//std::cout << "FPS >> " << fps << std::endl;
		}
	}

	float getFPS() {
		return std::max(fps, 1);
	}
}fps;