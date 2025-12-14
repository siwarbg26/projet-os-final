#ifndef BALL_H
#define BALL_H

// Classe représentant la balle du jeu Casse Brique
// Gère la position, la vélocité et les collisions

struct Ball
{
    int x;       // Position X en pixels
    int y;       // Position Y en pixels
    int vx;      // Vélocité X (pixels par tick)
    int vy;      // Vélocité Y (pixels par tick)
    int radius;  // Rayon de la balle
    bool active; // La balle est-elle en jeu ?

    // Constructeur
    Ball();

    // Mise à jour position
    void update();

    // Vérifier collision avec les bords
    void checkBorderCollision();

    // Rebondir
    void bounce(bool horizontal);

    // Réinitialiser la balle
    void reset();
};

#endif
