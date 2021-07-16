#pragma once

#include <vector>

#include "GeometryComponents.h"


double heuristic(Point a, Point b) {
	return abs((a - b).x) + abs((a - b).y);
}

std::vector<Point> getAStarPath(std::vector<std::vector<int>> map, Point start, Point end) {   // in map index 0 means not visited, 1 blocked, 2 visited
	std::vector<std::vector<Point>> came_from;
	came_from.resize(map.size());
	for (int i = 0; i < map.size(); i++) {
		came_from[i].resize(map[i].size());
	}

	std::vector<std::vector<double>> cost_so_far;
	cost_so_far.resize(map.size());
	for (int i = 0; i < map.size(); i++) {
		cost_so_far[i].resize(map[i].size(), 1000000);
	}

	std::vector<std::pair<Point, int>> queue;
	queue.push_back(std::pair<Point, int>(start, 0));
	cost_so_far[start.x][start.y] = 0;

	while (!queue.empty()) {
		Point current;
		int max_prior = 0;
		for (int i = 0; i < queue.size(); i++) {
			if (queue[max_prior].second > queue[i].second) {
				max_prior = i;
			}
		}
		current = queue[max_prior].first;
		if (current == end) {
			queue.clear();
			break;
		}
		queue.erase(queue.begin() + max_prior);
		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				if (i != 0 || i != 0) {
					double new_cost = cost_so_far[current.x][current.y] + 1;
					if (map[current.x + i][current.y + j] == 0 && (new_cost < cost_so_far[current.x + i][current.y + j])) {
						double priority = heuristic(end, Point(current.x + i, current.y + j));
						queue.push_back(std::pair<Point, int>(Point(current.x + i, current.y + j), priority));
						came_from[current.x + i][current.y + j] = current;
						cost_so_far[current.x + i][current.y + j] = new_cost;
						map[current.x + i][current.y + j] = 2;
					}
				}
			}
		}
	}

	std::vector<Point> answer;
	answer.push_back(end);
	while (answer[answer.size() - 1] != start && answer[answer.size() - 1] != Point()) {
		answer.push_back(came_from[answer[answer.size() - 1].x][answer[answer.size() - 1].y]);
	}
	if (answer[answer.size() - 1] == Point()) {
		answer.clear();
	}
	if (answer.size() > 3) {
		answer.erase(answer.begin());
		answer.pop_back();
	}

	return answer;
}