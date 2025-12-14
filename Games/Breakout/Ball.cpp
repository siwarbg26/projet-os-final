#include "Ball.h"

// Initialisation de la balle
Ball::Ball() : x(320), y(200), vx(3), vy(-3), radius(4), active(true)
{
}

// Mise à jour position
void Ball::update()
{
    if (!active)
        return;

    x += vx;
    y += vy;

    checkBorderCollision();
}

// Vérifier collision avec les bords
void Ball::checkBorderCollision()
{
    // Bordure gauche
    if (x - radius <= 0)
    {
        x = radius;
        vx = -vx;
    }

    // Bordure droite (640)
    if (x + radius >= 640)
    {
        x = 640 - radius;
        vx = -vx;
    }

    // Bordure supérieure
    if (y - radius <= 0)
    {
        y = radius;
        vy = -vy;
    }

    // Bordure inférieure (400) - balle perdue
    if (y + radius >= 400)
    {
        active = false;
    }
}

// Rebondir sur un axe
void Ball::bounce(bool horizontal)
{
    if (horizontal)
    {
        vx = -vx;
    }
    else
    {
        vy = -vy;
    }
}

// Réinitialiser la balle
void Ball::reset()
{
    x = 320;
    y = 200;
    vx = 3;
    vy = -3;
    active = true;
}
