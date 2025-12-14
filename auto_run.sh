#!/bin/bash
# Auto-boot script for Breakout game

cd /workspaces/base-projet

# Kill any existing QEMU
pkill -9 qemu-system-i386 2>/dev/null
sleep 1

# Create expect script to automate GRUB boot
cat > /tmp/grub_auto.exp << 'EOF'
#!/usr/bin/expect -f
set timeout 10
spawn qemu-system-i386 -display curses -serial stdio -net nic,model=ne2k_isa -net user,tftp=./build/boot -cdrom ./build/boot/grub.iso
expect "grub>"
sleep 1
send "\r"
interact
EOF

chmod +x /tmp/grub_auto.exp
/tmp/grub_auto.exp
