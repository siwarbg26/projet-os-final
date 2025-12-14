#ifndef SPRITES_BREAKOUT_H
#define SPRITES_BREAKOUT_H

#include <drivers/EcranBochs.h>

// Fonction pour dessiner un paddle (rectangle simple)
inline void draw_paddle(EcranBochs &vga, int x, int y, int width, int height, int r, int g, int b)
{
    for (int px = x; px < x + width; px++)
    {
        for (int py = y; py < y + height; py++)
        {
            vga.paint(px, py, r, g, b);
        }
    }
}

// Fonction pour dessiner une balle (cercle)
inline void draw_ball(EcranBochs &vga, int x, int y, int radius, int r, int g, int b)
{
    for (int dx = -radius; dx <= radius; dx++)
    {
        for (int dy = -radius; dy <= radius; dy++)
        {
            if (dx * dx + dy * dy <= radius * radius)
            {
                vga.paint(x + dx, y + dy, r, g, b);
            }
        }
    }
}

// Fonction pour dessiner une brick (rectangle simple)
inline void draw_brick(EcranBochs &vga, int x, int y, int width, int height, int r, int g, int b)
{
    for (int px = x; px < x + width; px++)
    {
        for (int py = y; py < y + height; py++)
        {
            vga.paint(px, py, r, g, b);
        }
    }
}

// Fonction simple pour afficher du texte bitmap
// Affiche "GAME OVER" avec des lettres
inline void draw_game_over(EcranBochs &vga, int score)
{
    // Remplir l'écran de noir
    vga.clear(0, 0, 0);

    // Dessiner un grand rectangle blanc au centre
    for (int x = 100; x < 540; x++)
    {
        for (int y = 150; y < 250; y++)
        {
            vga.paint(x, y, 255, 255, 255);
        }
    }

    // Dessiner "GAME OVER" en noir sur blanc (très simple)
    // Juste des barres épaisses
    // G
    for (int x = 130; x < 160; x++)
    {
        vga.paint(x, 160, 0, 0, 0);
        vga.paint(x, 230, 0, 0, 0);
    }
    for (int y = 160; y < 230; y++)
    {
        vga.paint(130, y, 0, 0, 0);
        vga.paint(159, y, 0, 0, 0);
    }
    for (int x = 145; x < 160; x++)
        vga.paint(x, 195, 0, 0, 0);

    // A
    for (int x = 170; x < 200; x++)
        vga.paint(x, 160, 0, 0, 0);
    for (int y = 160; y < 230; y++)
    {
        vga.paint(170, y, 0, 0, 0);
        vga.paint(199, y, 0, 0, 0);
    }
    for (int x = 170; x < 200; x++)
        vga.paint(x, 195, 0, 0, 0);
    vga.paint(199, 230, 0, 0, 0);

    // M
    for (int x = 210; x < 240; x++)
    {
        vga.paint(x, 160, 0, 0, 0);
        vga.paint(x, 230, 0, 0, 0);
    }
    for (int y = 160; y < 230; y++)
    {
        vga.paint(210, y, 0, 0, 0);
        vga.paint(239, y, 0, 0, 0);
        vga.paint(224, y, 0, 0, 0);
    }

    // E
    for (int x = 250; x < 280; x++)
    {
        vga.paint(x, 160, 0, 0, 0);
        vga.paint(x, 195, 0, 0, 0);
        vga.paint(x, 230, 0, 0, 0);
    }
    for (int y = 160; y < 230; y++)
        vga.paint(250, y, 0, 0, 0);

    // O
    for (int x = 290; x < 320; x++)
    {
        vga.paint(x, 160, 0, 0, 0);
        vga.paint(x, 230, 0, 0, 0);
    }
    for (int y = 160; y < 230; y++)
    {
        vga.paint(290, y, 0, 0, 0);
        vga.paint(319, y, 0, 0, 0);
    }

    // V
    for (int y = 160; y < 230; y++)
    {
        vga.paint(330 + (y - 160) / 2, y, 0, 0, 0);
        vga.paint(370 - (y - 160) / 2, y, 0, 0, 0);
    }

    // E
    for (int x = 380; x < 410; x++)
    {
        vga.paint(x, 160, 0, 0, 0);
        vga.paint(x, 195, 0, 0, 0);
        vga.paint(x, 230, 0, 0, 0);
    }
    for (int y = 160; y < 230; y++)
        vga.paint(380, y, 0, 0, 0);

    // R
    for (int x = 420; x < 450; x++)
    {
        vga.paint(x, 160, 0, 0, 0);
        vga.paint(x, 195, 0, 0, 0);
    }
    for (int y = 160; y < 230; y++)
        vga.paint(420, y, 0, 0, 0);
    for (int y = 160; y < 195; y++)
        vga.paint(449, y, 0, 0, 0);
    for (int y = 195; y < 230; y++)
        vga.paint(420 + (y - 195), y, 0, 0, 0);
}

// Fonction simple pour afficher "YOU WIN"
inline void draw_you_win(EcranBochs &vga, int score)
{
    // Remplir l'écran de noir
    vga.clear(0, 0, 0);

    // Dessiner un grand rectangle blanc au centre
    for (int x = 100; x < 540; x++)
    {
        for (int y = 150; y < 250; y++)
        {
            vga.paint(x, y, 0, 255, 0); // Vert pour victory!
        }
    }

    // Dessiner "YOU WIN" en noir sur vert
    // Y
    for (int x = 140; x < 160; x++)
        vga.paint(x, 160, 0, 0, 0);
    for (int y = 160; y < 195; y++)
    {
        vga.paint(140, y, 0, 0, 0);
        vga.paint(159, y, 0, 0, 0);
    }
    for (int y = 195; y < 230; y++)
    {
        vga.paint(149, y, 0, 0, 0);
    }

    // O
    for (int x = 170; x < 200; x++)
    {
        vga.paint(x, 160, 0, 0, 0);
        vga.paint(x, 230, 0, 0, 0);
    }
    for (int y = 160; y < 230; y++)
    {
        vga.paint(170, y, 0, 0, 0);
        vga.paint(199, y, 0, 0, 0);
    }

    // U
    for (int y = 160; y < 230; y++)
    {
        vga.paint(210, y, 0, 0, 0);
        vga.paint(239, y, 0, 0, 0);
    }
    for (int x = 210; x < 240; x++)
        vga.paint(x, 230, 0, 0, 0);

    // W
    for (int y = 160; y < 230; y++)
    {
        vga.paint(250, y, 0, 0, 0);
        vga.paint(279, y, 0, 0, 0);
    }
    for (int x = 250; x < 280; x++)
    {
        vga.paint(x, 160, 0, 0, 0);
        vga.paint(x, 230, 0, 0, 0);
    }

    // I
    for (int x = 290; x < 310; x++)
        vga.paint(x, 160, 0, 0, 0);
    for (int y = 160; y < 230; y++)
        vga.paint(300, y, 0, 0, 0);
    for (int x = 290; x < 310; x++)
        vga.paint(x, 230, 0, 0, 0);

    // N
    for (int y = 160; y < 230; y++)
    {
        vga.paint(320, y, 0, 0, 0);
        vga.paint(349, y, 0, 0, 0);
    }
    for (int y = 160; y < 230; y++)
        vga.paint(320 + (y - 160) / 2, y, 0, 0, 0);
}

#endif // SPRITES_BREAKOUT_H