#ifndef HITBOX_H
#define HITBOX_H

#include "glm/glm.hpp"
#include "ShaderProgram.h"
#include "Entity.h"  // Include the Entity class to link with Hitbox

class Hitbox {
public:
    glm::vec3 m_position;
    glm::vec3 m_offset;  // Offset from the linked entity

    glm::mat4 m_model_matrix;
    GLuint    m_texture_id;

    bool m_hidden = true;

    Entity* m_entity;  // Pointer to the linked entity

    Hitbox();
    Hitbox(GLuint texture_id, Entity* entity);

    void set_entity(Entity* entity);  
    void set_hidden(bool hide);
    void update(float delta_time);  
    void render(ShaderProgram* program);
};

#endif

