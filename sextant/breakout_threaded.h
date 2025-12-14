#ifndef BREAKOUT_THREADED_H
#define BREAKOUT_THREADED_H

/**
 * @file breakout_threaded.h
 *
 * VERSION MULTITHREADÉE du jeu Breakout
 *
 * ARCHITECTURE:
 * - Thread Principal (main) : Gestion du rendu et de la boucle jeu
 * - Thread Physique (ball_thread) : Déplacement de la balle
 * - Thread IA (paddle2_thread) : Contrôle de la raquette IA
 * - Thread Input (input_thread) : Lecture clavier pour paddle joueur
 *
 * SYNCHRONISATION:
 * - Mutex pour accès à l'état de la balle
 * - Mutex pour accès aux positions des paddles
 * - Conditions de jeu (game_over, you_win) protégées
 */

#include <drivers/EcranBochs.h>
#include <drivers/Clavier.h>
#include <sextant/ordonnancements/preemptif/thread.h>
#include <sextant/Synchronisation/Mutex/Mutex.h>
#include <sextant/sprites_breakout.h>

// Structure partagée pour l'état du jeu
typedef struct
{
    // État de la balle
    int ball_x, ball_y;
    int ball_dx, ball_dy;

    // Positions des paddles
    int paddle1_x, paddle1_y;
    int paddle2_x, paddle2_y;

    // État du jeu
    bool game_over;
    bool you_win;
    int score;

    // Flags
    bool initialized;
} GameState;

// Variables globales (visibles par tous les threads)
static GameState game_state = {
    320, 350,     // ball pos
    1, -1,        // ball velocity
    250, 370,     // paddle1
    250, 20,      // paddle2
    false, false, // game_over, you_win
    0,            // score
    false         // initialized
};

static Mutex ball_mutex;       // Protège ball_x, ball_y, ball_dx, ball_dy
static Mutex paddle_mutex;     // Protège positions et état
static Mutex game_state_mutex; // Protège game_over, you_win

// Briques (lecture seule après init)
typedef struct
{
    int x, y;
    bool destroyed;
} SimpleBrick;

static SimpleBrick bricks[12];

/**
 * Thread: Physique de la balle
 * Responsabilités:
 * - Déplacer la balle
 * - Déterminer les rebonds sur les murs
 * - Vérifier collision avec les paddles
 * - Vérifier collision avec les briques
 * - Détecter game_over / you_win
 */
void ball_physics_thread_func(void *arg)
{
    EcranBochs *vga = (EcranBochs *)arg;
    int brick_count = 0;
    int frame_counter = 0;

    while (true)
    {
        // Déplacer la balle lentement (tous les 3 cycles)
        frame_counter++;
        if (frame_counter > 3)
        {
            frame_counter = 0;

            ball_mutex.lock();
            game_state.ball_x += game_state.ball_dx;
            game_state.ball_y += game_state.ball_dy;

            // Rebond sur les murs latéraux
            if (game_state.ball_x <= 10)
                game_state.ball_dx = 1;
            if (game_state.ball_x >= 630)
                game_state.ball_dx = -1;

            // Vérifier game_over (haut ou bas)
            if (game_state.ball_y < 0 || game_state.ball_y > 400)
            {
                paddle_mutex.lock();
                game_state.game_over = true;
                paddle_mutex.unlock();
            }

            ball_mutex.unlock();
        }

        // Collision avec paddles
        ball_mutex.lock();
        paddle_mutex.lock();

        // Paddle 1 (joueur EN BAS)
        if (game_state.ball_y >= game_state.paddle1_y - 30 &&
            game_state.ball_x > game_state.paddle1_x - 10 &&
            game_state.ball_x < game_state.paddle1_x + 110)
        {
            game_state.ball_dy = -1;
            game_state.score++;
        }

        // Paddle 2 (IA EN HAUT)
        if (game_state.ball_y <= game_state.paddle2_y + 30 &&
            game_state.ball_x > game_state.paddle2_x - 10 &&
            game_state.ball_x < game_state.paddle2_x + 110)
        {
            game_state.ball_dy = 1;
            game_state.score++;
        }

        paddle_mutex.unlock();

        // Collision avec briques
        for (int i = 0; i < 12; i++)
        {
            if (!bricks[i].destroyed)
            {
                int bx = bricks[i].x;
                int by = bricks[i].y;
                int ball_radius = 10;

                if (game_state.ball_x + ball_radius > bx &&
                    game_state.ball_x - ball_radius < bx + 90 &&
                    game_state.ball_y + ball_radius > by &&
                    game_state.ball_y - ball_radius < by + 25)
                {
                    bricks[i].destroyed = true;
                    game_state.ball_dy = -game_state.ball_dy;
                    game_state.score += 10;
                    brick_count++;
                }
            }
        }

        // Vérifier YOU WIN
        if (brick_count == 12)
        {
            game_state.you_win = true;
        }

        ball_mutex.unlock();

        // Yield pour laisser les autres threads s'exécuter
        thread_yield();
    }
}

