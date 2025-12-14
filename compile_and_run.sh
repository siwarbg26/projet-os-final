#!/bin/bash

echo "======================================"
echo "  COMPILATION ET TEST VGA SEXTANT"
echo "======================================"
echo ""

# Arrêter QEMU s'il tourne
pkill -9 qemu 2>/dev/null
sleep 1

# Nettoyer et compiler
echo "🧹 Nettoyage..."
make clean > /dev/null 2>&1

echo "🔨 Compilation en cours..."
make 2>&1 | grep -E "(error:|Error:|fatal:)" && {
    echo "❌ ERREUR DE COMPILATION!"
    exit 1
}

echo "✅ Compilation réussie!"
echo ""
echo "🚀 Lancement du système..."
echo "📺 La sortie s'affiche ci-dessous:"
echo "======================================"
echo ""

# Lancer QEMU avec sortie série directe dans le terminal
cd build/boot
exec qemu-system-i386 \
    -kernel sextant.elf \
    -nographic \
    -serial mon:stdio \
    -no-reboot

# Le script s'arrête ici car QEMU bloque
# Pour quitter: Ctrl+A puis X (ou Ctrl+C)
