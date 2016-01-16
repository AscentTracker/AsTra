AsTra Users Guide V1.06
=======================

Basics
------

- Turn on with power switch, wait 10 seconds for startup/calibration
- Silent mode (no beeps): press any button at poweron 
 or set "b 0" in settings.txt on SD card
- Select grade with Up/Down buttons (?,4,4+,5a,5a+,...9c+)
- Select climber with Set-button (on left edge next to power switch)
  Running statistis (since poweron) for each climber are displayed.
  Press Go to select climber.
- Start climbing with Go-button: clock starts ticking, three beeps
  played and GO! displayed
- End climbing with Go-button (finished) or Set-button (failed to
  finish). Totals (altitude and seconds) and END are displayed until Go is pushed.
- Auto poweroff after 10 minutes of idle (set "i 0" in settings.txt to disable)
- If battery voltage drops below 2.0 V a warning and voltage reading are
  displayded until power is recycled. Exact safe lowest voltage level
  is unknown.
- set "d 1" in settings.txt to enable Daylight Savings Time (no need
  to adjust RTC)

Display
-------

The two row LCD displays the following information.

**At Power on:**
 AsTra version information and battery voltage.

**Idle:** 

 Left | Middle | Right
 ---- | -------| -----
 Route grade | Alt zero level (blinking square) | Climber name
 Altitude| | Time
 
**Climbing:**

 Left | Middle | Right
 ---- | -------| -----
 Route grade | GO! | Climber name
 Altitude | |  Elapsed time
 
**Select climber:**

 Left | Middle | Right
 ---- | -------| -----
 Total ascents | | Climber name
 Total meters | | Total time


