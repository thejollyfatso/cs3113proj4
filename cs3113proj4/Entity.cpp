#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"

void Entity::ai_activate(Entity* player) {
    switch (m_ai_type) {
    case WALKER:
        //ai_walk();
        ai_walk(3.0f, 5.0f);
        break;
    case GUARD:
        ai_guard(player);
        break;
    case TRAP:
        ai_trap(player);
    default:
        break;
    }
}

void Entity::ai_walk() {
    m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
}

void Entity::ai_walk(float l_bound, float r_bound) {
    switch_animation("run", false); // this should not need to be called
    if (m_position.x < l_bound)
    {
        if (!m_movement.x) m_movement.x = 1.0f;
        move_right();
    }
    if (m_position.x > r_bound)
    {
        if (!m_movement.x) m_movement.x = -1.0f;
        move_left();
    }
}

void Entity::ai_guard(Entity* player) {
    switch (m_ai_state) {
    case IDLE:
		switch_animation("idle", false);
		m_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        if (glm::distance(m_position, player->get_position()) < 3.0f) m_ai_state = WALKING;
        break;
    case WALKING:
        if (m_position.x > player->get_position().x + 1.2) {
            m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
            switch_animation("run", false);
            face_left();
        }
        else if (m_position.x < player->get_position().x - 1.2) {
            m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
            switch_animation("run", false);
            face_right();
        }
        else {
            //m_ai_state = IDLE;
            m_ai_state = ATTACKING;
        }
        break;
    case ATTACKING:
        attack();
		m_ai_state = IDLE;
        break;
    default:
        break;
    }
}

void Entity::ai_trap(Entity *player) {
    switch (m_ai_state) {
    case ATTACKING:
        attack();
        if (m_position.x > player->get_position().x + 1.2) {
            face_left();
            m_ai_state = IDLE;
        }
        else if (m_position.x < player->get_position().x - 1.2) {
            face_right();
            m_ai_state = IDLE;
        }
        break;
    case IDLE:
        if (!(m_position.x > player->get_position().x + 1.2 || m_position.x < player->get_position().x - 1.2)) {
            m_ai_state = ATTACKING;
            break;
        }
    }
}

Entity::Entity()
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(0.0f), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(0), m_velocity(0.0f), m_acceleration(0.0f), m_width(0.0f), m_height(0.0f) {}

Entity::Entity(GLuint texture_id, float speed, glm::vec3 acceleration, float jump_power, float animation_time,
    int animation_frames, int animation_index, int animation_cols,
    int animation_rows, float width, float height, EntityType entity_type)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_acceleration(acceleration), m_jumping_power(jump_power), m_animation_cols(animation_cols),
    m_animation_frames(animation_frames), m_animation_index(animation_index),
    m_animation_rows(animation_rows), m_animation_indices(nullptr),
    m_animation_time(animation_time), m_texture_id(texture_id), m_velocity(0.0f),
    m_width(width), m_height(height), m_entity_type(entity_type) {
    face_right();
}

Entity::Entity(GLuint texture_id, float speed, glm::vec3 acceleration, float jump_power, float animation_time,
    int animation_frames, int animation_index, int animation_cols,
    int animation_rows, float width, float height, EntityType entity_type, AIType ai_type, AIState ai_state)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_acceleration(acceleration), m_jumping_power(jump_power), m_animation_cols(animation_cols),
    m_animation_frames(animation_frames), m_animation_index(animation_index),
    m_animation_rows(animation_rows), m_animation_indices(nullptr),
    m_animation_time(animation_time), m_texture_id(texture_id), m_velocity(0.0f),
    m_width(width), m_height(height), m_entity_type(entity_type), m_ai_type(ai_type), m_ai_state(ai_state) {
    face_right();
}

Entity::Entity(GLuint texture_id, float speed, float width, float height, EntityType entity_type)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height), m_entity_type(entity_type) {}

Entity::Entity(GLuint texture_id, float speed, float width, float height, EntityType entity_type, AIType ai_type, AIState ai_state)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height), m_entity_type(entity_type), m_ai_type(ai_type), m_ai_state(ai_state) {}

Entity::~Entity() {}

void Entity::face_right() 
{ 
    if (!m_face_forward)
    {
		m_scale.x *= -1; 
		if (m_hurtbox) m_hurtbox->m_offset.x *= -1;
    }
	m_face_forward = true;
}

void Entity::face_left() 
{ 
    if (m_face_forward)
    {
        m_scale.x *= -1;
		if (m_hurtbox) m_hurtbox->m_offset.x *= -1;
    }
	m_face_forward = false;
}

void const Entity::set_hitbox(Hitbox* hitbox) { m_hitbox = hitbox; }
void const Entity::set_hurtbox(Hitbox* hurtbox) { m_hurtbox = hurtbox; }
// helpers
void const Entity::set_hitdata_by_animation() { m_hitbox->set_hitdata(m_current_animation); }
void const Entity::hitbox_activate() { m_hitbox->m_active = true; }
void const Entity::hitbox_deactivate() { m_hitbox->m_active = false; }

