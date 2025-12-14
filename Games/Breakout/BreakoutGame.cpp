#include "BreakoutGame.h"
#include "InputManager.h"
#include "drivers/EcranBochs.h"
#include "sextant/sprite.h"

#ifndef DISABLE_ASCII_RENDERER
#define ASCII_RENDERER_ENABLED 1
#else
#define ASCII_RENDERER_ENABLED 0
#endif

#if ASCII_RENDERER_ENABLED
#include <sextant/renderer_ascii.h>
#endif

// Placement-new helper (pas de libstdc++)
inline void *operator new(size_t, void *p) { return p; }

// Variable globale du jeu
BreakoutGame *g_game = 0;
EcranBochs *g_vga = 0;

// Stockage brut pour l'écran, évite new/delete
#if !ASCII_RENDERER_ENABLED
static unsigned char g_vga_storage[sizeof(EcranBochs)];
static bool g_vga_initialized = false;
#endif

#if ASCII_RENDERER_ENABLED
static unsigned char g_ascii_renderer_storage[sizeof(RendererAscii)];
static RendererAscii *g_ascii_renderer = 0;

static inline int ascii_inner_width()
{
    return RendererAscii::Width - 2;
}

static inline int ascii_inner_height()
{
    return RendererAscii::Height - 4;
}

static inline int ascii_x(int pixel)
{
    int value = 1 + (pixel * ascii_inner_width()) / SCREEN_WIDTH;
    if (value < 1)
        value = 1;
    if (value > RendererAscii::Width - 2)
        value = RendererAscii::Width - 2;
    return value;
}

static inline int ascii_y(int pixel)
{
    int value = 3 + (pixel * ascii_inner_height()) / SCREEN_HEIGHT;
    if (value < 3)
        value = 3;
    if (value > RendererAscii::Height - 2)
        value = RendererAscii::Height - 2;
    return value;
}

static inline int ascii_span_x(int span)
{
    int value = (span * ascii_inner_width()) / SCREEN_WIDTH;
    return (value < 1) ? 1 : value;
}

static inline int ascii_span_y(int span)
{
    int value = (span * ascii_inner_height()) / SCREEN_HEIGHT;
    return (value < 1) ? 1 : value;
}

static void to_string(int value, char *out)
{
    int idx = 0;
    if (value == 0)
    {
        out[idx++] = '0';
    }
    else
    {
        if (value < 0)
        {
            out[idx++] = '-';
            value = -value;
        }
        char tmp[12];
        int len = 0;
        while (value > 0 && len < 12)
        {
            tmp[len++] = char('0' + (value % 10));
            value /= 10;
        }
        while (len > 0)
        {
            out[idx++] = tmp[--len];
        }
    }
    out[idx] = '\0';
}

static void render_ascii(const BreakoutGame &game)
{
    if (!g_ascii_renderer)
        return;

    RendererAscii &ascii = *g_ascii_renderer;
    ascii.clear();

    ascii.drawRect(0, 0, RendererAscii::Width, 1, '#');
    ascii.drawRect(0, RendererAscii::Height - 1, RendererAscii::Width, 1, '#');
    ascii.drawRect(0, 0, 1, RendererAscii::Height, '#');
    ascii.drawRect(RendererAscii::Width - 1, 0, 1, RendererAscii::Height, '#');

    char number[16];
    ascii.drawText(2, 1, "P1:");
    to_string(game.score1, number);
    ascii.drawText(6, 1, number);
    ascii.drawText(12, 1, "P2:");
    to_string(game.score2, number);
    ascii.drawText(16, 1, number);
    ascii.drawText(24, 1, "Lives:");
    to_string(game.lives, number);
    ascii.drawText(32, 1, number);

    ascii.drawText(2, 2, "P1: Z/S  P2: </>  SPACE pause");

    for (int i = 0; i < GRID_ROWS * GRID_COLS; ++i)
    {
        if (!game.bricks[i].destroyed)
        {
            int left = ascii_x(game.bricks[i].getLeft());
            int top = ascii_y(game.bricks[i].getTop());
            int width = ascii_span_x(game.bricks[i].getRight() - game.bricks[i].getLeft());
            int height = ascii_span_y(game.bricks[i].getBottom() - game.bricks[i].getTop());
            char brick_char = (game.bricks[i].health > 1) ? '@' : '#';
            ascii.drawRect(left, top, width, height, brick_char);
        }
    }

    int p1_left = ascii_x(game.paddle1.getLeft());
    int p1_top = ascii_y(game.paddle1.getTop());
    int p1_width = ascii_span_x(game.paddle1.getRight() - game.paddle1.getLeft());
    int p1_height = ascii_span_y(game.paddle1.getBottom() - game.paddle1.getTop());
    ascii.drawRect(p1_left, p1_top, p1_width, p1_height, '=');

    int p2_left = ascii_x(game.paddle2.getLeft());
    int p2_top = ascii_y(game.paddle2.getTop());
    int p2_width = ascii_span_x(game.paddle2.getRight() - game.paddle2.getLeft());
    int p2_height = ascii_span_y(game.paddle2.getBottom() - game.paddle2.getTop());
    ascii.drawRect(p2_left, p2_top, p2_width, p2_height, '-');

    if (game.ball.active)
    {
        ascii.drawPoint(ascii_x(game.ball.x), ascii_y(game.ball.y), 'o');
    }

    ascii.present();
}
#endif

