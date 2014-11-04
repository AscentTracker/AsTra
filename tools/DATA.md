AsTra Data
----------

Each ascent is recorded to a file on SD card (poweroff before
removing card). Filenames are in format of YYMMDDAA.CSV
where YY is year (with two digits, sorry Y2K), MM is month, DD is day
and AA goes from AA to ZZ (so there is room for 625 ascents for each day).

First line has metadata in JSON-format, rest of the lines have
altitude etc data for the ascent in CSV format.
Line "#FAILED" indicates an ascent that has been marked as failed by the Set-button.

AsTra does not record location information. Location is added to
JSON metadata with _setloc_ utility. It can also be specified on command line
to _adastra_.

CSV fields are as follows (mostly used for debugging):

- milliseconds from start
- raw altitude from the sensor (based on starting temperature), in meters
- moving average of altitude
- kalman filtered altitude: only this is used by adastra
- power supply voltage
- altitude based on current temperature
- current temperature, degrees of C

AsTra V 1.* hardware stores four (4) records every second, _adastra_
uses every other of them.
