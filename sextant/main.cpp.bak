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

// Version simple du jeu Breakout - SANS THREADS!
	// Game Over - afficher message
	vga.clear(0, 0, 0);
	// Afficher écran blanc simple
	for (int x = 150; x < 490; x++)
	{
		for (int y = 150; y < 250; y++)
		{
			vga.paint(x, y, 255, 255, 255);
		}
	}
	vga.swapBuffer();

	// Attendre infini
	while (1)
		;
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
