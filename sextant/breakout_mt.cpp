/**
 * @file breakout_mt.cpp
 * VERSION MULTITHREADÉE du jeu Breakout avec Mutex et threads
 */

#include <drivers/EcranBochs.h>
#include <drivers/Clavier.h>
#include <sextant/ordonnancements/preemptif/thread.h>
#include <sextant/Synchronisation/Mutex/Mutex.h>
#include <sextant/sprites_breakout.h>

// Structure brique
struct SimpleBrick
{
    int x, y;
    bool destroyed;
};

// État partagé du jeu (protégé par Mutex)
struct GameState
{
    int ball_x, ball_y;
    int ball_dx, ball_dy;
    int paddle1_x, paddle1_y;
    int paddle2_x, paddle2_y;
    SimpleBrick bricks[12];
    bool game_over;
    bool you_win;
    int score;
    bool active;
} g_game_state = {
    320, 200, 1, -1, // ball (y=200, part verticalement vers le haut)
    250,
    370, // paddle1
    250,
    20, // paddle2
    {},
    false,
    false,
    0,   // briques, game_over, you_win, score
    true // active
};

static Mutex g_game_mutex; // Protège accès à g_game_state
static EcranBochs *g_vga = NULL;
static volatile bool stop_threads = false;

// ============================================================================
// THREAD 1: Physique de la balle
// ============================================================================
void ball_physics_thread(void *arg)
{
    int frame_counter = 0;

    while (!stop_threads)
    {
        if (!g_game_state.active)
        {
            thread_yield();
            continue;
        }
        frame_counter++;
        if (frame_counter > 3)
        {
            frame_counter = 0;
            g_game_mutex.lock();
            g_game_state.ball_x += g_game_state.ball_dx;
            g_game_state.ball_y += g_game_state.ball_dy;
            if (g_game_state.ball_x <= 10)
                g_game_state.ball_dx = 1;
            if (g_game_state.ball_x >= 630)
                g_game_state.ball_dx = -1;
            if (g_game_state.ball_y < 0 || g_game_state.ball_y > 400)
                g_game_state.game_over = true;
            // Collision paddle 1
            if (g_game_state.ball_y >= g_game_state.paddle1_y - 30 &&
                g_game_state.ball_x > g_game_state.paddle1_x - 10 &&
                g_game_state.ball_x < g_game_state.paddle1_x + 110)
            {
                g_game_state.ball_dy = -1;
                g_game_state.score++;
            }
            // Collision paddle 2
            if (g_game_state.ball_y <= g_game_state.paddle2_y + 30 &&
                g_game_state.ball_x > g_game_state.paddle2_x - 10 &&
                g_game_state.ball_x < g_game_state.paddle2_x + 110)
            {
                g_game_state.ball_dy = 1;
                g_game_state.score++;
            }
            // Collision briques
            int destroyed_count = 0;
            for (int i = 0; i < 12; i++)
            {
                if (!g_game_state.bricks[i].destroyed)
                {
                    int bx = g_game_state.bricks[i].x;
                    int by = g_game_state.bricks[i].y;
                    int r = 10;
                    if (g_game_state.ball_x + r > bx && g_game_state.ball_x - r < bx + 90 &&
                        g_game_state.ball_y + r > by && g_game_state.ball_y - r < by + 25)
                    {
                        g_game_state.bricks[i].destroyed = true;
                        g_game_state.ball_dy = -g_game_state.ball_dy;
                        g_game_state.score += 10;
                        destroyed_count++;
                    }
                }
            }
            // YOU WIN si toutes les briques sont cassées
            int all_destroyed = 0;
            for (int i = 0; i < 12; i++)
                if (g_game_state.bricks[i].destroyed)
                    all_destroyed++;
            if (all_destroyed == 12)
                g_game_state.you_win = true;
            g_game_mutex.unlock();
        }
        thread_yield();
    }
    extern void thread_exit();
    thread_exit();
}

// ============================================================================
// THREAD 2: IA du paddle 2
// ============================================================================
void paddle_ai_thread(void *arg)
{
    while (!stop_threads)
    {
        if (!g_game_state.active)
        {
            thread_yield();
            continue;
        }
        g_game_mutex.lock();
        if (g_game_state.ball_x < g_game_state.paddle2_x && g_game_state.paddle2_x > 0)
            g_game_state.paddle2_x -= 3;
        if (g_game_state.ball_x > g_game_state.paddle2_x + 100 && g_game_state.paddle2_x < 540)
            g_game_state.paddle2_x += 3;
        g_game_mutex.unlock();
        thread_yield();
    }
    extern void thread_exit();
    thread_exit();
}

// ============================================================================
// THREAD 3: Gestion clavier
// ============================================================================
void input_thread(void *arg)
{
    Clavier keyboard;

    while (!stop_threads)
    {
        if (!g_game_state.active)
        {
            thread_yield();
            continue;
        }
        if (keyboard.is_pressed(AZERTY::K_Z))
        {
            g_game_mutex.lock();
            if (g_game_state.paddle1_x > 0)
                g_game_state.paddle1_x -= 8;
            g_game_mutex.unlock();
        }
        if (keyboard.is_pressed(AZERTY::K_D))
        {
            g_game_mutex.lock();
            if (g_game_state.paddle1_x < 540)
                g_game_state.paddle1_x += 8;
            g_game_mutex.unlock();
        }
        thread_yield();
    }
    extern void thread_exit();
    thread_exit();
}

