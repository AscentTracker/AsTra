AsTra
=====

I wanted log my indoor climbing ascents with some automatic tracking
device to map altitude versus time. Measuring altitude could be done
actively with ultrasound or optical ranging but modern barometric
pressure sensors could do that passively and without calibration,
especially when only relative altitudes are required.

I was amazed by the
[videos](http://www.varesano.net/blog/fabio/ms5611-01ba-arduino-library-first-developments-results)
by Fabio Varesano about barometric pressure sensor MS5611-01BA01. A
resolution of a 50 cm would have been enough for logging climbing
routes but sensitivity of just 10 centimeters was more than I could
have hoped for. Individual altitude readouts from the sensor are noisy,
but averaging and filtering saves the day.

AsTra is also inspired by these projecs and tutorials:

* [FreeIMU](http://www.varesano.net/projects/hardware/FreeIMU) has the current library for MS5611
* [Altduino](http://www.altduino.de/) altitude logger for model rockets
* [openxvario](https://code.google.com/p/openxvario/) for RC planes, especially for **Kalman filtering** code
* [Arduino DIY SD Card Logging Shield](http://www.instructables.com/id/Arduino-DIY-SD-Card-Logging-Shield/) for cheap LC Studio SD card breakout
* [SD Card info in arduino-info](http://arduino-info.wikispaces.com/SD-Cards)
* [Why not to use 9 V batteries](http://cybergibbons.com/uncategorized/arduino-misconceptions-6-a-9v-battery-is-a-good-power-source/)
* [Resistor ladder](http://tronixstuff.com/2011/01/11/tutorial-using-analog-input-for-multiple-buttons/) to read multiple buttons via one analog input
* [Analog debounce](https://github.com/MatCat/AnalogDebounce) library to simpilify reading the buttons
* [Optimizing RAM usage](http://learn.adafruit.com/memories-of-an-arduino/optimizing-sram)

Parts list
----------

- Arduino Mini Pro 5V
- SCG MS5611 breakout
- Sparkfun Logic Level Converter
- Adafruit RTC module
- Sparkfun NCP1402-5V Step-Up Breakout
- 1117T-3.3V regulator + capacitors
- Sparkfun MicroSD breakout
- 2 x 16 LCD display
- buttons, resistors
- project box

Cost ~ 100 €, build time ~ 100 hours (includes lots of hw and sw prototyping & documentation) ;-)

Building the hardware
---------------------

I got my MS5611 breakout from
[CSG](http://www.csgshop.com/category.php?id_category=10). It arrived
with no documentation so I just had to guess with help of a similar
product by
[Drotek](http://www.drotek.fr/shop/en/home/44-ms5611-pressure-barometric-board.html)
on how to enable I2C mode and select the device address by soldering
**two** jumper pads.

Cheap SD card module from LC Studio was too big for my enclosure so I had to
use a MicroSD version from Sparkfun. That had the nice bonus of
eliminating the need for a 4050 level converter.

[Fritzing](http://fritzing.org/home/) [breadboard
view](AsTra-V0.44_bb.png) was a nice way
to plan and document(!) wirings on a proto board. Export to a PNG and
flip it over to give view from the solder side. Fritzing [schematic
view](AsTra-V0.44_schem.png) was
used in documenting the actual circuit design. Download
[AsTra-V0.44.fzz](AsTra-V0.44.fzz) for
complete Fritzing project.

I used [Inkscape](http://www.inkscape.org/en/) to draw "parts" for the
connectors and to rescale some (inch-measured?) Sparkfun
[parts](https://github.com/adafruit/Fritzing-Library/blob/master/parts/2.2%20TFT%20with%20MicroSD%20Breakout.fzpz).

I happened to select a box that was just big enough for all stuff
needed. My hardest build this far, using only hand tools.

Software 
--------

Version: [AsTra_V1.0](AsTra_V1_01.ino)

Program (event) logic is implemented as a Finite State Machine (FSM, see
graph [AsTra-FSM1-1-1.png](AsTra-FSM1-1-1.png)).

Sensor readings are cleaned with Kalman digital filtering. Also a
32-slot moving average is calculated for debugging. See [DATA.md](DATA.md) for
all fields. 

Starting and stopping the ascent tracking automatically (as indicated
in FSM and AUTOSTART macro in code) was more trouble than it was
worth. One button push before and two after ascent is mostly not too
much to remember. 

Arduino memory size limits are always a problem: some recent change in
Arduino IDE (Linux versions) bloated the binary over maximun size so I
had to disable all serial debugging output. Maybe I need a debugging
version with partial functionality?

MS5611 library returning NULL for a float on invalid data got me in a
big way. Note that
[MS561101BA_altitude.pde](https://github.com/PaulStoffregen/FreeIMU/blob/master/libraries/MS561101BA/examples/MS561101BA_altitude/MS561101BA_altitude.pde)
does NOT check for NULL return values as
[MS561101BA_simple.pde](https://github.com/PaulStoffregen/FreeIMU/blob/master/libraries/MS561101BA/examples/MS561101BA_simple/MS561101BA_simple.pde)
does. Those NULL values messed up the Kalman filtering code and
shortcircuited simple float calculation (foo - NULL = 0.0). Note to
self: always check return values and read interface descriptions.

Climbing grades are according to French (indoor) system: 4, 4+, 5a,
5a+, 5b, 5b+, 5c, 5c+, 6a, 6a+,... Modify gradenum2name() to suit your
needs.

Sensor initialization and setup code is inside loop() because
replicating them in setup() would bloat the binary. Sorry for the
inconvenience.

Static variables are used in openLog() to eliminate the need of scanning
the SD card for the next availabe filename while the power is still on.

Device is powered off after 10 minutes on inactivity. This drops the
current consumption from 21 mA to 7 mA so this is mostly an
excercise in power saving techniques.

See [USER-GUIDE.md](USER-GUIDE.md) for how the device is actually used.

Enhancements and alternatives
-----------------------------

Altitude readings tend to drift even when stationary. Longer (in time)
routes tend to end couple of meters higher than started. Possible
causes include temperature at different altitudes, wind and changes in
air conditioning affecting the local barometric pressure. CSV data
have altitude values calculated based on starting and current
temperature (see [DATA.md](DATA.md)) but I have not managed to find any
explanation for the drift.

One might use a second stationary unit for reference like in
differential GPS :-) On the other hand, the device is accurate enough
for my current needs where a resolution of 0.5 meters would suffice.

Alternative sw/hw designs:

* use TinyFAT as SD library (partition card for smaller capacity)
* replace AnalogDebounce with a smaller implementation
* replace rest of printfs with smaller code
* use I2C EEPROM chip for storage, build data download functionality
* add a BlueTooth connection to PC/smartphone app
* use LiPo as power source
* PCB

**See more notes on building in [image captions](pars.kuvat.fi/kuvat/AsTra/)**

&copy; Seppo Syrjänen 2014
