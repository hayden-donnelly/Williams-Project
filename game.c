#include <stdio.h>
#include <SDL2/SDL.h>
#include "engine.h"
//#include <SDL2/SDL_Mixer.h>

SDL_Texture *background_texture;
SDL_Texture *zombie_texture;
SDL_Texture *between_round_ui;
SDL_Texture *press_start;
SDL_Texture *speed_up;
SDL_Texture *game_over;
SDL_Texture *attack_speed_up;
SDL_Texture *damage_up;
SDL_Texture *victory;
SDL_Texture *final_challenge;

double shoot_cooldown = 0.2;
double shoot_cooldown_timer = 0;

int bullet_damage = 1;

int number_of_bullets = 20;
int number_of_enemies = 10;

int round_counter = 0;
bool round_over = true;

typedef struct
{
    SDL_Texture *texture;
    SDL_Rect rect;
    int dir_x;
    int dir_y;
    int speed;
    int health;
    void (*behaviour)(struct entity*);    // Only for interchangeable enemy patterns
    bool is_active;
} entity;

entity *bullets;
entity *enemies;
entity player;

void zombie_chase(entity *desired_entity);
entity new_zombie(entity desired_entity, int x, int y);
void reset_player();
void set_enemies_to_empty();

void game_load()
{
    // Init bullets
    bullets = (entity*)malloc(number_of_bullets*sizeof(entity));
    SDL_Texture *bullet_texture = load_image_to_texture("Art/Bullet.png");
    for(int i = 0; i < 20; i++)
    {
        bullets[i].texture = bullet_texture;
        SDL_Rect bullet_rect;
        bullet_rect.x = -20;
        bullet_rect.x = -20;
        bullet_rect.w = 16;
        bullet_rect.h = 16;
        bullets[i].rect = bullet_rect;
        bullets[i].dir_x = 0;
        bullets[i].dir_y = 0;
        bullets[i].speed = 20;
        bullets[i].health = 1;
        bullets[i].is_active = false;
    }

    // Init enemies
    enemies = (entity*)malloc(number_of_enemies*sizeof(entity));
    set_enemies_to_empty();

    // Init player
    player.texture = load_image_to_texture("Art/PlayerCharacter.png");
    SDL_Rect player_rect;
    player_rect.w = 85;
    player_rect.h = 126;
    player_rect.x = (render_width/2) - (player_rect.w/2);
    player_rect.y = (render_height/2) - (player_rect.h/2);
    player.rect = player_rect;
    player.dir_x = 0;
    player.dir_y = 0;
    player.speed = 10;
    player.health = 500;
    player.is_active = true;

    // Load background
    background_texture = load_image_to_texture("Art/Background.png");
    // Load enemy textures
    zombie_texture = load_image_to_texture("Art/Zombie.png");
    // Load UI
    press_start = load_image_to_texture("Art/PressStart.png");
    speed_up = load_image_to_texture("Art/SpeedUp.png");
    game_over = load_image_to_texture("Art/GameOver.png");
    attack_speed_up = load_image_to_texture("Art/AttackSpeedUp.png");
    damage_up = load_image_to_texture("Art/DamageUp.png");
    victory = load_image_to_texture("Art/Victory.png");
    final_challenge = load_image_to_texture("Art/FinalChallenge.png");
}

