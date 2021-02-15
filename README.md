# reno-bt
Arduino ESP bluetooth to serial converter for Renogy/srne solar controllers with RS232 ports.
Tested on Renogy Wanderer 30 only with solarAPP and Renogy BT on android.

Needs an rs232 to 3.3v TTL converter on serial2, and a 3.3v linear regulator for power. 
Alternatively, you could probably wire it up directly inside the unit to the 3.3v side of the on board rs232 transceiver.

RS232 pinout on controller is TX RX GND GND 12v 12v.
