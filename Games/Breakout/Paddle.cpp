#include "Paddle.h"

// Constructeur
Paddle::Paddle(int player)
    : x(300), y(350), width(40), height(10), speed(5),
      player_id(player), moving_left(false), moving_right(false)
{
    if (player == 1)
    {
        x = 580; // Deuxième joueur à droite
    }
}

// Mise à jour position
void Paddle::update()
{
    if (moving_left && x > 0)
    {
        x -= speed;
    }
    if (moving_right && x + width < 640)
    {
        x += speed;
    }

    checkBounds();
}

// Définir direction de déplacement
void Paddle::setDirection(bool left, bool right)
{
    moving_left = left;
    moving_right = right;
}

// Vérifier limites écran
void Paddle::checkBounds()
{
    if (x < 0)
        x = 0;
    if (x + width > 640)
        x = 640 - width;
}

// Obtenir les coordonnées pour collision
int Paddle::getLeft() const { return x; }
int Paddle::getRight() const { return x + width; }
int Paddle::getTop() const { return y; }
int Paddle::getBottom() const { return y + height; }
