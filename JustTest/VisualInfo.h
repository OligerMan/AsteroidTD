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
	attack1_anim,
	attack2_anim, 
	attack3_anim,
	selected_anim,


	COUNT_ANIMATION
};

std::vector<std::string> animation_type;

void animation_type_init() {
	animation_type.resize(COUNT_ANIMATION);

	animation_type[null_anim] = "null";
	animation_type[hold_anim] = "hold";
	animation_type[move_anim] = "move";
	animation_type[attack1_anim] = "attack1";
	animation_type[attack2_anim] = "attack2";
	animation_type[attack3_anim] = "attack3";
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