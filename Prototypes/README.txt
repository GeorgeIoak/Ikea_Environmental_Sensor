* I designed in a 40MHz Oscillator in a 2.0 x 1.6 package but selected the wrong JLCPCB part.

  -- JLCPCB Part C213412, KDS Daishinku 1ZNA16000AB0P in a SMD2520 package
  -- Correct Part is a ECS ECS-TXO-2016-33-400-TR
  -- I bought 20pcs of Abracon EBRA53L2H-40.000M on 11/26/21 from Arrow

* The pi filter for the antenna works better with no caps mounted.

* The series resistor for the light sensor is too high
  -- R30 is 10kΩ but with 5.0V on the source the Vout is ~4.8V in bright light
  -- Changing to 1.0kΩ keeps Vout less than 3.0V
  -- Should monitor voltage once the board is in the enclosure and adjust R30

2023/06/07

* The connectors for the PM2.5 sensor were checked for fit
  -- The CAX 1.25-2A works well for the 2-pin power connector
  -- The CJT A1501WV-4P works for the 4-pin data cable
  -- There needs to be more space between these 2 connectors since the 
     cable housings are wider than the header footprint

* The USB C Connector doesn't line up with the hole in the enclosure
  -- The USB C connector needs to be closer to the board edge
  -- It's currently ~1.7mm away from the board edge and needs to be < 1mm
  -- GCT USB4160-03-0070-C looks to be slim enough, https://gct.co/connector/usb4160