void const Entity::attack() 
{ 
    switch_animation("attack", true);  
    set_hitdata_by_animation(); 
}

void const Entity::death() 
{ 
    m_alive = false;
    switch_animation("death", true);  
}


void Entity::set_animation(std::string animation_name, int* indices, int frames, int active_frames, int active_start) {
    m_animations[animation_name] = { std::vector<int>(indices, indices + frames), active_frames, active_start };
}

void Entity::switch_animation(std::string animation_name, bool locked) {
    if (!m_animation_lock || (animation_name == "death" && m_current_animation != "death")) {
        if (locked) m_animation_index = 0;
        m_animation_lock = locked;
        auto it = m_animations.find(animation_name);
        if (it != m_animations.end()) {
            m_current_animation = animation_name;

            // Access the indices, active_frames, and active_start from the AnimationData
            AnimationData& animData = it->second;
            m_animation_indices = animData.indices.data();
            m_animation_frames = animData.indices.size();
        }
    }
}

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index) {
    if (m_current_animation.empty() || m_animation_indices == nullptr) return;

    // Get the frame index from the current animation
    int frame_index = m_animation_indices[m_animation_index];


    float u_coord = (float)(frame_index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(frame_index / m_animation_cols) / (float)m_animation_rows;

    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;

    // antiquated code
    float margin_u = 0.0f;
    float margin_v = 0.0f;

    // Adjusted UV coordinates with margin
    float tex_coords[] = {
        u_coord + margin_u,         v_coord + height - margin_v,
        u_coord + width - margin_u, v_coord + height - margin_v,

        u_coord + width - margin_u, v_coord + margin_v,
        u_coord + margin_u,         v_coord + height - margin_v,
        u_coord + width - margin_u, v_coord + margin_v,
        u_coord + margin_u,         v_coord + margin_v
    };

    float vertices[] = {
        -0.5, -0.5, 0.5, -0.5, 0.5, 0.5,
        -0.5, -0.5, 0.5, 0.5, -0.5, 0.5
    };

    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);

    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

bool const Entity::check_collision(Entity* other) const {
    float left_margin_adjustment = m_margin[0][0];
    float right_margin_adjustment = m_margin[0][1];
    float up_margin_adjustment = m_margin[1][0];
    float down_margin_adjustment = m_margin[1][1];

    float adjusted_width = m_width - left_margin_adjustment - right_margin_adjustment;
    float adjusted_height = m_height - up_margin_adjustment - down_margin_adjustment;

    float other_left_margin_adjustment = other->m_margin[0][0];
    float other_right_margin_adjustment = other->m_margin[0][1];
    float other_up_margin_adjustment = other->m_margin[1][0];
    float other_down_margin_adjustment = other->m_margin[1][1];

    float other_adjusted_width = other->m_width - other_left_margin_adjustment - other_right_margin_adjustment;
    float other_adjusted_height = other->m_height - other_up_margin_adjustment - other_down_margin_adjustment;

    float x_distance = fabs((m_position.x + left_margin_adjustment - right_margin_adjustment) -
        (other->m_position.x + other_left_margin_adjustment - other_right_margin_adjustment))
        - ((adjusted_width + other_adjusted_width) / 2.0f);
    float y_distance = fabs((m_position.y + up_margin_adjustment - down_margin_adjustment) -
        (other->m_position.y + other_up_margin_adjustment - other_down_margin_adjustment))
        - ((adjusted_height + other_adjusted_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}

void const Entity::check_collision_x(Entity* collidable_entities, int collidable_entity_count) {
    float left_margin_adjustment = m_margin[0][0];
    float right_margin_adjustment = m_margin[0][1];
    float adjusted_width = m_width - left_margin_adjustment - right_margin_adjustment;

    for (int i = 0; i < collidable_entity_count; i++) {
        Entity* collidable_entity = &collidable_entities[i];
        if (!collidable_entity->m_is_active) return;
        if (collidable_entity == this) continue;

        float other_left_margin_adjustment = collidable_entity->m_margin[0][0];
        float other_right_margin_adjustment = collidable_entity->m_margin[0][1];
        float other_adjusted_width = collidable_entity->m_width - other_left_margin_adjustment - other_right_margin_adjustment;

        if (check_collision(collidable_entity)) {
            float x_distance = fabs((m_position.x + left_margin_adjustment - right_margin_adjustment) -
                (collidable_entity->m_position.x + other_left_margin_adjustment - other_right_margin_adjustment));
            float x_overlap = fabs(x_distance - (adjusted_width / 2.0f) - (other_adjusted_width / 2.0f));
            if (m_velocity.x > 0) {
                m_position.x -= x_overlap;
                m_velocity.x = 0;
                m_collided_right = true;
            }
            else if (m_velocity.x < 0) {
                m_position.x += x_overlap;
                m_velocity.x = 0;
                m_collided_left = true;
            }
        }
    }
}

void const Entity::check_collision_y(Entity* collidable_entities, int collidable_entity_count) {
    float up_margin_adjustment = m_margin[1][0];
    float down_margin_adjustment = m_margin[1][1];
    float adjusted_height = m_height - up_margin_adjustment - down_margin_adjustment;

    for (int i = 0; i < collidable_entity_count; i++) {
        Entity* collidable_entity = &collidable_entities[i];
        if (!collidable_entity->m_is_active) return;
        if (collidable_entity == this) continue;

        float other_up_margin_adjustment = collidable_entity->m_margin[1][0];
        float other_down_margin_adjustment = collidable_entity->m_margin[1][1];
        float other_adjusted_height = collidable_entity->m_height - other_up_margin_adjustment - other_down_margin_adjustment;

        if (check_collision(collidable_entity)) {
            float y_distance = fabs((m_position.y + up_margin_adjustment - down_margin_adjustment) -
                (collidable_entity->m_position.y + other_up_margin_adjustment - other_down_margin_adjustment));
            float y_overlap = fabs(y_distance - (adjusted_height / 2.0f) - (other_adjusted_height / 2.0f));

            if (m_velocity.y > 0) {
                m_position.y -= y_overlap;
                m_velocity.y = 0;
                m_collided_top = true;
            }
            else if (m_velocity.y < 0) {
                m_position.y += y_overlap;
                m_velocity.y = 0;
                m_collided_bottom = true;
            }
        }
    }
}

void const Entity::check_collision_y(Map* map) {
    float up_margin_adjustment = m_margin[1][0];
    float down_margin_adjustment = m_margin[1][1];
    float adjusted_height = m_height - up_margin_adjustment - down_margin_adjustment;
    float adjusted_width = m_width - (m_margin[0][0] + m_margin[0][1]);

    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (adjusted_height / 2) - up_margin_adjustment, m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - (adjusted_width / 2), m_position.y + (adjusted_height / 2) - up_margin_adjustment, m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (adjusted_width / 2), m_position.y + (adjusted_height / 2) - up_margin_adjustment, m_position.z);

    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (adjusted_height / 2) + down_margin_adjustment, m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - (adjusted_width / 2), m_position.y - (adjusted_height / 2) + down_margin_adjustment, m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (adjusted_width / 2), m_position.y - (adjusted_height / 2) + down_margin_adjustment, m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0) {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.y > 0) {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.y > 0) {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }

    if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0) {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.y < 0) {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.y < 0) {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
}

void const Entity::check_collision_x(Map* map) {
    float left_margin_adjustment = m_margin[0][0];
    float right_margin_adjustment = m_margin[0][1];
    float adjusted_height = m_height - (m_margin[1][0] + m_margin[1][1]);
    float adjusted_width = m_width - left_margin_adjustment - right_margin_adjustment;

    glm::vec3 left = glm::vec3(m_position.x - (adjusted_width / 2) + left_margin_adjustment, m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (adjusted_width / 2) - right_margin_adjustment, m_position.y, m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0) {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0) {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
    }
}

void Entity::update(float delta_time, Entity* player, Entity* collidable_entities, int collidable_entity_count, Map* map) {
    if (!m_is_active) return;
    switch_animation("idle", false);

    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_left = false;
    m_collided_right = false;

    if (m_entity_type == ENEMY) ai_activate(player);

    if (m_animation_indices != nullptr && !m_current_animation.empty()) {
        m_animation_time += delta_time;
        float frames_per_second = (float)1 / SECONDS_PER_FRAME;

        if (m_animation_time >= frames_per_second) {
            m_animation_time = 0.0f;
            m_animation_index++;

            // Check if we are within the active frames
            auto it = m_animations.find(m_current_animation);
            if (it != m_animations.end()) {
                int active_start = it->second.active_start;
                int active_end = active_start + it->second.active_frames;

                if (m_animation_index >= active_start && m_animation_index < active_end) {
                    // The frame is active
                    m_hitbox->set_active(true);
                }
                else {
                    m_hitbox->set_active(false);
                }
            }

            if (m_animation_index >= m_animation_frames) {
                m_animation_index = 0; // Loop back to the first frame
                m_animation_lock = false;
                m_hitbox->set_active(false);
                if (!m_alive) deactivate();  // if animation has finished and no longer alive
            }
        }
    }

    m_velocity.x = m_movement.x * m_speed;
    m_velocity += m_acceleration * delta_time;

    if (m_is_jumping) {
        m_is_jumping = false;
        m_velocity.y += m_jumping_power;
    }

    m_position.x += m_velocity.x * delta_time;
    check_collision_x(collidable_entities, collidable_entity_count);
    check_collision_x(map);

    m_position.y += m_velocity.y * delta_time;
    check_collision_y(collidable_entities, collidable_entity_count);
    check_collision_y(map);


    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale); // DEBUG
}

void Entity::render(ShaderProgram* program) {
    if (!m_is_active) return;
    program->set_model_matrix(m_model_matrix);

    if (m_animation_indices != NULL) {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

