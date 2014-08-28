AsTra
=====

AsTra = Ascent Tracker is an Arduino-based altitude data logger for
(indoor)climbing.

AsTra is a smallish (12x6x2.5 cm, 200 g) wearable device to record
altitude (based on barometric pressure) four times a second to an
SD card. For convenience also time, name of the climber and grade of
the route are selectable and stored with every ascent. The data are
processed with a Perl program to yield daily ascent curves (with
[dygraphs](http://dygraphs.com/)) for every climber.

The [demo graph for Tarzan](http://www.helsinki.fi/~syrjanen/AsTra/Tarzan/2014-05-27.html)
displays each ascent as a curve, height versus time. You can select
which ascents are visible, zoom in with mouse etc.

AsTra has two components:

- [hw](hw): AsTra hardware (with Arduino software)
- [tools](tools): Software and tools for graphing AsTra data

More documentation on design and building AsTra can be found
in [AsTra image library](http://pars.kuvat.fi/kuvat/AsTra/?pw=AsTra)

*Contact: ascend dot tracker at gmail dot com*
