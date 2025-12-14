#include <hal/multiboot.h>
#include <drivers/Ecran.h>

// TP2
#include <sextant/interruptions/idt.h>
#include <sextant/interruptions/irq.h>
#include <sextant/interruptions/handler/handler_tic.h>
#include <sextant/interruptions/handler/handler_clavier.h>
#include <drivers/timer.h>
#include <drivers/Clavier.h>

// TP3
#include <sextant/memoire/memoire.h>

// TP4
#include <sextant/ordonnancements/cpu_context.h>
#include <sextant/ordonnancements/preemptif/thread.h>
#include <sextant/types.h>

#include <sextant/Synchronisation/Spinlock/Spinlock.h>

#include <hal/pci.h>
#include <drivers/vga.h>
#include <drivers/EcranBochs.h>

#include <sextant/sprite.h>
#include <Games/Breakout/BreakoutGame.h>
#include <drivers/PortSerie.h>

// Placement-new helper (éviter la dépendance libstdc++)
inline void *operator new(size_t, void *p) { return p; }

extern char __e_kernel, __b_kernel, __b_data, __e_data, __b_stack, __e_load;
extern vaddr_t bootstrap_stack_bottom;
extern size_t bootstrap_stack_size;

extern BreakoutGame *g_game;
extern EcranBochs *g_vga;
extern void breakout_game_thread_main(void *arg);
extern void paddle1_thread_main(void *arg);
extern void paddle2_thread_main(void *arg);

static unsigned char g_breakout_storage[sizeof(BreakoutGame)];

void demo_simple_serie()
{
	PortSerie port;
	port.ecrireMot("\n\n=================================\n");
	port.ecrireMot("   TEST VGA - COMPILATION OK!   \n");
	port.ecrireMot("=================================\n\n");

	for (int compteur = 0; compteur <= 100; compteur += 2)
	{
		port.ecrireMot("[");
		for (int i = 0; i < compteur / 2; i++)
		{
			port.ecrireMot("=");
		}
		for (int i = compteur / 2; i < 50; i++)
		{
			port.ecrireMot(" ");
		}
		port.ecrireMot("] ");

		char pct[10];
		pct[0] = '0' + (compteur / 10);
		pct[1] = '0' + (compteur % 10);
		pct[2] = '%';
		pct[3] = '\n';
		pct[4] = '\0';
		port.ecrireMot(pct);

		for (volatile int i = 0; i < 10000000; i++)
			; // Délai
	}

	port.ecrireMot("\n");
	port.ecrireMot("=================================\n");
	port.ecrireMot("  DEMO VGA TERMINEE - SUCCESS!  \n");
	port.ecrireMot("=================================\n");
	port.ecrireMot("\nLa compilation VGA fonctionne!\n");
	port.ecrireMot("Vous pouvez maintenant:\n");
	port.ecrireMot("1. Utiliser demo_vga() avec VNC\n");
	port.ecrireMot("2. Utiliser demo_bochs_8/32()\n");
	port.ecrireMot("3. Lancer le jeu Breakout\n\n");

	while (1)
		; // Boucle infinie
}

void demo_vga()
{
	set_vga_mode13();
	set_palette_vga(palette_vga);
	ui16_t offset = 0;

	while (1)
	{
		// Fond coloré qui change
		clear_vga_screen((offset / 10) % 256);

		// Plusieurs carrés colorés animés
		plot_square(offset % 280, 20, 40, 15);			// Carré blanc qui bouge
		plot_square((offset * 2) % 280, 80, 35, 12);	// Carré rouge rapide
		plot_square(150, 120, 30, 10);					// Carré vert fixe
		plot_square((280 - offset) % 280, 140, 25, 14); // Carré jaune inverse

		// Sprite au centre
		draw_sprite(sprite_door_data, 32, 32, 144, 84);

		offset = (offset + 3) % 280;
	}
}

