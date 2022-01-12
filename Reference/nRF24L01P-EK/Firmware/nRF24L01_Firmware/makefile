OBJS=Protocol_API.obj isr.obj adc.obj nrf_api.obj f320_flashprimitives.obj fwupgrade.obj ll_api.obj main.obj testapp.obj usb.obj
#OBJV=version.obj
all: nrf24l01fw-l.hex nrf24l01fw-h.hex

nrf24l01fw-l.m51: $(OBJS)
	bl51 fwupgrade.obj, protocol_API.obj, ll_api.obj, f320_flashprimitives.obj, isr.obj, adc.obj, nrf_api.obj,main.obj, testapp.obj, usb.obj, include\USBX_F320_1.LIB TO nrf24l01fw-l RS(256) PL(68) PW(78) XDATA(?XD?_USB_INIT?USB_API(0x07AF)) CO(200H)

nrf24l01fw-h.m51: $(OBJS)
	bl51 fwupgrade.obj, protocol_API.obj, ll_api.obj, f320_flashprimitives.obj, isr.obj, adc.obj, nrf_api.obj,main.obj, testapp.obj, usb.obj, include\USBX_F320_1.LIB TO nrf24l01fw-h RS(256) PL(68) PW(78) XDATA(?XD?_USB_INIT?USB_API(0x07AF)) CO(2000H)

nrf24l01fw-l.hex: nrf24l01fw-l.m51
	oh51 nrf24l01fw-l HEXFILE(nrf24l01fw-l.hex)

nrf24l01fw-h.hex: nrf24l01fw-h.m51
	oh51 nrf24l01fw-h HEXFILE(nrf24l01fw-h.hex)
	
#version.c: $(OBJS)
#	perl mkversion.pl version.c

.c.obj:
	C51 $< DB OE CD

clean:
	del *.obj
	del *.hex
	del *.lst
  del *.bak
  del *.err
  del *.m51
  del NRF24L01FW-L
  del NRF24L01FW-H

Cleanup:
  cleanup.bat