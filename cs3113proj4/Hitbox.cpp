#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Hitbox.h"
#include "Entity.h"

Hitbox::Hitbox()
    : m_offset(0.0f), m_entity(nullptr) {
    m_model_matrix = glm::mat4(1.0f);
    m_position = glm::vec3(0);
    m_scale = glm::vec3(1.0f);
}

Hitbox::Hitbox(GLuint texture_id, Entity* entity)
    : m_texture_id(texture_id), m_offset(0.0f), m_entity(entity) {
    m_model_matrix = glm::mat4(1.0f);
    m_scale = glm::vec3(1.0f);
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

void Hitbox::set_hidden(bool hide)
{
    m_hidden = hide;
}

void Hitbox::set_scale(float x_scale, float y_scale)
{
    m_scale.x = x_scale;
    m_scale.y = y_scale;
}

void Hitbox::set_offset(float x_offset, float y_offset)
{
    m_offset.x = x_offset;
    m_offset.y = y_offset;
}

void Hitbox::add_hitdata(const std::string& key, const glm::vec3& scale, const glm::vec3& offset)
{
    key_value_store[key] = std::make_pair(scale, offset);
}

void Hitbox::set_hitdata(const std::string& key)
{
    auto hitdata = key_value_store.find(key);
    if (hitdata != key_value_store.end()) {
        m_scale = hitdata->second.first;
        m_offset = hitdata->second.second;
    }
    else {
        m_scale = glm::vec3(1.0f);
        m_offset = glm::vec3(0.0f);
    }
}

void Hitbox::update(float delta_time) {
    if (m_entity) {
        // Update the hitbox position based on the entity position and offset
        m_position = m_entity->get_position() + m_offset;

        // Update the model matrix with the new position
        m_model_matrix = glm::mat4(1.0f);
        m_model_matrix = glm::translate(m_model_matrix, m_position);
        m_model_matrix = glm::scale(m_model_matrix, m_scale);
    }
}

void Hitbox::render(ShaderProgram* program) {
    if (!m_hidden)
    { 
		program->set_model_matrix(m_model_matrix);

		float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
		float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };
		glBindTexture(GL_TEXTURE_2D, m_texture_id);

		glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program->get_position_attribute());
		glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
		glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program->get_position_attribute());
	}
}