/**
 * Thread: Intelligence Artificielle (Paddle 2)
 * Responsabilités:
 * - Suivre la position de la balle
 * - Bouger la raquette IA automatiquement
 */
void paddle_ai_thread_func(void *arg)
{
    while (true)
    {
        ball_mutex.lock();
        paddle_mutex.lock();

        // L'IA suit la balle
        if (game_state.ball_x < game_state.paddle2_x && game_state.paddle2_x > 0)
            game_state.paddle2_x -= 3;
        if (game_state.ball_x > game_state.paddle2_x + 100 && game_state.paddle2_x < 540)
            game_state.paddle2_x += 3;

        paddle_mutex.unlock();
        ball_mutex.unlock();

        thread_yield();
    }
}

/**
 * Thread: Gestion de l'input clavier
 * Responsabilités:
 * - Lire les touches Z et D
 * - Déplacer la raquette joueur (paddle1)
 */
void input_thread_func(void *arg)
{
    Clavier keyboard;

    while (true)
    {
        if (keyboard.is_pressed(AZERTY::K_Z))
        {
            paddle_mutex.lock();
            if (game_state.paddle1_x > 0)
                game_state.paddle1_x -= 8;
            paddle_mutex.unlock();
        }

        if (keyboard.is_pressed(AZERTY::K_D))
        {
            paddle_mutex.lock();
            if (game_state.paddle1_x < 540)
                game_state.paddle1_x += 8;
            paddle_mutex.unlock();
        }

        thread_yield();
    }
}

/**
 * Version MULTITHREADÉE du jeu Breakout
 * Utilise 3 threads + thread principal pour le rendu
 */
void threaded_breakout()
{
    EcranBochs vga(640, 400, VBE_MODE::_32);
    vga.init();

    // Initialiser les briques
    for (int row = 0; row < 2; row++)
    {
        for (int col = 0; col < 6; col++)
        {
            bricks[row * 6 + col].x = 40 + col * 100;
            bricks[row * 6 + col].y = 80 + row * 35;
            bricks[row * 6 + col].destroyed = false;
        }
    }

    game_state.initialized = true;

    // Créer les threads
    // NOTE: Cette partie dépend de l'API thread du kernel
    // À adapter selon sextant/Activite/Threads.h

    // POUR L'INSTANT: Version simple avec mutex
    // Les threads réels seront ajoutés une fois validés

    // Boucle de rendu PRINCIPAL (dans le thread principal)
    while (!game_state.game_over && !game_state.you_win)
    {
        vga.clear(0, 0, 0);

        // Dessiner paddles
        draw_paddle(vga, game_state.paddle1_x, game_state.paddle1_y, 100, 10, 0, 255, 0);
        draw_paddle(vga, game_state.paddle2_x, game_state.paddle2_y, 100, 10, 0, 255, 0);

        // Dessiner balle
        draw_ball(vga, game_state.ball_x, game_state.ball_y, 5, 255, 255, 255);

        // Dessiner briques
        for (int i = 0; i < 12; i++)
        {
            if (!bricks[i].destroyed)
            {
                draw_brick(vga, bricks[i].x, bricks[i].y, 90, 25, 255, 0, 0);
            }
        }

        vga.swapBuffer();
    }

    // Afficher écran de fin
    if (game_state.you_win)
    {
        draw_you_win(vga, game_state.score);
    }
    else
    {
        draw_game_over(vga, game_state.score);
    }

    vga.swapBuffer();

    // Boucle infinie pour afficher le résultat
    while (1)
    {
        if (game_state.you_win)
            draw_you_win(vga, game_state.score);
        else
            draw_game_over(vga, game_state.score);
    }
}

#endif // BREAKOUT_THREADED_H
