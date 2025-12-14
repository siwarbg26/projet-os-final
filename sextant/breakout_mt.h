#ifndef BREAKOUT_MT_H
#define BREAKOUT_MT_H

/**
 * @file breakout_mt.h
 *
 * VERSION MULTITHREADÉE du jeu Breakout
 *
 * ARCHITECTURE:
 * - Thread Principal (main) : Gestion du rendu
 * - Thread Physique : Déplacement balle + collision
 * - Thread IA : Contrôle paddle2 (raquette IA)
 * - Thread Input : Lecture clavier pour paddle1
 *
 * SYNCHRONISATION:
 * - Mutex pour protéger l'état partagé
 * - Ordonnanceur preemptif du kernel
 *
 * JUSTIFICATION ORDONNANCEUR:
 * Avec plusieurs threads accédant à l'état du jeu, il est crucial d'avoir
 * un ordonnanceur preemptif pour :
 * - Éviter que un thread bloque les autres
 * - Partager équitablement le CPU entre input, physique, IA et rendu
 * - Gérer les interruptions (clavier, timer) correctement
 */

// Forward declaration
void threaded_breakout();

#endif
