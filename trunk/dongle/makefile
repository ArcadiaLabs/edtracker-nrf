TARGET   = eddngl.hex
CFLAGS   = --model-small -I.. -I../mcu-lib -DNRF24LU1 -DDBG_MODE=1
LFLAGS   = --code-loc 0x0000 --code-size 0x4000 --xram-loc 0x8000 --xram-size 0x800
ASFLAGS  = -plosgff
RELFILES = main.rel usb_desc.rel nrfutils.rel rf_dngl.rel usb.rel reports.rel rf_addr.rel nrfdbg.rel nRF24L.rel crtxinit.rel

VPATH    = ../mcu-lib:../

$(TARGET): $(RELFILES)
	sdcc $(CFLAGS) $(LFLAGS) $(RELFILES) -o $(TARGET)

%.rel: %.c
	sdcc $(CFLAGS) -c $<

%.rel: %.asm
	sdas8051 $(ASFLAGS) $@ $< 

clean:
	rm -f *.hex *.lnk *.lst *.map *.rel *.rst *.sym *.mem *.lk *.asm *.lk *.cdb *.omf

all: clean $(TARGET)

flash: clean $(TARGET)
	nrfburn -f 16 -w $(TARGET)
