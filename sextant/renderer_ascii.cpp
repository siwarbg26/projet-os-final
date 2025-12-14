#include "renderer_ascii.h"

RendererAscii::RendererAscii() : port_(), first_present_(true)
{
    clear();
    screen_.effacerEcran(NOIR);
}

void RendererAscii::clear()
{
    for (int y = 0; y < Height; ++y)
    {
        for (int x = 0; x < Width; ++x)
        {
            buffer[y][x] = ' ';
        }
    }
}

void RendererAscii::drawRect(int x, int y, int w, int h, char c)
{
    if (x < 0)
    {
        w += x;
        x = 0;
    }
    if (y < 0)
    {
        h += y;
        y = 0;
    }
    if (x + w > Width)
        w = Width - x;
    if (y + h > Height)
        h = Height - y;
    if (w <= 0 || h <= 0)
        return;
    for (int j = 0; j < h; ++j)
    {
        for (int i = 0; i < w; ++i)
        {
            buffer[y + j][x + i] = c;
        }
    }
}

void RendererAscii::drawPoint(int x, int y, char c)
{
    if (x < 0 || x >= Width || y < 0 || y >= Height)
        return;
    buffer[y][x] = c;
}

void RendererAscii::drawText(int x, int y, const char *s)
{
    if (y < 0 || y >= Height)
        return;
    int cx = x;
    for (const char *p = s; p && *p; ++p)
    {
        if (cx >= 0 && cx < Width)
            buffer[y][cx] = *p;
        ++cx;
        if (cx >= Width)
            break;
    }
}

void RendererAscii::present()
{
    if (first_present_)
    {
        port_.ecrireMot("\x1b[2J\x1b[?25l");
        first_present_ = false;
    }

    port_.ecrireMot("\x1b[H");

    char line[Width + 3];
    for (int y = 0; y < Height; ++y)
    {
        for (int x = 0; x < Width; ++x)
        {
            const char ch = buffer[y][x];
            line[x] = ch;
            screen_.afficherCaractere(y, x, BLANC, NOIR, ch);
        }
        line[Width] = '\r';
        line[Width + 1] = '\n';
        line[Width + 2] = '\0';
        port_.ecrireMot(line);
    }
}
