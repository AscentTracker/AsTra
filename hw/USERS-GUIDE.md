AsTra Users Guide
=================

Basics
------

- Turn on with power switch, wait 10 seconds for startup/calibration
- Silent mode (no beeps): press any button at poweron 
 or set "b 0" in settings.txt on SD card
- Select grade with Up/Down buttons (?,4,4+,5a,5a+,...9c+)
- Select climber with Set-button (on left edge next to power switch)
- Start climbing with Go-button: clock starts ticking, three beeps
played and GO! displayed
- End climbing with Go-button (finished) or Set-button (failed to
 finish). Totals (altitude and seconds) are displayed until Go is pushed.
- Auto poweroff after 10 minutes of idle (set "i 0" in settings.txt to disable)
- If battery voltage drops below 2.0 V a warning and voltage reading are
  displayded until power is recycled. Exact safe lowest voltage level
  is unknown.

Display
-------

The two row LCD displays the following information.

At Power on:
 AsTra version information and battery voltage.

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
 Max altitude | | Time to max alt

Data
----

Each ascent is recorded to a file on SD card (poweroff before
removing card). Filenames are in format of  YYMMDDAA.CSV
where YY is year (with two digits, sorry Y2K), MM is month, DD is day
and AA goes from AA to ZZ (there is room for 625 ascents for each day).

First line has metadata in JSON-format, rest of the lines have
altitude etc data for the ascent in CSV format.
Line "#FAILED" indicates an ascent that has been marked as failed by the Set-button.

AsTra does not record location information. Location is added to
JSON metadata with _setloc_ utility. It can be specified on command line
also to _adastra_.

CSV fields are as follows (mostly used for debugging):

- milliseconds from start
- raw altitude from the sensor (based on starting temperature)
- moving average of altitude
- kalman filtered altitude: only this is used by adastra
- power supply voltage
- altitude based on current temperature
- current temperature

AsTra V 1.* stores four (4) records every second, _adastra_ displays two of
them.
