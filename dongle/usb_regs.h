#ifndef USB_REGS_H
#define USB_REGS_H

#include <compiler_mcs51.h>

USB_REGISTER(uint8_t out5buf[USB_EP5_SIZE], 0xC440);
USB_REGISTER(uint8_t in5buf[USB_EP5_SIZE], 0xC480);
USB_REGISTER(uint8_t out4buf[USB_EP4_SIZE], 0xC4C0);
USB_REGISTER(uint8_t in4buf[USB_EP4_SIZE], 0xC500);
USB_REGISTER(uint8_t out3buf[USB_EP3_SIZE], 0xC540);
USB_REGISTER(uint8_t in3buf[USB_EP3_SIZE], 0xC580);
USB_REGISTER(uint8_t out2buf[USB_EP2_SIZE], 0xC5C0);
USB_REGISTER(uint8_t in2buf[USB_EP2_SIZE], 0xC600);
USB_REGISTER(uint8_t out1buf[USB_EP1_SIZE], 0xC640);
USB_REGISTER(uint8_t in1buf[USB_EP1_SIZE], 0xC680);
USB_REGISTER(uint8_t out0buf[USB_EP0_SIZE], 0xC6C0);
USB_REGISTER(uint8_t in0buf[USB_EP0_SIZE], 0xC700);
USB_REGISTER(uint8_t out8data, 0xC760);
USB_REGISTER(uint8_t in8data, 0xC768);
USB_REGISTER(uint8_t out8bch, 0xC770);
USB_REGISTER(uint8_t out8bcl, 0xC771);
USB_REGISTER(uint8_t bout1addr, 0xC781);
USB_REGISTER(uint8_t bout2addr, 0xC782);
USB_REGISTER(uint8_t bout3addr, 0xC783);
USB_REGISTER(uint8_t bout4addr, 0xC784);
USB_REGISTER(uint8_t bout5addr, 0xC785);
USB_REGISTER(uint8_t binstaddr, 0xC788);
USB_REGISTER(uint8_t bin1addr, 0xC789);
USB_REGISTER(uint8_t bin2addr, 0xC78A);
USB_REGISTER(uint8_t bin3addr, 0xC78B);
USB_REGISTER(uint8_t bin4addr, 0xC78C);
USB_REGISTER(uint8_t bin5addr, 0xC78D);
USB_REGISTER(uint8_t isoerr, 0xC7A0);
USB_REGISTER(uint8_t zbcout, 0xC7A2);
USB_REGISTER(uint8_t ivec, 0xC7A8);
USB_REGISTER(uint8_t in_irq, 0xC7A9);
USB_REGISTER(uint8_t out_irq, 0xC7AA);
USB_REGISTER(uint8_t usbirq, 0xC7AB);
USB_REGISTER(uint8_t in_ien, 0xC7AC);
USB_REGISTER(uint8_t out_ien, 0xC7AD);
USB_REGISTER(uint8_t usbien, 0xC7AE);
USB_REGISTER(uint8_t usbbav, 0xC7AF);
USB_REGISTER(uint8_t ep0cs, 0xC7B4);
USB_REGISTER(uint8_t in0bc, 0xC7B5);
USB_REGISTER(uint8_t in1cs, 0xC7B6);
USB_REGISTER(uint8_t in1bc, 0xC7B7);
USB_REGISTER(uint8_t in2cs, 0xC7B8);
USB_REGISTER(uint8_t in2bc, 0xC7B9);
USB_REGISTER(uint8_t in3cs, 0xC7BA);
USB_REGISTER(uint8_t in3bc, 0xC7BB);
USB_REGISTER(uint8_t in4cs, 0xC7BC);
USB_REGISTER(uint8_t in4bc, 0xC7BD);
USB_REGISTER(uint8_t in5cs, 0xC7BE);
USB_REGISTER(uint8_t in5bc, 0xC7BF);
USB_REGISTER(uint8_t out0bc, 0xC7C5);
USB_REGISTER(uint8_t out1cs, 0xC7C6);
USB_REGISTER(uint8_t out1bc, 0xC7C7);
USB_REGISTER(uint8_t out2cs, 0xC7C8);
USB_REGISTER(uint8_t out2bc, 0xC7C9);
USB_REGISTER(uint8_t out3cs, 0xC7CA);
USB_REGISTER(uint8_t out3bc, 0xC7CB);
USB_REGISTER(uint8_t out4cs, 0xC7CC);
USB_REGISTER(uint8_t out4bc, 0xC7CD);
USB_REGISTER(uint8_t out5cs, 0xC7CE);
USB_REGISTER(uint8_t out5bc, 0xC7CF);
USB_REGISTER(uint8_t usbcs, 0xC7D6);
USB_REGISTER(uint8_t togctl, 0xC7D7);
USB_REGISTER(uint8_t usbframel, 0xC7D8);
USB_REGISTER(uint8_t usbframeh, 0xC7D9);
USB_REGISTER(uint8_t fnaddr, 0xC7DB);
USB_REGISTER(uint8_t usbpair, 0xC7DD);
USB_REGISTER(uint8_t inbulkval, 0xC7DE);
USB_REGISTER(uint8_t outbulkval, 0xC7DF);
USB_REGISTER(uint8_t inisoval, 0xC7E0);
USB_REGISTER(uint8_t outisoval, 0xC7E1);
USB_REGISTER(uint8_t isostaddr, 0xC7E2);
USB_REGISTER(uint8_t isosize, 0xC7E3);

// we're overlapping the request array with specific request structures
// this makes it a little easier to extract the correct data
USB_REGISTER(uint8_t setupbuf[8], 0xC7E8);
USB_REGISTER(usb_request_value_t	usbRequest, 0xC7E8);
USB_REGISTER(usb_req_std_get_desc_t	usbReqGetDesc, 0xC7E8);
USB_REGISTER(usb_req_hid_get_desc_t	usbReqHidGetDesc, 0xC7E8);
USB_REGISTER(usb_req_hid_get_set_report_t usbReqHidGetSetReport, 0xC7E8);
                        
USB_REGISTER(uint8_t out8addr, 0xC7F0);
USB_REGISTER(uint8_t in8addr, 0xC7F8);

#endif	// USB_REGS_H