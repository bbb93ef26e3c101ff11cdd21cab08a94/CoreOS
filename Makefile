GCCPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = obj/loader.o \
          obj/core/gdt.o \
          obj/drivers/driver.o \
          obj/hardwarecommunication/port.o \
          obj/hardwarecommunication/interruptstubs.o \
          obj/hardwarecommunication/interrupts.o \
		  obj/hardwarecommunication/pci.o \
          obj/drivers/keyboard.o \
          obj/drivers/mouse.o \
		  obj/drivers/video/vga.o \
		  obj/common/memoryoperations.o \
          obj/kernel.o

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	gcc $(GCCPARAMS) -c -o $@ $<

obj/%.o: src/%.s	
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<

kernel.krnl: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

CoreOS.iso: kernel.krnl
	mkdir -p iso/boot/grub
	cp kernel.krnl iso/boot/kernel.krnl
	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	echo 'menuentry "CoreOS" {' >> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/kernel.krnl' >> iso/boot/grub/grub.cfg
	echo '  boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=CoreOS.iso iso
	rm -rf iso

install_tools:
	sudo apt-get update -y
	sudo apt-get install g++ binutils libc6-dev-i386 grub-legacy xorriso 

run: CoreOS.iso
	(killall VirtualBox && sleep 1) || true
	VirtualBox --startvm 'CoreOS' &

install: kernel.krnl
	sudo cp $< /boot/kernel.krnl

.PHONY: clean
clean:
	rm -rf obj kernel.krnl CoreOS.iso