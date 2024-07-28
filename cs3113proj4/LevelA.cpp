#include "LevelA.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

constexpr char SPRITESHEET_FILEPATH[] = "assets/spritesheet.png",
           PLATFORM_FILEPATH[]    = "assets/platformPack_tile027.png",
           ENEMY_FILEPATH[]       = "assets/enemy_spritesheet.png";

unsigned int LEVEL_DATA[] =
{
    66, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    110, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    86, 87, 86, 0, 0, 0, 0, 0, 0, 88, 27, 28, 27, 28,
    106, 107, 27, 28, 27, 28, 27, 28, 27, 107, 107, 107, 107, 107,
    126, 127, 128, 127, 127, 107, 107, 107, 107, 107, 107, 107, 107, 107
};

LevelA::~LevelA()
{
    delete [] m_game_state.enemies;
    delete [] m_game_state.hitboxes;
    delete [] m_game_state.hurtboxes;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelA::initialise()
{
    GLuint map_texture_id = Utility::load_texture("assets/Fantasy Swamp Forest/Free/Terrain_and_Props.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, map_texture_id, 1.0f, 20, 34);
    
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);

    glm::vec3 acceleration = glm::vec3(0.0f, -9.81f, 0.0f);

    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        6.0f,                      // jumping power
        0.0f,                      // animation time
        8,                         // animation frame amount
        0,                         // current animation index
        8,                         // animation column amount
        8,                         // animation row amount
        2.0f,                      // width
        2.0f,                       // height
        PLAYER
    );

    // enlarge
    glm::vec3 new_scale = { 4.0f, 4.0f, 0.0f };
    m_game_state.player->set_scale(new_scale);
    m_game_state.player->set_margin_y(1.0f / 3.3f); // trim sprite
    m_game_state.player->set_margin_x(1.0f / 2.0f); // trim sprite
    
    // set animations
    int run_animation[] = { 56, 57, 58, 59, 60, 61, 62, 63 };
    int idle_animation[] = { 40, 41, 42, 43, 44, 45 };
    int counter_animation[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    int attack_animation[] = { 8, 9, 10, 11, 12, 13, 14, 15 };
    int jump_animation[] = { 48, 49 };

    m_game_state.player->set_animation("run", run_animation, 8);
    m_game_state.player->set_animation("idle", idle_animation, 6);
    m_game_state.player->set_animation("counter", counter_animation, 8);
    m_game_state.player->set_animation("attack", attack_animation, 8);
    m_game_state.player->set_animation("jump", jump_animation, 2);
    m_game_state.player->switch_animation("idle", false); // start with idle

    m_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
    
    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    int e_run_animation[] = { 24, 25, 26, 27, 28, 29, 30, 31 };
    int e_idle_animation[] = { 16, 17, 18, 19 };
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
		m_game_state.enemies[i] = Entity(
			enemy_texture_id,         // texture id
			1.0f,                      // speed
			acceleration,              // acceleration
			5.0f,                      // jumping power
			0.0f,                      // animation time
			8,                         // animation frame amount
			0,                         // current animation index
			8,                         // animation column amount
			5,                         // animation row amount
			1.0f,                      // width
			1.0f,                       // height
			ENEMY,
			GUARD,
			IDLE
		);
		m_game_state.enemies[i].set_scale(new_scale);
		m_game_state.enemies[i].set_margin_y(-1.0f / 6.0f); // trim sprite
		m_game_state.enemies[i].set_margin_x(1.0f / 6.0f); // trim sprite
		m_game_state.enemies[i].set_animation("run", e_run_animation, 8);
		m_game_state.enemies[i].set_animation("idle", e_idle_animation, 4);
		m_game_state.enemies[i].switch_animation("idle", false); // start with idle
    }


    m_game_state.enemies[0].set_position(glm::vec3(8.0f, 6.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));

    /* Create Hitboxes and Hurtboxes */
    GLuint hitbox_texture_id = Utility::load_texture("assets/hitbox.png");
    GLuint hurtbox_texture_id = Utility::load_texture("assets/hurtbox.png");

    // create array
    m_game_state.hitboxes = new Hitbox[ENEMY_COUNT + 1]; // may need to use n_number_of_enemies later
    int player_hb_index = ENEMY_COUNT;

    // create hitboxes for each enemy then player
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.hitboxes[i] = Hitbox(
            hitbox_texture_id,         // texture id
            &m_game_state.enemies[i]
		);
        m_game_state.enemies[i].set_hitbox(&m_game_state.hitboxes[i]);
    }
    m_game_state.hitboxes[player_hb_index] = Hitbox(hitbox_texture_id, m_game_state.player);

    // set player hitbox and add hitdata
    m_game_state.player->set_hitbox(&m_game_state.hitboxes[player_hb_index]);
    glm::vec3 hb_scale = { 1.0f, 1.0f, 1.0f };
    glm::vec3 hb_offset = { 1.3f, 0.4f, 0.0f };
    m_game_state.player->get_hitbox()->add_hitdata("attack", hb_scale, hb_offset);
    hb_scale = { 1.0f, 1.0f, 1.0f };
    hb_offset = { 0.8f, 0.6f, 0.0f };
    m_game_state.player->get_hitbox()->add_hitdata("counter", hb_scale, hb_offset);
    //m_game_state.player->get_hitbox()->set_hidden(false);
    m_game_state.enemies[0].get_hitbox()->set_hidden(false);

    // repeat for hurtboxes
    m_game_state.hurtboxes = new Hitbox[ENEMY_COUNT + 1]; // may need to use n_number_of_enemies later
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.hurtboxes[i] = Hitbox(
            hurtbox_texture_id,         // texture id
            &m_game_state.enemies[i]
		);
        m_game_state.enemies[i].set_hurtbox(&m_game_state.hurtboxes[i]);
    }
    m_game_state.hurtboxes[player_hb_index] = Hitbox(hurtbox_texture_id, m_game_state.player);
    m_game_state.player->set_hurtbox(&m_game_state.hurtboxes[player_hb_index]);
    // scale and offset hurtbox
    hb_scale = { 0.7f, 1.4f, 1.0f };
    hb_offset = { -0.15f, 0.1f, 0.0f };
    m_game_state.player->get_hurtbox()->set_scale(hb_scale.x, hb_scale.y);
    m_game_state.player->get_hurtbox()->set_offset(hb_offset.x, hb_offset.y);

    //m_game_state.player->get_hurtbox()->set_hidden(false);
    m_game_state.enemies[0].get_hurtbox()->set_hidden(false);

    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.bgm = Mix_LoadMUS("assets/dooblydoo.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(0.0f);
    
    m_game_state.jump_sfx = Mix_LoadWAV("assets/bounce.wav");
}

void LevelA::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    
    for (int i = 0; i < m_number_of_enemies + 1; i++)
    {
        m_game_state.hitboxes[i].update(delta_time);
        m_game_state.hurtboxes[i].update(delta_time);
    }

    for (int i = 0; i < m_number_of_enemies; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
    }
}


void LevelA::render(ShaderProgram *g_shader_program)
{
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    for (int i = 0; i < m_number_of_enemies + 1; i++)
    {
        m_game_state.hitboxes[i].render(g_shader_program);
        m_game_state.hurtboxes[i].render(g_shader_program);
    }
    for (int i = 0; i < m_number_of_enemies; i++)
            m_game_state.enemies[i].render(g_shader_program);
}
