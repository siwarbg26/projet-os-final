#pragma once

#include "../drivers/PortSerie.h"
#include "../drivers/Ecran.h"
#include "types.h"

class RendererAscii
{
public:
    RendererAscii();
    void clear();
    void drawRect(int x, int y, int w, int h, char c);
    void drawPoint(int x, int y, char c);
    void drawText(int x, int y, const char *s);
    void present();

    static const int Width = 80;
    static const int Height = 25;

private:
    char buffer[Height][Width];
    PortSerie port_;
    bool first_present_;
    Ecran screen_;
};
