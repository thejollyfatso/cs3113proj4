#ifndef HITBOX_H
#define HITBOX_H

#include "glm/glm.hpp"
#include "ShaderProgram.h"
#include "Entity.h"  // Include the Entity class to link with Hitbox

class Hitbox {
public:
    glm::vec3 m_position;
    glm::vec3 m_offset;  // Offset from the linked entity

    float m_red, m_green, m_blue, m_opacity;  // Color and opacity

    glm::mat4 m_model_matrix;

    Entity* m_entity;  // Pointer to the linked entity

    Hitbox();
    Hitbox(Entity* entity);

    void set_entity(Entity* entity);  
    void update(float delta_time);  
    void render(ShaderProgram* program);
};

#endif

