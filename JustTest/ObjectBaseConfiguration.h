#pragma once

#include "Object.h"

Object * getBaseObject(ObjectType object_type) {
    switch (object_type)
    {
    case null:
        return new Object();
    case stone:
        return new Object();
    case hero:
        return new Object();
    case asteroid:
        return new Object
        (
            Point(),
            Point(),
            ObjectType::asteroid,
            CollisionType::asteroid_col,
            VisualInfo
            (
                SpriteType::asteroid_sprite,
                AnimationType::hold_anim,
                1000000000
            )
        );
    case turret:
        return new Object
        (
            Point(),
            Point(),
            ObjectType::turret,
            CollisionType::turret_col,
            VisualInfo
            (
                SpriteType::turret_sprite,
                AnimationType::hold_anim,
                1000000000
            )
        );
    case dome:
        return new Object
        (
            Point(),
            Point(),
            ObjectType::dome,
            CollisionType::dome_col,
            VisualInfo
            (
                SpriteType::dome_sprite,
                AnimationType::hold_anim,
                1000000000
            )
        );
    case science:
        return new Object
        (
            Point(),
            Point(),
            ObjectType::science,
            CollisionType::science_col,
            VisualInfo
            (
                SpriteType::science_sprite,
                AnimationType::hold_anim,
                1000000000
            )
        );
    case gold:
        return new Object
        (
            Point(),
            Point(),
            ObjectType::gold,
            CollisionType::gold_col,
            VisualInfo
            (
                SpriteType::gold_sprite,
                AnimationType::hold_anim,
                1000000000
            )
        );
    case bullet:
        return new Object
        (
            Point(),
            Point(),
            ObjectType::bullet,
            CollisionType::bullet_col,
            VisualInfo
            (
                SpriteType::bullet_sprite,
                AnimationType::hold_anim,
                1000000000
            )
        );
    case alien_fighter:
        return new Object
        (
            Point(),
            Point(),
            ObjectType::alien_fighter,
            CollisionType::alien_fighter_col,
            VisualInfo
            (
                SpriteType::alien_fighter_sprite,
                AnimationType::hold_anim,
                1000000000
            )
        );
    case alien_turret1:
        return new Object
        (
            Point(),
            Point(),
            ObjectType::alien_turret1,
            CollisionType::alien_turret1_col,
            VisualInfo
            (
                SpriteType::alien_turret1_sprite,
                AnimationType::hold_anim,
                1000000000
            )
        );
    case alien_gunship:
        return new Object
        (
            Point(),
            Point(),
            ObjectType::alien_gunship,
            CollisionType::alien_gunship_col,
            VisualInfo
            (
                SpriteType::alien_gunship_sprite,
                AnimationType::hold_anim,
                1000000000
            )
        );
    case alien_turret2:
        return new Object
        (
            Point(),
            Point(),
            ObjectType::alien_turret2,
            CollisionType::alien_turret2_col,
            VisualInfo
            (
                SpriteType::alien_turret2_sprite,
                AnimationType::hold_anim,
                1000000000
            )
        );
    case alien_bombard:
        return new Object
        (
            Point(),
            Point(),
            ObjectType::alien_bombard,
            CollisionType::alien_bombard_col,
            VisualInfo
            (
                SpriteType::alien_bombard_sprite,
                AnimationType::hold_anim,
                1000000000
            )
        );
    case alien_turret3:
        return new Object
        (
            Point(),
            Point(),
            ObjectType::alien_turret3,
            CollisionType::alien_turret3_col,
            VisualInfo
            (
                SpriteType::alien_turret3_sprite,
                AnimationType::hold_anim,
                1000000000
            )
        );
    case drone:
        return new Object();
    case drone_turret:
        return new Object();
    case rocket_launcher:
        return new Object();
    default:
        return nullptr;
    }
}