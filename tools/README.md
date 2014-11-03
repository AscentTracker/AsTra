AsTra Data Reporting
=================

You can plot AsTra CSV files in any way you want.

These Linux flavored tools produce HTML+CSV files for dygraphs.js
visualization library and Apache flavored www server.

Software needed:

- rsync to smartly move files, use cp if not avaible
- Perl

*Tools provided*:

- _getnew_: shell program to fetch new files from SD card, runs _adastra_
- _adastra_: Perl program to process CSV files to WWW reports
- _publish_: template shell program for publishing the results
- _setloc_: Perl program to set JSON location field in CSV files

Install
-------

* edit mount point to _getnew_
* edit WWW charset to _adastra_
* edit publish location to _publish_
* chmod a+x getnew adastra publish setloc

Quick usage
-----------

* insert SD card
* ./getnew Location
* ./publish


You can view the reports with a modern browser in directory www.

You can set location info to CSV files like this

 ./setloc -loc Location data/140606*.CSV

and make reports from those files

 ./adastra -v data/140606*.CSV

You can override location with adastra option -loc

