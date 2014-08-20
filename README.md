AsTra
=====

AsTra = Ascent Tracker, an Arduino-based altitude data logger for (indoor)climbing

AsTra is a smallish (12x6x2.5 cm, 200 g) wearable device to record
altitude (based on barometric pressure) four times every second to a
SD card. For convenience also time, name of the climber and grade of
the route are selectable and stored with every ascent. The data are
processed with a Perl program to yield daily ascent curves (with
[dygraphs](http://dygraphs.com/)) for every climber.

The [demo graph for
Tarzan](http://www.helsinki.fi/~syrjanen/AsTra/Tarzan/2014-05-27.html)
displays each ascent as a curve, height versus time. You can select
which ascents are visible, zoom in with mouse etc.

I will publish all software and support files in August, 2014.

*Contact: ascend dot tracker at gmail dot com*

From an idea to a wearable device
---------------------------------

I have had indoor climbing as a hobby for a couple of years. All that
time I have wanted to come up with some method of keeping a record or
a diary of my climbing sessions and track my (moderate) progress. Just
recording route grades and dates on a notepad would have been too
simple for me. :-)

What I wanted was some automatic tracking device to map altitude
versus time. Measuring altitude could be done actively with ultrasound
or optical ranging but modern barometric pressure sensors could do
that passively and without calibration, especially when only relative
altitudes are required.

I was amazed by the
[videos](http://www.varesano.net/blog/fabio/ms5611-01ba-arduino-library-first-developments-results)
by Fabio Varesano about barometric pressure sensor MS5611-01BA01. A
resolution of a 50 cm would have been enough for logging climbing
routes but sensitivity of just 10 centimeters was more than I would
have hoped. Individual altitude readouts from the sensor are noisy,
but averaging and filtering saves the day.

AsTra is also inspired by these projecs and tutorials:

* [FreeIMU](http://www.varesano.net/projects/hardware/FreeIMU) has the current library for MS5611
* [Altduino](http://www.altduino.de/) altitude logger for model rockets
* [openxvario](https://code.google.com/p/openxvario/) for RC planes, especially for **Kalman filtering** code
* [Arduino DIY SD Card Logging Shield](http://www.instructables.com/id/Arduino-DIY-SD-Card-Logging-Shield/) for cheap LC Studio SD card breakout
* [SD Cards in arduino-info](http://arduino-info.wikispaces.com/SD-Cards)
* [Why not to use 9 V batteries](http://cybergibbons.com/uncategorized/arduino-misconceptions-6-a-9v-battery-is-a-good-power-source/)
* [Resistor ladder](http://tronixstuff.com/2011/01/11/tutorial-using-analog-input-for-multiple-buttons/) to read multiple buttons via one analog input
* [Analog debounce](https://github.com/MatCat/AnalogDebounce) library to simpilify reading the buttons
* [Optimizing RAM usage](http://learn.adafruit.com/memories-of-an-arduino/optimizing-sram)

Does it work?
-------------

Well, yes, at least for my purposes. Altitude readings tend to drift
even when stationary and especially longer (in time) routes tend to
end couple of meters higher than started. Possible causes include
temperature at different altitudes, wind and changes in air
conditioning affecting the barometric pressure. No tests have (yet)
been made outside.

Building
--------

I got my MS5611 breakout from
[CSG](http://www.csgshop.com/category.php?id_category=10). It arrived
with no documentation so I just had to guess with help of a similar
product by
[Drotek](http://www.drotek.fr/shop/en/home/44-ms5611-pressure-barometric-board.html)
on how to enable I2C mode and select the device address by soldering
**two** jumper pads.

SD card module from LC Studio was too big for my enclosure so I had to
use a MicroSD version from Sparkfun. Unfortunately that might mean
that I'm stuck with SfFat library because scarse avalability of small
MicroSD cards that can be used with TinyFAT etc. (Note: partitioning
might help with this)

[Fritzing](http://fritzing.org/home/) breadboard view was a nice way
to plan and document(!) wirings on a proto board. Export to a PNG and
flip it over to give view from the solder side. Schematic view was
used in documenting the actual circuit design.

I used [Inkscape](http://www.inkscape.org/en/) to draw "parts" for the
connectors and to rescale some (inch-measured?) Sparkfun
[parts](https://github.com/adafruit/Fritzing-Library/blob/master/parts/2.2%20TFT%20with%20MicroSD%20Breakout.fzpz).

I happened to select a box that was just big enough for all stuff
needed but not much. My hardest build this far, using only hand tools.

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

About software
--------------

Arduino memory size limits are always a problem.

MS5611 library returning NULL for a float on invalid data got me in a big way. Note that [MS561101BA_altitude.pde](https://github.com/PaulStoffregen/FreeIMU/blob/master/libraries/MS561101BA/examples/MS561101BA_altitude/MS561101BA_altitude.pde) does NOT check for NULL return values as [MS561101BA_simple.pde](https://github.com/PaulStoffregen/FreeIMU/blob/master/libraries/MS561101BA/examples/MS561101BA_simple/MS561101BA_simple.pde) does. Those NULL values messed up the Kalman filtering code and shortcircuited simple float calculation (foo - NULL = 0.0).

TODO & enhancements
-------------------

* check TinyFAT
* change LCD for a BlueTooth connection to a smartphone app (don't have one at the moment)
* LiPo for power source
* PCB

**See more notes on building in image captions**