void game_update(double dt)
{
    if(round_over)
    {
        switch(round_counter)
        {
            case -1:    // Gameover
                reset_player();
                between_round_ui = game_over;
                if(input_get_key_down(SDL_SCANCODE_SPACE))
                {
                    round_counter = 0;
                    return;
                } else { return; }
                break;
            case 0:
                reset_player();
                between_round_ui = press_start;
                if(input_get_key_down(SDL_SCANCODE_SPACE))
                {
                    round_over = false;
                    player.health = 120;
                    player.speed = 7;
                    shoot_cooldown = 0.4;
                    bullet_damage = 1;
                    set_enemies_to_empty();
                    enemies[0] = new_zombie(enemies[0], 200, 360);
                    enemies[1] = new_zombie(enemies[1], 1080, 360);
                } else { return; }  
                break;
            case 1:
                reset_player();
                between_round_ui = speed_up;
                if(input_get_key_down(SDL_SCANCODE_SPACE))
                {   
                    round_over = false;
                    player.health = 90;
                    player.speed = 10;
                    shoot_cooldown = 0.4;
                    bullet_damage = 1;
                    set_enemies_to_empty();
                    enemies[0] = new_zombie(enemies[0], 200, 360);
                    enemies[1] = new_zombie(enemies[1], 1080, 360);
                    enemies[2] = new_zombie(enemies[2], 640, 150);
                } else { return; }
                break;
            case 2:
                reset_player();
                between_round_ui = attack_speed_up;
                if(input_get_key_down(SDL_SCANCODE_SPACE))
                {
                    round_over = false;
                    player.health = 50;
                    player.speed = 10;
                    shoot_cooldown = 0.2;
                    bullet_damage = 1;
                    set_enemies_to_empty();
                    enemies[0] = new_zombie(enemies[0], 200, 360);
                    enemies[1] = new_zombie(enemies[1], 1080, 360);
                    enemies[2] = new_zombie(enemies[2], 640, 150);
                    enemies[3] = new_zombie(enemies[3], 640, 520);
                    return;
                } else { return; }
            case 3:
                reset_player();
                between_round_ui = damage_up;
                if(input_get_key_down(SDL_SCANCODE_SPACE))
                {
                    round_over = false;
                    player.health = 20;
                    player.speed = 10;
                    shoot_cooldown = 0.2;
                    bullet_damage = 2;
                    set_enemies_to_empty();
                    enemies[0] = new_zombie(enemies[0], 200, 360);
                    enemies[1] = new_zombie(enemies[1], 1080, 360);
                    enemies[2] = new_zombie(enemies[2], 640, 150);
                    return;
                } else { return; }
                break;
            case 4:
                reset_player();
                between_round_ui = final_challenge;
                if(input_get_key_down(SDL_SCANCODE_SPACE))
                {
                    round_over = false;
                    player.health = 20;
                    player.speed = 10;
                    shoot_cooldown = 0.2;
                    bullet_damage = 2;
                    set_enemies_to_empty();
                    enemies[0] = new_zombie(enemies[0], 200, 360);
                    enemies[1] = new_zombie(enemies[1], 1080, 360);
                    enemies[2] = new_zombie(enemies[2], 640, 150);
                    enemies[3] = new_zombie(enemies[3], 640, 520);
                    return;
                } else { return; }
            default:
                reset_player();
                between_round_ui = victory;
                if(input_get_key_down(SDL_SCANCODE_SPACE))
                {   
                    round_counter = 0;
                    return;
                } else { return; }
                break;
        }
    }

    // Cooldowns
    if(shoot_cooldown_timer > 0) { shoot_cooldown_timer -= 0.01; }

    // Player Character Movement
    player.dir_x = 0;
    player.dir_y = 0;
    if(input_get_key(SDL_SCANCODE_W) && player.rect.y > 75)
    {
        player.dir_y = -1;
    }
    if(input_get_key(SDL_SCANCODE_S) && (player.rect.y + player.rect.h) < 627)
    {
        player.dir_y = 1;;
    }
    if(input_get_key(SDL_SCANCODE_A) && player.rect.x > 97)
    {
        player.dir_x = -1;
    }
    if(input_get_key(SDL_SCANCODE_D) && (player.rect.x + player.rect.w) < 1190)
    {
        player.dir_x = 1;
    }
    player.rect.x += player.dir_x * player.speed;
    player.rect.y += player.dir_y * player.speed;

    // Shooting
    if(shoot_cooldown_timer <= 0)
    {
        int dir_x = 0;
        int dir_y = 0;
        if(input_get_key(SDL_SCANCODE_UP))
        {
            shoot_cooldown_timer = shoot_cooldown;
            dir_y = -1;
        }
        else if(input_get_key(SDL_SCANCODE_DOWN))
        {
            shoot_cooldown_timer = shoot_cooldown;
            dir_y = 1;
        }
        else if(input_get_key(SDL_SCANCODE_LEFT))
        {
            shoot_cooldown_timer = shoot_cooldown;
            dir_x = -1;
        }
        else if(input_get_key(SDL_SCANCODE_RIGHT))
        {
            shoot_cooldown_timer = shoot_cooldown;
            dir_x = 1;
        }

        if(dir_x != 0 || dir_y != 0)
        {
            // Iterate through bullet until inactive bullet is found
            for(int i = 0; i < number_of_bullets; i++)
            {
                if(!bullets[i].is_active)
                {
                    bullets[i].dir_x = dir_x;
                    bullets[i].dir_y = dir_y;
                    bullets[i].rect.x = player.rect.x + (player.rect.w/2);
                    bullets[i].rect.y = player.rect.y + (player.rect.h/2);
                    bullets[i].is_active = true;
                    break;
                }
            }
        }
    }

    for(int i = 0; i < number_of_bullets; i++)
    {
        if(bullets[i].is_active)
        {
            // Move bullets
            bullets[i].rect.x += bullets[i].dir_x * bullets[i].speed;
            bullets[i].rect.y += bullets[i].dir_y * bullets[i].speed;
            
            // Check if bullets are still on screen
            if(bullets[i].rect.x > render_width || (bullets[i].rect.x + bullets[i].rect.w) < 0
                || bullets[i].rect.y > render_height || (bullets[i].rect.y + bullets[i].rect.h) < 0)
            {
                bullets[i].is_active = false;
            } 
        }
    }

    // round over is false as long as there are still enemies in the scene or player is alive
    round_over = true;
    for(int i = 0; i < number_of_enemies; i++)
    {
        if(enemies[i].is_active)
        {
            round_over = false;
            // Call enemy behaviour
            enemies[i].behaviour(&enemies[i]);
    
            // Check collision with bullets
            for(int a = 0; a < number_of_bullets; a++)
            {
                if(bullets[a].is_active
                    && enemies[i].rect.x < bullets[a].rect.x + bullets[a].rect.w
                    && enemies[i].rect.x + enemies[i].rect.w > bullets[a].rect.x
                    && enemies[i].rect.y < bullets[a].rect.y + bullets[a].rect.h
                    && enemies[i].rect.y + enemies[i].rect.h > bullets[a].rect.y)
                {
                    enemies[i].health -= bullet_damage;
                    bullets[a].is_active = false;

                    if(enemies[i].health <= 0)
                    {
                        enemies[i].is_active = false;
                    }
                }
            }

            // Check collision with player
            if(player.is_active
                && enemies[i].rect.x < player.rect.x + player.rect.w
                && enemies[i].rect.x + enemies[i].rect.w > player.rect.x
                && enemies[i].rect.y < player.rect.y + player.rect.h
                && enemies[i].rect.y + enemies[i].rect.h > player.rect.y)
            {
                player.health -= 1;
                if(player.health <= 0)
                {
                    player.is_active = false;
                }
            }
        }
    }

    if(!player.is_active)
    {
        round_over = true;
        round_counter = -1;
    }

    if(round_over && round_counter != -1)
    {
        round_counter++;
    }
}