// ============================================================================
// CONSTRUCTEUR ET INITIALISATION
// ============================================================================

BreakoutGame::BreakoutGame()
    : paddle1(0), paddle2(1), score1(0), score2(0), lives(3),
      state(GAME_MENU)
{
}

BreakoutGame::~BreakoutGame()
{
}

// Initialiser le jeu
void BreakoutGame::init()
{
#if !ASCII_RENDERER_ENABLED
    // Initialiser le driver VGA si nécessaire (instance statique pour éviter l'allocation dynamique)
    if (!g_vga_initialized)
    {
        g_vga = new (g_vga_storage) EcranBochs(SCREEN_WIDTH, SCREEN_HEIGHT, VBE_MODE::_8);
        g_vga->init();
        g_vga->set_palette(palette_vga);
        g_vga_initialized = true;
    }
#endif

#if ASCII_RENDERER_ENABLED
    if (!g_ascii_renderer)
    {
        g_ascii_renderer = new (g_ascii_renderer_storage) RendererAscii();
    }
#endif

    // Créer la grille de briques
    for (int row = 0; row < GRID_ROWS; row++)
    {
        for (int col = 0; col < GRID_COLS; col++)
        {
            int index = row * GRID_COLS + col;
            int x = col * BRICK_WIDTH;
            int y = row * BRICK_HEIGHT + 40;
            bricks[index] = Brick(x, y, 1);
        }
    }

    ball.reset();
    state = GAME_RUNNING;

    // Initialiser le gestionnaire d'entrées
    InputManager::init();
}

// ============================================================================
// BOUCLE PRINCIPALE DU JEU
// ============================================================================

void BreakoutGame::gameLoop()
{
    unsigned int frame_count = 0;

    while (state != GAME_OVER && state != GAME_WIN)
    {
        // Mettre à jour les entrées
        InputManager::update();

        // Mettre à jour direction des raquettes
        paddle1.setDirection(InputManager::isPlayer1Left(), InputManager::isPlayer1Right());
        paddle2.setDirection(InputManager::isPlayer2Left(), InputManager::isPlayer2Right());

        // Verrouiller accès à l'état du jeu
        game_mutex.lock();

        // Mise à jour logique
        update();
        handleCollisions();

        // Rendu
        render();

        game_mutex.unlock();

        frame_count++;
    }
}

// Mise à jour logique du jeu
void BreakoutGame::update()
{
    if (state != GAME_RUNNING)
        return;

    // Mettre à jour les entités
    ball.update();
    paddle1.update();
    paddle2.update();

    // Vérifier si la balle est perdue
    if (!ball.active)
    {
        lives--;
        if (lives <= 0)
        {
            state = GAME_OVER;
        }
        else
        {
            ball.reset();
        }
    }

    // Vérifier victoire (toutes les briques détruites)
    bool all_destroyed = true;
    for (int i = 0; i < GRID_ROWS * GRID_COLS; i++)
    {
        if (!bricks[i].destroyed)
        {
            all_destroyed = false;
            break;
        }
    }
    if (all_destroyed)
    {
        state = GAME_WIN;
    }
}

// Gestion des collisions
void BreakoutGame::handleCollisions()
{
    if (!ball.active)
        return;

    // Collision balle-raquette 1
    if (ball.x > paddle1.getLeft() && ball.x < paddle1.getRight() &&
        ball.y + ball.radius >= paddle1.getTop() &&
        ball.y - ball.radius <= paddle1.getBottom())
    {
        ball.bounce(false); // Rebond vertical
        score1 += 10;
    }

    // Collision balle-raquette 2
    if (ball.x > paddle2.getLeft() && ball.x < paddle2.getRight() &&
        ball.y + ball.radius >= paddle2.getTop() &&
        ball.y - ball.radius <= paddle2.getBottom())
    {
        ball.bounce(false);
        score2 += 10;
    }

    // Collision balle-briques
    for (int i = 0; i < GRID_ROWS * GRID_COLS; i++)
    {
        if (!bricks[i].destroyed && bricks[i].checkCollision(ball.x, ball.y, ball.radius))
        {
            bricks[i].takeDamage();
            ball.bounce(true); // Rebond horizontal (simplifié)
            score1 += 50;      // Attribuer le point au joueur 1 (ou au joueur qui a renvoyé)
            break;
        }
    }
}

