AsTra Data Reporting
=================

You can plot AsTra CSV files in any way you want, see [DATA.md](DATA.md) for
field information. These Linux flavored
tools produce HTML+CSV files for [dygraphs](http://dygraphs.com/) visualization library and
Apache flavored www server. Porting to Windows and IOS would not be
too hard.

Software needed:

- rsync to smartly move files, use cp if not available
- Perl

*Tools provided*:

- _getnew_: shell program to fetch new files from SD card, runs _adastra_
- _adastra_: Perl program to process CSV files to WWW reports
- _publish_: shell program for publishing the results
- _setloc_: Perl program to set JSON location field in CSV files

Install
-------

* edit mount point to _getnew_
* edit WWW charset to _adastra_
* edit publish location to _publish_ (optional)
* chmod a+x getnew adastra publish setloc

Quick usage
-----------

* insert SD card
* ./getnew Location (automatically fetches new files and runs adastra with them)
* ./publish (if you want to publish them to a www server)

You can view the reports with a modern browser in directory [www](www).

You can set location info to CSV files like this

 ./setloc -loc Location data/140606*.CSV

and make reports from those files

 ./adastra -v data/140606*.CSV

You can override location with adastra option -loc

&copy; Seppo Syrjänen 2014