// ============================================================================
// FONCTION PRINCIPALE: Jeu avec threads
// ============================================================================
void threaded_breakout()
{
    EcranBochs vga(640, 400, VBE_MODE::_32);
    vga.init();
    g_vga = &vga;

    while (true)
    {
        // Désactiver la partie pour bloquer les anciens threads
        g_game_mutex.lock();
        g_game_state.active = false;
        g_game_mutex.unlock();
        stop_threads = true;
        // Pause plus longue pour laisser les threads sortir de leur boucle
        for (volatile int i = 0; i < 100000000; ++i)
        {
            __asm__("nop");
        }

        // Debug : afficher le nombre de threads actifs (si possible)
        extern int thread_count_debug();
        int nb_threads = thread_count_debug();
        char buf[64];
        // Affichage sur l'écran VGA (coin haut gauche)
        snprintf(buf, sizeof(buf), "Threads actifs: %d", nb_threads);
        vga.clear(0, 0, 0);
        draw_simple_text(vga, 10, 10, buf);
        vga.swapBuffer();
        // Petite pause pour voir le message
        for (volatile int i = 0; i < 50000000; ++i)
        {
            __asm__("nop");
        }
        stop_threads = false;
        g_game_mutex.lock();
        g_game_state.ball_x = 320;
        g_game_state.ball_y = 200;
        g_game_state.ball_dx = 1;
        g_game_state.ball_dy = -1;
        g_game_state.paddle1_x = 250;
        g_game_state.paddle1_y = 370;
        g_game_state.paddle2_x = 250;
        g_game_state.paddle2_y = 20;
        g_game_state.score = 0;
        g_game_state.game_over = false;
        g_game_state.you_win = false;
        for (int row = 0; row < 2; row++)
        {
            for (int col = 0; col < 6; col++)
            {
                g_game_state.bricks[row * 6 + col].x = 40 + col * 100;
                g_game_state.bricks[row * 6 + col].y = 80 + row * 35;
                g_game_state.bricks[row * 6 + col].destroyed = false;
            }
        }
        g_game_state.active = true;
        g_game_mutex.unlock();

        // Créer les threads qui s'exécutent en parallèle
        struct thread *t_ball = create_kernel_thread(ball_physics_thread, NULL);
        struct thread *t_ai = create_kernel_thread(paddle_ai_thread, NULL);
        struct thread *t_input = create_kernel_thread(input_thread, NULL);

        // BOUCLE PRINCIPALE: Rendu seulement (input/physics/IA dans les threads)
        while (!g_game_state.game_over && !g_game_state.you_win)
        {
            vga.clear(0, 0, 0);

            g_game_mutex.lock();

            draw_paddle(vga, g_game_state.paddle1_x, g_game_state.paddle1_y, 100, 10, 0, 255, 0);
            draw_paddle(vga, g_game_state.paddle2_x, g_game_state.paddle2_y, 100, 10, 0, 255, 0);
            draw_ball(vga, g_game_state.ball_x, g_game_state.ball_y, 5, 255, 255, 255);

            // Colorier les briques selon la ligne
            for (int i = 0; i < 12; i++)
            {
                if (!g_game_state.bricks[i].destroyed) {
                    int row = i / 6;
                    int col = i % 6;
                    int r = 255, g = 0, b = 0;
                    if (row == 0) { r = 255; g = 128; b = 0; } // orange
                    if (row == 1) { r = 0; g = 128; b = 255; } // bleu clair
                    // Pour plus de couleurs, ajouter d'autres lignes
                    draw_brick(vga, g_game_state.bricks[i].x, g_game_state.bricks[i].y, 90, 25, r, g, b);
                }
            }

            // Afficher le score en haut à gauche
            char score_buf[32];
            snprintf(score_buf, sizeof(score_buf), "Score: %d", g_game_state.score);
            draw_simple_text(vga, 10, 10, score_buf);
            g_game_mutex.unlock();

            vga.swapBuffer();
            thread_yield(); // Laisser les autres threads s'exécuter
        }

        // Afficher écran de fin
        vga.clear(0, 0, 0);
        if (g_game_state.you_win)
            draw_you_win(vga, g_game_state.score);
        else
            draw_game_over(vga, g_game_state.score);
        // Afficher le score sur l'écran de fin
        char score_buf[32];
        snprintf(score_buf, sizeof(score_buf), "Score: %d", g_game_state.score);
        draw_simple_text(vga, 180, 250, score_buf);
        vga.swapBuffer();

        // Écran de fin avec possibilité de recommencer
        Clavier keyboard;
        int wait_r = 1;
        while (wait_r)
        {
            if (g_game_state.you_win)
                draw_you_win(vga, g_game_state.score);
            else
                draw_game_over(vga, g_game_state.score);
            // Afficher le score sur l'écran de fin
            draw_simple_text(vga, 180, 250, score_buf);
            vga.swapBuffer();

            // Afficher un message pour recommencer
            draw_simple_text(vga, 180, 350, "Appuyez sur R pour rejouer");
            vga.swapBuffer();

            // Attendre que l'utilisateur appuie sur 'R' (ou 'r')
            if (keyboard.is_pressed('r') || keyboard.is_pressed('R'))
            {
                wait_r = 0;
            }
            thread_yield();
        }
        // (Les threads précédents continuent, mais la structure d'état est proprement réinitialisée à chaque boucle)
    }
}