void demo_bochs_8()
{
	ui16_t WIDTH = 640, HEIGHT = 400;
	EcranBochs vga(WIDTH, HEIGHT, VBE_MODE::_8);
	const char SPEED = 2;
	Clavier c;

	vga.init();
	vga.clear(0);
	vga.set_palette(palette_vga);
	vga.plot_palette(0, 0, 25);

	int x = 0, y = 0;
	while (true)
	{
		if (c.is_pressed(AZERTY::K_Z))
		{
			y -= SPEED;
			if (y < 0)
				y += HEIGHT;
		}
		if (c.is_pressed(AZERTY::K_Q))
		{
			x -= SPEED;
			if (x < 0)
				x += WIDTH;
		}
		if (c.is_pressed(AZERTY::K_S))
		{
			y = (y + SPEED) % HEIGHT;
		}
		if (c.is_pressed(AZERTY::K_D))
		{
			x = (x + SPEED) % WIDTH;
		}
		vga.clear(1);
		vga.plot_sprite(sprite_data, SPRITE_WIDTH, SPRITE_HEIGHT, x, y);
		vga.swapBuffer();
	}
}

void demo_bochs_32()
{
	EcranBochs vga(640, 400, VBE_MODE::_32);
	vga.init();
	ui8_t offset = 0;
	while (true)
	{
		for (int y = 0; y < vga.getHeight(); y++)
		{
			for (int x = 0; x < vga.getWidth(); x++)
			{
				vga.paint(x, y,
						  (~x << y % 3) + offset & y,
						  ~offset * (x & ~y),
						  offset | (~y < 2 - x % 16));
			}
		}
		++offset;
	}
}

// Structure simple pour les briques du jeu
struct SimpleBrick
{
	int x, y;
	bool destroyed;
};

