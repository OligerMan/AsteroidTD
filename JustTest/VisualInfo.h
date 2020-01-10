#pragma once

enum SpriteType {
	null_sprite,
	stone_sprite,
	hero_sprite,
	asteroid_sprite,
	redactor_back_sprite,
	turret_sprite,
	dome_sprite,
	science_sprite,
	gold_sprite,
	bullet_sprite,

	alien_fighter_sprite,
	alien_turret1_sprite,
	alien_gunship_sprite,
	alien_turret2_sprite,

	asteroid_gold_interspersed_sprite,
	asteroid_iron_interspersed_sprite,
	asteroid_suspiciously_flat_sprite,

	asteroid_strange_cracked_sprite,
	asteroid_ordinary_wealthy_sprite,
	asteroid_poor_mountainous_sprite,
	asteroid_wealthy_cracked_sprite,
	asteroid_ordinary_mountainous_sprite,
	asteroid_strange_poor_sprite,

	asteroid_swampy_with_gold_mines_sprite,
	asteroid_unstable_explosive_ore_sprite,
	asteroid_old_laboratory_sprite,
	asteroid_lava_surface_sprite,

	asteroid_drone_factory_sprite,
	asteroid_rocket_launcher_sprite,
	asteroid_ancient_laboratory_sprite,
	asteroid_ancient_giant_gold_mine_sprite,

	drone_sprite,
	drone_turret_sprite,
	rocket_launcher_sprite,
	rocket_sprite,

	COUNT_SPRITE
};

std::vector<std::string> sprite_type;

void sprite_type_init() {
	sprite_type.resize(COUNT_SPRITE);

	sprite_type[null_sprite] = "null";
	sprite_type[stone_sprite] = "stone";
	sprite_type[hero_sprite] = "hero";
	sprite_type[asteroid_sprite] = "asteroid";
	sprite_type[redactor_back_sprite] = "redactor_back";
	sprite_type[turret_sprite] = "turret";
	sprite_type[dome_sprite] = "dome";
	sprite_type[science_sprite] = "science";
	sprite_type[gold_sprite] = "gold"; 
	sprite_type[bullet_sprite] = "bullet";
	sprite_type[alien_fighter_sprite] = "alien_fighter";
	sprite_type[alien_turret1_sprite] = "alien_turret1";
	sprite_type[alien_gunship_sprite] = "alien_gunship";
	sprite_type[alien_turret2_sprite] = "alien_turret2";
	sprite_type[asteroid_gold_interspersed_sprite] = "asteroid_gold_interspersed";
	sprite_type[asteroid_iron_interspersed_sprite] = "asteroid_iron_interspersed";
	sprite_type[asteroid_suspiciously_flat_sprite] = "asteroid_suspiciously_flat";
	sprite_type[asteroid_strange_cracked_sprite] = "asteroid_strange_cracked";
	sprite_type[asteroid_ordinary_wealthy_sprite] = "asteroid_ordinary_wealthy";
	sprite_type[asteroid_poor_mountainous_sprite] = "asteroid_poor_mountainous";
	sprite_type[asteroid_wealthy_cracked_sprite] = "asteroid_wealthy_cracked";
	sprite_type[asteroid_ordinary_mountainous_sprite] = "asteroid_ordinary_mountainous";
	sprite_type[asteroid_strange_poor_sprite] = "asteroid_strange_poor";
	sprite_type[asteroid_swampy_with_gold_mines_sprite] = "asteroid_swampy_with_gold_mines";
	sprite_type[asteroid_unstable_explosive_ore_sprite] = "asteroid_unstable_explosive_ore";
	sprite_type[asteroid_old_laboratory_sprite] = "asteroid_old_laboratory";
	sprite_type[asteroid_lava_surface_sprite] = "asteroid_lava_surface";
	sprite_type[asteroid_drone_factory_sprite] = "asteroid_drone_factory";
	sprite_type[asteroid_rocket_launcher_sprite] = "asteroid_rocket_launcher";
	sprite_type[asteroid_ancient_laboratory_sprite] = "asteroid_ancient_laboratory";
	sprite_type[asteroid_ancient_giant_gold_mine_sprite] = "asteroid_ancient_giant_gold_mine";
	sprite_type[drone_sprite] = "drone";
	sprite_type[drone_turret_sprite] = "drone_turret";
	sprite_type[rocket_launcher_sprite] = "rocket_launcher";
	sprite_type[rocket_sprite] = "rocket";
}

int is_sprite_type_exists(std::string example) {
	for (int i = 0; i < COUNT_SPRITE; i++) {
		if (sprite_type[i] == example) {
			return i;
		}
	}
	return -1;
}

enum AnimationType {
	null_anim,
	hold_anim,
	move_anim,
	selected_anim,

	COUNT_ANIMATION
};

std::vector<std::string> animation_type;

void animation_type_init() {
	animation_type.resize(COUNT_ANIMATION);

	animation_type[null_anim] = "null";
	animation_type[hold_anim] = "hold";
	animation_type[move_anim] = "move";
	animation_type[selected_anim] = "selected";
}

int is_animation_type_exists(std::string example) {
	for (int i = 0; i < COUNT_ANIMATION; i++) {
		if (animation_type[i] == example) {
			return i;
		}
	}
	return -1;
}

struct VisualInfo {
	SpriteType object_spr_type = SpriteType::null_sprite;
	AnimationType animation_type = AnimationType::null_anim;
	int frame_num = 0;
	int frame_duration = 1;

	VisualInfo() {}

	VisualInfo(SpriteType sprite_type, AnimationType animation_type, int frame_duration) : frame_duration(frame_duration), object_spr_type(sprite_type), animation_type(animation_type) {}
};