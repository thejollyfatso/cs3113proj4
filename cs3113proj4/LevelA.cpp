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
    86, 87, 86, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    106, 107, 27, 28, 27, 28, 27, 28, 1, 2, 2, 2, 2, 2,
    126, 127, 128, 127, 127, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

LevelA::~LevelA()
{
    delete [] m_game_state.enemies;
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

    glm::vec3 acceleration = glm::vec3(0.0f, -4.81f, 0.0f);

    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        0.0f,                      // animation time
        8,                         // animation frame amount
        0,                         // current animation index
        8,                         // animation column amount
        8,                         // animation row amount
        1.0f,                      // width
        1.0f,                       // height
        PLAYER
    );
    
    // set animations
    int run_animation[] = { 56, 57, 58, 59, 60, 61, 62, 63 };
    int idle_animation[] = { 40, 41, 42, 43, 44, 45 };

    m_game_state.player->set_animation("run", run_animation, 8);
    m_game_state.player->set_animation("idle", idle_animation, 6);
    m_game_state.player->switch_animation("idle"); // start with idle

    m_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(3.0f);
    
    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    int e_run_animation[] = { 24, 25, 26, 27, 28, 29, 30, 31 };
    int e_idle_animation[] = { 16, 17, 18, 19 };
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
		m_game_state.enemies[i] =  Entity(
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
		m_game_state.enemies[i].set_animation("run", e_run_animation, 8);
		m_game_state.enemies[i].set_animation("idle", e_idle_animation, 4);
		m_game_state.enemies[i].switch_animation("idle"); // start with idle
    }


    m_game_state.enemies[0].set_position(glm::vec3(8.0f, 0.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));

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
    
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
    }
}


void LevelA::render(ShaderProgram *g_shader_program)
{
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    for (int i = 0; i < m_number_of_enemies; i++)
            m_game_state.enemies[i].render(g_shader_program);
}
