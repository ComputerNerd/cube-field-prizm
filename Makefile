CC=sh3eb-elf-gcc
CPP=sh3eb-elf-g++
OBJCOPY=sh3eb-elf-objcopy
MKG3A=mkg3a
RM=rm
#CFLAGS=-m4a-nofpu -s -mb -Wl,--relax -Os -ffunction-sections -fdata-sections -fmerge-all-constants -mhitachi -Wall -Wextra -I../../include  -L../../lib
CFLAGS=-m4a-nofpu -mb -fgcse-sm -fgcse-las -fgcse-after-reload -O3 -g -fmerge-all-constants -flto -mhitachi -Wall -Wextra -I../../include  -L../../lib
LDFLAGS=$(CFLAGS) -s -nostartfiles -T../../toolchain/prizm.x -Wl,-static -Wl,--gc-sections 
OBJECTS=src/cube.o src/key.o src/graphic_functions.o src/fixed.o src/main.o src/text.o
PROJ_NAME=CubeField
BIN=$(PROJ_NAME).bin
ELF=$(PROJ_NAME).elf
ADDIN=$(PROJ_NAME).g3a
 
all: $(ADDIN)

$(ADDIN): $(BIN)
	$(MKG3A) -n :"Cube Field" -i uns:unselected.png -i sel:selected.png $< $@

.s.o:
	$(CC) -c $(CFLAGS) $< -o $@
 
.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
	
.cpp.o:
	$(CC) -c $(CFLAGS) $< -o $@
	
.cc.o:
	$(CC) -c $(CFLAGS) $< -o $@

$(ELF): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
$(BIN): $(ELF)
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(BIN)

clean:
	rm -f $(OBJECTS) $(PROJ_NAME).bin $(PROJ_NAME).elf $(ADDIN)
