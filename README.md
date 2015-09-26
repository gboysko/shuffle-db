# iPod shuffle Database Builder

This release contains multiple versions of the program:
* Python (RECOMMENDED)
* Win32/C (DEPRECATED)

Please use the Python version, because it is the only one that is actively developed, and it has a lot more features compared to the Win32 version.

To compile the Win32 version, use lcc-win32.

## Recent Changes

September 26, 2015 - Added support to use disk and track numbers to order songs in an album. Inspection of MP3 tag data using eyeD3:

http://eyed3.nicfit.net/

Note: Occasionally, you may see this warning during execution:

```
eyed3.id3.frames:WARNING: Unknown ID3 frame ID: RGAD
```

This is due to an unexpected MP3 Tag ("RGAD" - Replay Gain ADjustment). This is not an error--the file is still copied.

## Usage

To use the Database Builder, copy rebuild_db.py to your iPod's root directory and start it, e.g. with a double click in Explorer or Finder.

For any other details regarding the software, please visit the webpage at http://shuffle-db.sourceforge.net

Have fun with the program (and with your iPod as well),
Martin J. Fiedler <martin.fiedler@gmx.net>
