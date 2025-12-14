#ifndef PADDLE_H
#define PADDLE_H

// Classe représentant une raquette du jeu
// Supporte 2 joueurs simultanés avec contrôle clavier indépendant

struct Paddle
{
    int x;         // Position X en pixels
    int y;         // Position Y en pixels
    int width;     // Largeur de la raquette
    int height;    // Hauteur de la raquette
    int speed;     // Vitesse de déplacement
    int player_id; // 0 = Joueur 1 (Z/S), 1 = Joueur 2 (Flèches)
    bool moving_left;
    bool moving_right;

    // Constructeur
    Paddle(int player);

    // Mise à jour position en fonction des entrées
    void update();

    // Définir direction de déplacement
    void setDirection(bool left, bool right);

    // Vérifier limites écran
    void checkBounds();

    // Obtenir la position pour collision
    int getLeft() const;
    int getRight() const;
    int getTop() const;
    int getBottom() const;
};

#endif
