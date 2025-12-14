#include "Brick.h"

// Constructeur par défaut
Brick::Brick()
    : x(0), y(0), width(32), height(16), health(1),
      destroyed(false), color(2)
{
}

// Constructeur avec paramètres
Brick::Brick(int px, int py, int h)
    : x(px), y(py), width(32), height(16), health(h),
      destroyed(false), color(2)
{ // 2 = couleur cyan/vert
}

// Prendre du dégât
void Brick::takeDamage()
{
    if (health > 0)
    {
        health--;
        if (health <= 0)
        {
            destroyed = true;
        }
    }
}

// Détection collision avec la balle (boîte englobante simple)
bool Brick::checkCollision(int ball_x, int ball_y, int radius) const
{
    if (destroyed)
        return false;

    // Boîte englobante AABB (Axis-Aligned Bounding Box)
    int closest_x = (ball_x < x) ? x : (ball_x > x + width) ? x + width
                                                            : ball_x;
    int closest_y = (ball_y < y) ? y : (ball_y > y + height) ? y + height
                                                             : ball_y;

    int distance_x = ball_x - closest_x;
    int distance_y = ball_y - closest_y;

    return (distance_x * distance_x + distance_y * distance_y) < (radius * radius);
}

// Obtenir coordonnées
int Brick::getLeft() const { return x; }
int Brick::getRight() const { return x + width; }
int Brick::getTop() const { return y; }
int Brick::getBottom() const { return y + height; }
