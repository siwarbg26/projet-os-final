#include "InputManager.h"
#include "drivers/Clavier.h"

// Instance clavier utilisée par le jeu
static Clavier g_clavier;

// Variables statiques
bool InputManager::key_states[256] = {false};
unsigned char InputManager::last_scancode = 0;

// Initialiser le gestionnaire d'entrées
void InputManager::init()
{
    // Réinitialiser tous les états des touches
    for (int i = 0; i < 256; i++)
    {
        key_states[i] = false;
    }
}

// Mettre à jour l'état des touches
void InputManager::update()
{
    // Scancodes utilisés par le jeu
    const unsigned char codes[] = {0x2C, /* Z */ 0x1F, /* S */ 0x4B, /* LEFT */ 0x4D, /* RIGHT */ 0x39, /* SPACE */ 0x01 /* ESC */};

    for (unsigned int i = 0; i < sizeof(codes); i++)
    {
        unsigned char code = codes[i];
        key_states[code] = g_clavier.is_pressed(code);
    }
}

// Vérifier si une touche est appuyée
bool InputManager::isKeyPressed(unsigned char scancode)
{
    return key_states[scancode & 0x7F];
}

// Joueur 1 - Contrôle Z/S
bool InputManager::isPlayer1Left()
{
    // Z = gauche
    return isKeyPressed(0x2C); // Scancode Z
}

bool InputManager::isPlayer1Right()
{
    // S = droite
    return isKeyPressed(0x1F); // Scancode S
}

// Joueur 2 - Contrôle flèches
bool InputManager::isPlayer2Left()
{
    // Flèche gauche
    return isKeyPressed(0x4B); // Scancode Flèche gauche
}

bool InputManager::isPlayer2Right()
{
    // Flèche droite
    return isKeyPressed(0x4D); // Scancode Flèche droite
}
