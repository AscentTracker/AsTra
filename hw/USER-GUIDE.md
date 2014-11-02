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

