#ifndef BREAKOUT_INPUT_MANAGER_H
#define BREAKOUT_INPUT_MANAGER_H

// Gestionnaire des entrées clavier pour Breakout
// Supporte 2 joueurs simultanés avec leurs propres contrôles

class InputManager
{
public:
    // Initialiser le gestionnaire d'entrées
    static void init();

    // Mettre à jour l'état des touches
    static void update();

    // Vérifier si une touche est appuyée
    static bool isKeyPressed(unsigned char scancode);

    // Obtenir l'état du joueur 1 (Z=gauche, S=droite)
    static bool isPlayer1Left();
    static bool isPlayer1Right();

    // Obtenir l'état du joueur 2 (flèches)
    static bool isPlayer2Left();
    static bool isPlayer2Right();

    // Codes de touche du clavier
    enum KeyCode
    {
        KEY_Z = 0x2C,     // Z - Joueur 1 gauche
        KEY_S = 0x1F,     // S - Joueur 1 droite
        KEY_LEFT = 0x4B,  // Flèche gauche - Joueur 2 gauche
        KEY_RIGHT = 0x4D, // Flèche droite - Joueur 2 droite
        KEY_SPACE = 0x39, // Barre espace - Action/Pause
        KEY_ESC = 0x01,   // Echap - Quitter
    };

private:
    // Tableau d'état des touches (256 scancodes possibles)
    static bool key_states[256];

    // Dernier scancode reçu
    static unsigned char last_scancode;
};

#endif