void game_render(SDL_Renderer *renderer)
{
    // Render background
    SDL_RenderCopy(renderer, background_texture, NULL, NULL); 

    // Render player
    if(player.is_active)
    {
        SDL_RenderCopy(renderer, player.texture, NULL, &player.rect);
    }

    // Render enemies
    for(int i = 0; i < number_of_enemies; i++)
    {
        if(enemies[i].is_active)
        {
            SDL_RenderCopy(renderer, enemies[i].texture, NULL, &enemies[i].rect);
        }
    }

    // Render bullets
    for(int i = 0; i < number_of_bullets; i++)
    {
        if(bullets[i].is_active)
        {
            SDL_RenderCopy(renderer, bullets[i].texture, NULL, &bullets[i].rect);
        }
    }

    // Render UI
    if(between_round_ui != NULL && round_over)
    {
        SDL_RenderCopy(renderer, between_round_ui, NULL, NULL);
    }
}

void game_unload()
{
    SDL_DestroyTexture(background_texture);
    SDL_DestroyTexture(zombie_texture);
    SDL_DestroyTexture(between_round_ui);
    SDL_DestroyTexture(press_start);
    SDL_DestroyTexture(speed_up);
    SDL_DestroyTexture(game_over);
    SDL_DestroyTexture(attack_speed_up);
    SDL_DestroyTexture(damage_up);
    SDL_DestroyTexture(victory);
    SDL_DestroyTexture(final_challenge);
}

void zombie_chase(entity *desired_entity)
{
    int horizontal_offset = player.rect.x + (player.rect.w/2) - desired_entity->rect.x + (desired_entity->rect.w/2);
    if(horizontal_offset > 50)
    {
        desired_entity->dir_x = 1;
    }
    else if(horizontal_offset < -50)
    {
        desired_entity->dir_x = -1;
    }

    int vertical_offset = player.rect.y + (player.rect.h/2) - desired_entity->rect.y + (desired_entity->rect.h/2);
    if(vertical_offset > 50)
    {
        desired_entity->dir_y = 1;
    }
    else if(vertical_offset < -50)
    {
        desired_entity->dir_y = -1;
    }
    
    // Randomizes movement so enemmies don't stack
    int rand_x = (rand() % (1 + 1 - 0)) + 1;
    rand_x *= desired_entity->dir_x;
    int rand_y = (rand() % (1 + 1 - 0)) + 1;
    rand_y *= desired_entity->dir_y;

    desired_entity->rect.x += desired_entity->dir_x * desired_entity->speed + rand_x;
    desired_entity->rect.y += desired_entity->dir_y * desired_entity->speed + rand_y;
}

entity new_zombie(entity desired_entity, int x, int y)
{   
    desired_entity.texture = zombie_texture;
    desired_entity.rect.w = 70;
    desired_entity.rect.h = 70;
    desired_entity.rect.x = x;
    desired_entity.rect.y = y;
    desired_entity.dir_x = 0;
    desired_entity.dir_y = 0;
    desired_entity.speed = 4;
    desired_entity.health = 3;
    desired_entity.behaviour = &zombie_chase;
    desired_entity.is_active = true;
    return desired_entity;
}

void set_enemies_to_empty()
{
    for(int i = 0; i < number_of_enemies; i++)
    {
        SDL_Rect enemy_rect;
        enemy_rect.x = 0;
        enemy_rect.y = 0;
        enemy_rect.w = 0;
        enemy_rect.h = 0;
        enemies[i].rect = enemy_rect;
        enemies[i].dir_x = 0;
        enemies[i].dir_y = 0;
        enemies[i].speed = 0;
        enemies[i].health = 0;
        enemies[i].is_active = false;
    }
}

void reset_player()
{
    player.rect.x = (render_width/2) - (player.rect.w/2);
    player.rect.y = (render_height/2) - (player.rect.h/2);
    player.is_active = true;
}