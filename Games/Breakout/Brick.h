#ifndef BRICK_H
#define BRICK_H

// Classe représentant une brique du jeu
// Supporte une grille de briques avec destruction progressive

struct Brick
{
    int x;          // Position X en pixels
    int y;          // Position Y en pixels
    int width;      // Largeur (32 pixels par défaut)
    int height;     // Hauteur (16 pixels par défaut)
    int health;     // Points de vie (1-3)
    bool destroyed; // La brique est-elle détruite ?
    int color;      // Couleur VGA

    // Constructeur par défaut
    Brick();

    // Constructeur avec paramètres
    Brick(int px, int py, int h = 1);

    // Prendre du dégât
    void takeDamage();

    // Vérifier si collision avec balle (boîte englobante)
    bool checkCollision(int ball_x, int ball_y, int radius) const;

    // Obtenir coordonnées pour affichage
    int getLeft() const;
    int getRight() const;
    int getTop() const;
    int getBottom() const;
};

#endif
