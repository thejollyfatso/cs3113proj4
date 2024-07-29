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
        if (!m_entity->m_face_forward) m_offset.x *= -1.0f;
    }
    else {
        m_scale = glm::vec3(1.0f);
        m_offset = glm::vec3(0.0f);
    }
}

bool Hitbox::isColliding(const Hitbox* other) const
{
    if (!other) return false;
    if (!other->m_entity->m_is_active) return false;

    // Calculate this hitbox bounds
    float thisLeft = m_position.x - m_scale.x / 2.0f;
    float thisRight = m_position.x + m_scale.x / 2.0f;
    float thisTop = m_position.y + m_scale.y / 2.0f;
    float thisBottom = m_position.y - m_scale.y / 2.0f;

    // Calculate other hitbox bounds
    float otherLeft = other->m_position.x - other->m_scale.x / 2.0f;
    float otherRight = other->m_position.x + other->m_scale.x / 2.0f;
    float otherTop = other->m_position.y + other->m_scale.y / 2.0f;
    float otherBottom = other->m_position.y - other->m_scale.y / 2.0f;

    // Check for overlap
    bool collisionX = thisLeft < otherRight && thisRight > otherLeft;
    bool collisionY = thisBottom < otherTop && thisTop > otherBottom;

    return collisionX && collisionY;
}

void Hitbox::checkCollisions(Hitbox* hurtboxes, int num_hurtboxes)
{
    // Check for collisions with other hurtboxes and set them to visible
	for (int i = 0; i < num_hurtboxes; ++i) {
		if (this != &hurtboxes[i] && isColliding(&hurtboxes[i]) && m_active && &hurtboxes[i].m_entity->m_is_active) {
			//hurtboxes[i].m_hidden = false;
			//this->m_hidden = false;
			hurtboxes[i].m_entity->deactivate(); // kill
		}
		else { this->m_hidden = true; hurtboxes[i].m_hidden = true; }
	}
}

// hurtbox updates which are passive awaiting hitbox check
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

// enemy update which only needs to check for player collision
void Hitbox::update(float delta_time, Hitbox* otherHitbox) {
    if (m_entity) {
        // Update the hitbox position based on the entity position and offset
        m_position = m_entity->get_position() + m_offset;

        // Update the model matrix with the new position
        m_model_matrix = glm::mat4(1.0f);
        m_model_matrix = glm::translate(m_model_matrix, m_position);
        m_model_matrix = glm::scale(m_model_matrix, m_scale);

        // Check for collision with the other hitbox
        if (isColliding(otherHitbox) && otherHitbox->m_entity->m_is_active && m_entity->m_is_active) {
            this->m_hidden = false;
            otherHitbox->m_hidden = false;
        }
		else { this->m_hidden = true; otherHitbox->m_hidden = true; }
    }
}

// player update which checks against enemy hurtboxes
void Hitbox::update(float delta_time, Hitbox* hurtboxes, int num_hurtboxes) {
    if (m_entity) {
        // Update the hitbox position based on the entity position and offset
        m_position = m_entity->get_position() + m_offset;

        // Update the model matrix with the new position
        m_model_matrix = glm::mat4(1.0f);
        m_model_matrix = glm::translate(m_model_matrix, m_position);
        m_model_matrix = glm::scale(m_model_matrix, m_scale);

        checkCollisions(hurtboxes, num_hurtboxes);
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