// Version simple du jeu Breakout - SANS THREADS!
void simple_breakout()
{
	EcranBochs vga(640, 400, VBE_MODE::_32);
	vga.init();

	// Positions et dimensions
	int paddle1_x = 250, paddle1_y = 20;
	int paddle2_x = 250, paddle2_y = 370;
	int ball_x = 320, ball_y = 200;
	int ball_dx = 1, ball_dy = 1;
	int score = 0;

	// Couleurs RGB
	int paddle_r = 0, paddle_g = 255, paddle_b = 0;
	int ball_r = 255, ball_g = 255, ball_b = 255;
	int brick_r = 255, brick_g = 0, brick_b = 0;
	int bg_r = 0, bg_g = 0, bg_b = 0;

	// Créer les briques (structure simple)
	SimpleBrick bricks[12];
	for (int row = 0; row < 2; row++)
	{
		for (int col = 0; col < 6; col++)
		{
			bricks[row * 6 + col].x = 30 + col * 105;
			bricks[row * 6 + col].y = 80 + row * 35;
			bricks[row * 6 + col].destroyed = false;
		}
	}

	Clavier keyboard;
	int frame_counter = 0;
	int keyboard_counter = 0; // Compteur pour lire le clavier moins souvent

	while (1)
	{
		// Effacer l'écran
		vga.clear(bg_r, bg_g, bg_b);

		// Contrôle paddle 1 avec clavier (Z et D) - SEULEMENT AVEC CLAVIER
		// Lire le clavier tous les 3 frames seulement pour éviter les faux positifs
		keyboard_counter++;
		if (keyboard_counter > 3)
		{
			keyboard_counter = 0;
			if (keyboard.is_pressed(AZERTY::K_Z) && paddle1_x > 0)
				paddle1_x -= 5;
			if (keyboard.is_pressed(AZERTY::K_D) && paddle1_x < 540)
				paddle1_x += 5;
		}

		// Paddle 2 suit automatiquement la balle (vitesse lente)
		if (ball_x < paddle2_x && paddle2_x > 0)
			paddle2_x -= 2;
		if (ball_x > paddle2_x + 100 && paddle2_x < 540)
			paddle2_x += 2;

		// Mouvement de la balle (ralenti)
		frame_counter++;
		if (frame_counter > 5)
		{
			ball_x += ball_dx;
			ball_y += ball_dy;
			frame_counter = 0;
		}

		// Rebond sur les murs gauche/droit - CLAMP pour éviter de sortir
		if (ball_x <= 10)
			ball_dx = 1;
		if (ball_x >= 630)
			ball_dx = -1;

		// Rebond sur paddle 1 (haut)
		if (ball_y <= paddle1_y + 20 && ball_x > paddle1_x && ball_x < paddle1_x + 100)
		{
			ball_dy = 1;
			score++;
		}

		// Rebond sur paddle 2 (bas)
		if (ball_y >= paddle2_y - 20 && ball_x > paddle2_x && ball_x < paddle2_x + 100)
		{
			ball_dy = -1;
			score++;
		}

		// Collision avec les briques
		for (int i = 0; i < 12; i++)
		{
			if (!bricks[i].destroyed)
			{
				int bx = bricks[i].x;
				int by = bricks[i].y;
				if (ball_x > bx && ball_x < bx + 90 && ball_y > by && ball_y < by + 25)
				{
					bricks[i].destroyed = true;
					ball_dy = -ball_dy;
					score += 10;
				}
			}
		} // Réinitialiser si la balle sort
		if (ball_y < 0 || ball_y > 400)
		{
			ball_x = 320;
			ball_y = 200;
			ball_dx = 1;
			ball_dy = 1;
		}

		// Dessiner les éléments du jeu
		// Paddle 1 (raquette du haut) - rectangle
		for (int x = paddle1_x; x < paddle1_x + 100; x++)
		{
			for (int y = paddle1_y; y < paddle1_y + 10; y++)
			{
				vga.paint(x, y, paddle_r, paddle_g, paddle_b);
			}
		}

		// Paddle 2 (raquette du bas) - rectangle
		for (int x = paddle2_x; x < paddle2_x + 100; x++)
		{
			for (int y = paddle2_y; y < paddle2_y + 10; y++)
			{
				vga.paint(x, y, paddle_r, paddle_g, paddle_b);
			}
		}

		// Balle - cercle simple
		for (int dx = -5; dx <= 5; dx++)
		{
			for (int dy = -5; dy <= 5; dy++)
			{
				if (dx * dx + dy * dy <= 25)
				{
					vga.paint(ball_x + dx, ball_y + dy, ball_r, ball_g, ball_b);
				}
			}
		}

		// Briques - SEULEMENT LES BRIQUES NON DÉTRUITES
		for (int i = 0; i < 12; i++)
		{
			if (!bricks[i].destroyed)
			{
				int bx = bricks[i].x;
				int by = bricks[i].y;
				for (int x = bx; x < bx + 90; x++)
				{
					for (int y = by; y < by + 25; y++)
					{
						vga.paint(x, y, brick_r, brick_g, brick_b);
					}
				}
			}
		}

		vga.swapBuffer();
	}
}

extern "C" void Sextant_main(unsigned long magic, unsigned long addr)
{
	Ecran ecran;
	Timer timer;

	idt_setup();
	irq_setup();
	timer.i8254_set_frequency(1000);
	irq_set_routine(IRQ_TIMER, ticTac);

	asm volatile("sti\n");
	irq_set_routine(IRQ_KEYBOARD, handler_clavier);

	multiboot_info_t *mbi = (multiboot_info_t *)addr;
	mem_setup(&__e_kernel, (mbi->mem_upper << 10) + (1 << 20), &ecran);

	ecran.effacerEcran(NOIR);
	ecran.afficherMot(0, 0, "Demarrage test VGA...");

	// initialize pci bus to detect GPU address
	checkBus(0);

	// Jeu Breakout SIMPLE - SANS THREADS!
	simple_breakout();

	// Autres démos :
	// demo_vga(); // Carres colores qui bougent!
	// demo_bochs_32();  // Bochs 32 bits avec animation RGB
}
