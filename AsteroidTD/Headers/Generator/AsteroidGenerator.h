#pragma once

#include <vector>
#include <cmath>
#include <random>

#include "../Geometry/GeometryComponents.h"
#include "../Geometry/Noise.h"
#include "../Game/FactionList.h"

class AsteroidGenerator {
public:

	struct NPC_Metadata {
		FactionList faction = null_faction;

		enum npc_type {
			empty,
			defense,
			common
		} type = empty;
	};

private:

	std::vector<std::pair<Point, std::reference_wrapper<NPC_Metadata>>> point_arr;
	NPC_Metadata null_ref;
	std::map<std::pair<int, int>, NPC_Metadata> checked_grid;
	float radius, grid_size, random_radius, skip_chance;

	std::mt19937 r_eng;
	std::uniform_real_distribution<float> float_dist, angle_dist;

	static const int threshold = 50;

	Noise NPC_Metadata_dist;

	std::pair<int, int> squareToHex(Point p) {
		return{ int((p.x / sqrt(3) - (p.y / 3)) / grid_size), int(p.y / 3 * 2 / grid_size) };
	}

	Point getHexCenter(std::pair<int, int> p) {
		return Point(float(p.first) * sqrt(3) + float(p.second) * sqrt(3) / 2, float(p.second) * 3 / 2) * grid_size;
	}

	uint32_t getLength(std::pair<int, int> p) {
		return (abs(p.first) + abs(p.second) + abs(p.first + p.second)) / 2;
	}

public:

	AsteroidGenerator(float grid_size, float random_radius, float skip_chance, uint32_t NPC_Metadata_dist_seed = 0) :
		radius(radius),
		grid_size(grid_size / sqrt(3)),
		random_radius(random_radius),
		skip_chance(skip_chance),
		float_dist(0, 1),
		angle_dist(0, 360),
		r_eng(std::random_device()()),
		NPC_Metadata_dist(100, 1000, 0.5, 2, NPC_Metadata_dist_seed, 6)
	{}

	std::vector<std::pair<Point, std::reference_wrapper<NPC_Metadata>>> getAsteroidPositions() {
		return point_arr;
	}

	float getRadius() {
		return radius;
	}

	std::vector<std::pair<Point, std::reference_wrapper<NPC_Metadata>>> processFrame(Point camera_pos, float camera_radius) { // return new positions for asteroids
		std::vector<std::pair<Point, std::reference_wrapper<NPC_Metadata>>> output;
		std::pair<int, int> camera_hex_pos = squareToHex(camera_pos);
		int distance = int(camera_radius / grid_size);
		for (int x = -distance; x <= distance; x++) {
			for (int y = std::max(-distance, -distance - x); y <= std::min(distance, distance - x); y++) {
				std::pair<int, int> p = { x + camera_hex_pos.first, y + camera_hex_pos.second };
				if (checked_grid.find(p) == checked_grid.end()) {
					Point hex_center = getHexCenter(p);
					Point rand_vec = Point(random_radius, 0).getRotated(angle_dist(r_eng));
					NPC_Metadata NPC_Metadata_spec;
					if (float_dist(r_eng) > skip_chance) {
						int index = 0;
						int height = 0;
						NPC_Metadata_dist.getTerritorialDistribution(hex_center + rand_vec, threshold, index, height);
						if (height > threshold) {
							NPC_Metadata_spec.faction = FactionList(index + FactionList::alliance_of_ancient_knowledge);
						}
						for (int _x = -1; _x <= 1; _x++) {
							for (int _y = std::max(-1, -1 - _x); _y <= std::min(1, 1 - _x); _y++) {
								if (_x != 0 || _y != 0) {
									auto iter = checked_grid.find(std::make_pair(p.first + _x, p.second + _y));
									if (iter != checked_grid.end()) {

										if (NPC_Metadata_spec.faction == FactionList::null_faction) {
											if (iter->second.faction != FactionList::null_faction && iter->second.type != NPC_Metadata::npc_type::defense) {
												NPC_Metadata_spec.faction = iter->second.faction;
												NPC_Metadata_spec.type = NPC_Metadata::npc_type::defense;
												_x = 255; _y = 255;
											}
										}
										else {
											if (iter->second.faction == FactionList::null_faction) {
												iter->second.faction = NPC_Metadata_spec.faction;
												iter->second.type = NPC_Metadata::npc_type::defense;
											}
										}
									}
								}
							}
						}

						std::reference_wrapper<NPC_Metadata> NPC_Metadata_ref = (checked_grid[p] = NPC_Metadata_spec);
						point_arr.push_back({ hex_center + rand_vec, NPC_Metadata_ref });
						output.push_back({ hex_center + rand_vec, NPC_Metadata_ref });
					}
					else {
						checked_grid[p] = null_ref;
					}
				}
			}
		}
		return output;
	}
};
