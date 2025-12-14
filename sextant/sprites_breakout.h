#ifndef SPRITES_BREAKOUT_H
#define SPRITES_BREAKOUT_H

#include <drivers/EcranBochs.h>

inline void draw_paddle(EcranBochs &vga, int x, int y, int width, int height, int r, int g, int b)
{
    for (int px = x; px < x + width; px++)
        for (int py = y; py < y + height; py++)
            vga.paint(px, py, r, g, b);
}

inline void draw_ball(EcranBochs &vga, int x, int y, int radius, int r, int g, int b)
{
    for (int dx = -radius; dx <= radius; dx++)
        for (int dy = -radius; dy <= radius; dy++)
            if (dx * dx + dy * dy <= radius * radius)
                vga.paint(x + dx, y + dy, r, g, b);
}

inline void draw_brick(EcranBochs &vga, int x, int y, int width, int height, int r, int g, int b)
{
    for (int px = x; px < x + width; px++)
        for (int py = y; py < y + height; py++)
            vga.paint(px, py, r, g, b);
}

// Fonction simple pour dessiner du texte en "bitmap" gros pixels
inline void draw_simple_text(EcranBochs &vga, int x, int y, const char *text)
{
    int pos_x = x;
    for (const char *p = text; *p; p++) {
        char c = *p;
        // Chaque caractère est un carré blanc simple
        // Pour "GAME OVER" et "YOU WIN" avec des lettres très simples
        
        if (c == 'G') {
            for (int i = 0; i < 20; i++) {
                vga.paint(pos_x, y + i, 255, 255, 255);
                vga.paint(pos_x + 20, y + i, 255, 255, 255);
                if (i == 0 || i == 10 || i == 19) {
                    for (int j = 0; j < 20; j++)
                        vga.paint(pos_x + j, y + i, 255, 255, 255);
                }
            }
            for (int i = 10; i < 20; i++)
                vga.paint(pos_x + 10, y + i, 255, 255, 255);
        }
        else if (c == 'A') {
            for (int i = 0; i < 20; i++) {
                vga.paint(pos_x, y + i, 255, 255, 255);
                vga.paint(pos_x + 20, y + i, 255, 255, 255);
                if (i == 10) {
                    for (int j = 0; j < 20; j++)
                        vga.paint(pos_x + j, y + i, 255, 255, 255);
                }
            }
            for (int j = 0; j < 20; j++)
                vga.paint(pos_x + j, y, 255, 255, 255);
        }
        else if (c == 'M') {
            for (int i = 0; i < 20; i++) {
                vga.paint(pos_x, y + i, 255, 255, 255);
                vga.paint(pos_x + 20, y + i, 255, 255, 255);
            }
            vga.paint(pos_x + 10, y + 5, 255, 255, 255);
        }
        else if (c == 'E') {
            for (int i = 0; i < 20; i++)
                vga.paint(pos_x, y + i, 255, 255, 255);
            for (int j = 0; j < 20; j++) {
                vga.paint(pos_x + j, y, 255, 255, 255);
                vga.paint(pos_x + j, y + 10, 255, 255, 255);
                vga.paint(pos_x + j, y + 19, 255, 255, 255);
            }
        }
        else if (c == 'O') {
            for (int i = 0; i < 20; i++) {
                vga.paint(pos_x, y + i, 255, 255, 255);
                vga.paint(pos_x + 20, y + i, 255, 255, 255);
            }
            for (int j = 0; j < 20; j++) {
                vga.paint(pos_x + j, y, 255, 255, 255);
                vga.paint(pos_x + j, y + 19, 255, 255, 255);
            }
        }
        else if (c == 'V') {
            for (int i = 0; i < 20; i++) {
                vga.paint(pos_x, y + i, 255, 255, 255);
                vga.paint(pos_x + 20, y + i, 255, 255, 255);
            }
        }
        else if (c == 'R') {
            for (int i = 0; i < 20; i++)
                vga.paint(pos_x, y + i, 255, 255, 255);
            for (int j = 0; j < 20; j++) {
                vga.paint(pos_x + j, y, 255, 255, 255);
                vga.paint(pos_x + j, y + 10, 255, 255, 255);
            }
            for (int i = 0; i < 10; i++)
                vga.paint(pos_x + 20, y + i, 255, 255, 255);
            for (int i = 10; i < 20; i++)
                vga.paint(pos_x + 10 + i/2, y + i, 255, 255, 255);
        }
        else if (c == 'Y') {
            for (int i = 0; i < 10; i++) {
                vga.paint(pos_x, y + i, 255, 255, 255);
                vga.paint(pos_x + 20, y + i, 255, 255, 255);
            }
            for (int i = 10; i < 20; i++)
                vga.paint(pos_x + 10, y + i, 255, 255, 255);
        }
        else if (c == 'U') {
            for (int i = 0; i < 20; i++) {
                vga.paint(pos_x, y + i, 255, 255, 255);
                vga.paint(pos_x + 20, y + i, 255, 255, 255);
            }
            for (int j = 0; j < 20; j++)
                vga.paint(pos_x + j, y + 19, 255, 255, 255);
        }
        else if (c == 'W') {
            for (int i = 0; i < 20; i++) {
                vga.paint(pos_x, y + i, 255, 255, 255);
                vga.paint(pos_x + 20, y + i, 255, 255, 255);
            }
            vga.paint(pos_x + 10, y + 15, 255, 255, 255);
        }
        else if (c == 'I') {
            for (int i = 0; i < 20; i++)
                vga.paint(pos_x + 5, y + i, 255, 255, 255);
        }
        else if (c == 'N') {
            for (int i = 0; i < 20; i++) {
                vga.paint(pos_x, y + i, 255, 255, 255);
                vga.paint(pos_x + 20, y + i, 255, 255, 255);
            }
            for (int i = 0; i < 20; i++)
                vga.paint(pos_x + i, y + i, 255, 255, 255);
        }
        else if (c == ' ') {
            // Espace
        }
        
        pos_x += 30;
    }
}

inline void draw_game_over(EcranBochs &vga, int score)
{
    vga.clear(150, 0, 0);  // Fond rouge
    draw_simple_text(vga, 100, 150, "GAME OVER");
}

inline void draw_you_win(EcranBochs &vga, int score)
{
    vga.clear(0, 150, 0);  // Fond vert
    draw_simple_text(vga, 120, 150, "YOU WIN");
}

#endif
