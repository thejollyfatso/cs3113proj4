#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Hitbox.h"
#include "Entity.h"

Hitbox::Hitbox()
    : m_red(1.0f), m_green(1.0f), m_blue(1.0f), m_opacity(1.0f), m_offset(0.0f), m_entity(nullptr) {
    m_model_matrix = glm::mat4(1.0f);
    m_position = glm::vec3(0);
}

Hitbox::Hitbox(GLuint texture_id, Entity* entity)
    : m_texture_id(texture_id), m_red(1.0f), m_green(0.1f), m_blue(0.1f), m_opacity(1.0f), m_offset(0.0f), m_entity(entity) {
    m_model_matrix = glm::mat4(1.0f);
    if (m_entity) {
        m_position = m_entity->get_position();
    }
    else {
        m_position = glm::vec3(0);
    }
}

void Hitbox::set_entity(Entity* entity) {
    m_entity = entity;
}

void Hitbox::update(float delta_time) {
    if (m_entity) {
        // Update the hitbox position based on the entity position and offset
        m_position = m_entity->get_position() + m_offset;

        // Update the model matrix with the new position
        m_model_matrix = glm::mat4(1.0f);
        m_model_matrix = glm::translate(m_model_matrix, m_position);
    }
}

void Hitbox::render(ShaderProgram* program) {
    program->set_model_matrix(m_model_matrix);

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };
    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    program->set_colour(m_red, m_green, m_blue, m_opacity);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
}

