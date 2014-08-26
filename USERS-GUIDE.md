AsTra
=====

Basics
------
-turn on with power switch, wait 10 seconds for startup/calibration
-silent mode (no beeps): press any button when powering on 
 or set "b 0" in settings.txt on SD card
-select grade with Up/Down buttons (?,4,4+,5a,5a+,...9c+)
-select climber with Set-button (on left edge next to power switch)
-start climbing with Go-button
-end climbing with Go-button (finished) or Set-button (failed to
 finish). Totals (altitude and seconds) are displayed until Go is pushed.
-auto poweroff after 10 minutes of idle (set "s 0" in settings.txt to disable)

Display
-------

Two row LCD displays the following information.

Poweron: AsTra version information and battery voltage.

Idle: 

 Left | Middle | Right
 ---- | -------| -----
 Route grade | Alt zero level (blinking square) | Climber name
 Altitude| | Time
 
Climbing:

 Left | Middle | Right
 ---- | -------| -----
 Route grade | GO! | Climber name
 Altitude | |  Elapsed time
 
Stop:

 Left | Middle | Right
 ---- | -------| -----
 Route grade | END | Climber name
 Max altitude | Time to max alt

Data
----

Each ascent is recorded to a file on SD card (poweroff before
removing card). Filenames are in format of 

 YYMMDDab.CSV

where YY is year (with two digits, sorry Y2K), MM is month, DD is day
and ab goes from AA to ZZ (there is room for 625 ascents for each day).

First line has metadata in JSON-format, rest of the lines have
altitude etc data for the ascent in CSV format.
Line "#FAILED" indicates an ascent that has been marked as failed by the Set-button.

AsTra does not record any location information. Location is added to
JSON metadata with _setloc_ utility. It can be specified on command line
also to _adastra_.

CSV fields are as follows (most used for debugging):

-milliseconds from start
-raw altitude from the sensor (based on starting temperature)
-moving average of altitude
-kalman filtered altitude: only this is used by adastra
-power supply voltage
-altitude based on current temperature
-current temperature

AsTra stores four (4) records every second, _adastra_ displays two of
them.



