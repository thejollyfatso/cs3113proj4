#ifndef HITBOX_H
#define HITBOX_H

#include <unordered_map>
#include <string>
#include "glm/glm.hpp"
#include "ShaderProgram.h"
#include "Entity.h"  // Include the Entity class to link with Hitbox

class Entity;

class Hitbox {
public:
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_offset;  // Offset from the linked entity
    std::unordered_map<std::string, std::pair<glm::vec3, glm::vec3>> key_value_store; // !! keys assumed to match animation name in entity

    glm::mat4 m_model_matrix;
    GLuint    m_texture_id;

    bool m_hidden = true;
    bool m_active = false; // for active hitframes

    Entity* m_entity;  // Pointer to the linked entity

    Hitbox();
    Hitbox(GLuint texture_id, Entity* entity);

    void set_entity(Entity* entity);  
    void set_hidden(bool hide);
    void set_scale(float x_scale, float y_scale);
    void set_offset(float x_offset, float y_offset);
    void add_hitdata(const std::string& key, const glm::vec3& scale, const glm::vec3& offset);
    void set_hitdata(const std::string& key);

    void update(float delta_time);  
    void render(ShaderProgram* program);
};

#endif

