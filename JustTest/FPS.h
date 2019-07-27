#pragma once

#include <chrono>

class FPS {
	// FPS counter
	int frame_stamp = 0;
	std::chrono::time_point<std::chrono::system_clock> time_stamp;
	int fps = 240;

public:

	FPS() {
		time_stamp = std::chrono::system_clock::now();
	}

	void processFrame(int frame_num) {
		std::chrono::time_point<std::chrono::system_clock> cur_time_stamp = std::chrono::system_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(cur_time_stamp - time_stamp).count() >= 500) {
			fps = fps * 0.7 + ((frame_num - frame_stamp) * 2) * 0.3;
			time_stamp = cur_time_stamp;
			frame_stamp = frame_num;
		}
	}

	float getFPS() {
		return fps;
	}
}fps;