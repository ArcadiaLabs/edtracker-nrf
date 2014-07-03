TARGET   = edtracker
CFLAGS   = --model-large -Imcu-lib -DNRF24LE1 -DDBG_MODE=0
LFLAGS   = --code-loc 0x0000 --code-size 0x4000 --xram-loc 0x0000 --xram-size 0x400
ASFLAGS  = -plosgff
RELFILES = edtracker.rel i2c.rel mpu_simple.rel sleeping.rel rf_addr.rel rf_head.rel nRF24L.rel nrfdbg.rel nrfutils.rel crtxinit.rel

VPATH    = mcu-lib

hex: $(TARGET).hex

$(TARGET).hex: $(RELFILES)
	sdcc $(CFLAGS) $(LFLAGS) $(RELFILES) -o $(TARGET).hex
	grep "ROM/EPROM" $(TARGET).mem

%.rel: %.c
	sdcc $(CFLAGS) -c $<

%.rel: %.asm
	sdas8051 $(ASFLAGS) $@ $< 

clean:
	rm --force *.hex *.lnk *.lst *.map *.rel *.rst *.sym *.mem *.lk *.asm *.lk *.cdb *.omf

all: clean hex

flash: clean hex
	nrfburn -f 16 -w $(TARGET).hex
