TARGET    = eddngl
CFLAGS    = --model-large -I.. -I../mcu-lib -DNRF24LU1
CFLAGS   += -DDBG_MODE=1
LFLAGS    = --code-loc 0x0000 --code-size 0x8000 --xram-loc 0x8000 --xram-size 0x800
ASFLAGS   = -plosgff
RELFILES  = main.rel proc_packet.rel usb_desc.rel mymath.rel rf_dngl.rel usb.rel reports.rel rf_addr.rel
RELFILES += nrfutils.rel nrfdbg.rel nRF24L.rel crtxinit.rel

VPATH    = ../mcu-lib:../

$(TARGET).hex: $(RELFILES)
	sdcc $(CFLAGS) $(LFLAGS) $(RELFILES) -o $(TARGET).hex
	grep "ROM/EPROM" $(TARGET).mem

%.rel: %.c
	sdcc $(CFLAGS) -c $<

%.rel: %.asm
	sdas8051 $(ASFLAGS) $@ $< 

clean:
	rm -f *.hex *.lnk *.lst *.LST *.map *.rel *.rst *.sym *.mem *.lk *.asm *.lk *.cdb *.omf

all: clean $(TARGET).hex

flash: clean $(TARGET).hex
	nrfburn -f 16 -w $(TARGET).hex
