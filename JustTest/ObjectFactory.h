#pragma once

#include <vector>

#include "Object.h"

class ObjectFactory {
public:

    enum RecipeType {
        base_object_recipe,

        RECIPE_COUNT
    };

    struct ObjectRecipe {
        ObjectType type;
        RecipeType recipe;

        ObjectRecipe(ObjectType type) : type(type), recipe(base_object_recipe) {}
        ObjectRecipe(RecipeType type) : recipe(type), type(ObjectType::null) {}
    };

private:
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
            return new Object
            (
                Point(),
                Point(),
                ObjectType::drone,
                CollisionType::drone_col,
                VisualInfo
                (
                    SpriteType::drone_sprite,
                    AnimationType::hold_anim,
                    1000000000
                )
            );
        case drone_turret:
            return new Object
            (
                Point(),
                Point(),
                ObjectType::drone_turret,
                CollisionType::drone_turret_col,
                VisualInfo
                (
                    SpriteType::drone_turret_sprite,
                    AnimationType::hold_anim,
                    1000000000
                )
            );
        case rocket_launcher:
            return new Object();
        default:
            return nullptr;
        }
    }

    Object * getRecipeObject(RecipeType recipe) {
        switch (recipe) {
        default:
            return nullptr;
        }
    }

public:
    
    Object * getObject(std::vector<ObjectRecipe> recipe_list) {
        if (!recipe_list.empty()) {
            Object * output;
            Object * current_object;

            if (recipe_list[0].recipe == RecipeType::base_object_recipe) {
                output = getBaseObject(recipe_list[0].type);
            }
            else {
                output = getRecipeObject(recipe_list[0].recipe);
            }
            current_object = output;

            for (int i = 1; i < recipe_list.size(); i++) {
                if (recipe_list[0].recipe == RecipeType::base_object_recipe) {
                    current_object->setDieObject(getBaseObject(recipe_list[i].type));
                }
                else {
                    current_object->setDieObject(getRecipeObject(recipe_list[i].recipe));
                }
                current_object = current_object->getDieObject();
            }

            return output;
        }
        return nullptr;
    }

} object_factory;