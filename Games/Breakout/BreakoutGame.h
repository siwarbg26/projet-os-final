#ifndef BREAKOUT_GAME_H
#define BREAKOUT_GAME_H

#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include "sextant/Synchronisation/Mutex/Mutex.h"
#include "sextant/Synchronisation/Semaphore/Semaphore.h"

// Constantes du jeu
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 400
#define GRID_ROWS 2
#define GRID_COLS 4
#define BRICK_WIDTH 100
#define BRICK_HEIGHT 20

// États du jeu
enum GameState
{
    GAME_MENU,
    GAME_RUNNING,
    GAME_PAUSED,
    GAME_OVER,
    GAME_WIN
};

// Classe principale du moteur de jeu
class BreakoutGame
{
public:
    Ball ball;
    Paddle paddle1;
    Paddle paddle2;
    Brick* bricks;  // Grille de briques

    int score1;      // Score joueur 1
    int score2;      // Score joueur 2
    int lives;       // Vies restantes
    GameState state; // État actuel du jeu

    Mutex game_mutex;     // Synchronisation accès à l'état du jeu
    Semaphore update_sem; // Synchronisation des mises à jour
    // Constructeur et destructeur
    BreakoutGame();
    ~BreakoutGame();

    // Initialiser le jeu
    void init();

    // Boucle principale du jeu (appelée par le thread game)
    void gameLoop();

    // Mise à jour logique du jeu
    void update();

    // Rendu du jeu
    void render();

    // Gestion des collisions
    void handleCollisions();

    // Détection collision balle-brique
    bool checkBallBrickCollision();

    // Détection collision balle-raquette
    bool checkBallPaddleCollision();

    // Afficher les briques
    void drawBricks();

    // Afficher la balle
    void drawBall();

    // Afficher les raquettes
    void drawPaddles();

    // Afficher l'interface (score, vies)
    void drawUI();

    // Obtenir l'état du jeu
    GameState getState() const;

    // Terminer le jeu
    void endGame();
};

// Fonctions de thread
void breakout_game_thread_main();
void paddle1_thread_main();
void paddle2_thread_main();

#endif