// ============================================================================
// RENDU
// ============================================================================

void BreakoutGame::render()
{
#if !ASCII_RENDERER_ENABLED
    if (g_vga)
    {
        g_vga->clear(0);
        drawBricks();
        drawPaddles();
        drawBall();
        drawUI();
        g_vga->swapBuffer();
    }
#endif

#if ASCII_RENDERER_ENABLED
    render_ascii(*this);
#endif
}

// Afficher les briques
void BreakoutGame::drawBricks()
{
    if (!g_vga)
        return;

    for (int i = 0; i < GRID_ROWS * GRID_COLS; i++)
    {
        if (!bricks[i].destroyed)
        {
            // Dessiner un rectangle pour chaque brique
            // Couleur dépend du niveau de santé
            unsigned char color = (bricks[i].health == 1) ? 2 : 3;

            for (int y = bricks[i].getTop(); y < bricks[i].getBottom(); y++)
            {
                for (int x = bricks[i].getLeft(); x < bricks[i].getRight(); x++)
                {
                    g_vga->paint(x, y, color, color, color);
                }
            }
        }
    }
}

// Afficher la balle
void BreakoutGame::drawBall()
{
    if (!g_vga || !ball.active)
        return;

    // Dessiner un carré pour la balle
    for (int dy = -ball.radius; dy <= ball.radius; dy++)
    {
        for (int dx = -ball.radius; dx <= ball.radius; dx++)
        {
            int px = ball.x + dx;
            int py = ball.y + dy;
            if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT)
            {
                g_vga->paint(px, py, 15, 15, 15); // Blanc pour la balle
            }
        }
    }
}

// Afficher les raquettes
void BreakoutGame::drawPaddles()
{
    if (!g_vga)
        return;

    // Dessiner raquette 1 (bleue)
    for (int y = paddle1.getTop(); y < paddle1.getBottom(); y++)
    {
        for (int x = paddle1.getLeft(); x < paddle1.getRight(); x++)
        {
            g_vga->paint(x, y, 1, 1, 1); // Bleu
        }
    }

    // Dessiner raquette 2 (rouge)
    for (int y = paddle2.getTop(); y < paddle2.getBottom(); y++)
    {
        for (int x = paddle2.getLeft(); x < paddle2.getRight(); x++)
        {
            g_vga->paint(x, y, 4, 4, 4); // Rouge
        }
    }
}

// Afficher l'interface (score, vies)
void BreakoutGame::drawUI()
{
    if (!g_vga)
        return;

    // Afficher bordures du jeu
    // Haut
    for (int x = 0; x < SCREEN_WIDTH; x++)
    {
        g_vga->paint(x, 0, 7, 7, 7); // Gris clair
        g_vga->paint(x, 30, 7, 7, 7);
    }

    // Bas
    for (int x = 0; x < SCREEN_WIDTH; x++)
    {
        g_vga->paint(x, SCREEN_HEIGHT - 1, 7, 7, 7);
    }

    // Gauche et droite
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        g_vga->paint(0, y, 7, 7, 7);
        g_vga->paint(SCREEN_WIDTH - 1, y, 7, 7, 7);
    }
}

// Obtenir l'état du jeu
GameState BreakoutGame::getState() const
{
    return state;
}

// Terminer le jeu
void BreakoutGame::endGame()
{
    state = GAME_OVER;
}

// ============================================================================
// THREADS FONCTIONS
// ============================================================================

// Thread principal du jeu
void breakout_game_thread_main(void * /*arg*/)
{
    if (g_game)
    {
        g_game->gameLoop();
    }
}

// Thread de la raquette 1
void paddle1_thread_main(void * /*arg*/)
{
    while (g_game && g_game->getState() != GAME_OVER)
    {
        g_game->game_mutex.lock();
        g_game->paddle1.update();
        g_game->game_mutex.unlock();
    }
}

// Thread de la raquette 2
void paddle2_thread_main(void * /*arg*/)
{
    while (g_game && g_game->getState() != GAME_OVER)
    {
        g_game->game_mutex.lock();
        g_game->paddle2.update();
        g_game->game_mutex.unlock();
    }
}
