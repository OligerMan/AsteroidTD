#pragma once

enum EffectList {
	null_effect,
	damage_buff,
	attack_speed_buff,
	fire_debuff,
	regen_buff,
	const_heal,
	move_speed_buff,

	EFFECT_NUM
};

struct Effect {
	int time = 0;
	EffectList effect;

	Effect(int time, EffectList effect) : time(time), effect(effect) {}
};