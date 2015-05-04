FastQSP (jack edition)
======================

This is as modified version of FastQSP, with some tight-coupling to the game Jack-o-nine-tales.

So far it adds the following improvements (or hacks if you will :-)) to the game:
- Audio system
- JSON
- Video playback
- Ignore CRC check
- File existence
---
- Discussion
- Download / build / run

Audio system
============
- Ability to play multiple audio streams
- Audio streams can be looped and told to exclusively play a single file
- Volume, and loop + single stream flags, can be changed during play.

JSON
====
- Override variables from external JSON files, residing in the subdirectory "json" in the game directory.
The json file, must have the extension json.
The json directory is recursively scanned - you can create any directory structure you like.
To specify an array indice, name the file as a number, or prepend the filename with a number followed by a underscore.
Like this:
5.json
5_this_goes_to_indice_5.json

Video playback
==============
- Show a video instead of an image (tight-coupled to jack-o-nine-tales).
See hongfire thread, page 72, for instructions.

Ignore CRC check
================
- GUI option to ignore the build-in CRC check. To allow load of save games, from a different game version.
Use this feature at your own risk - lot of things probably will work, but some things might not.
Take a backup of your save game and the game version you were using, before upgrading the game.
You find it under the game menu, after the game has loaded.

File existence
==============
From QSP code, you can check if a file exists (in the gamedirectory) using the % operator:
file_exists = %relative/path/to/file.ext
file_exists is now a boolean/int with the value 0 or 1 depending wether the file exist.

Discussion
==========
The game and a bit about this engine, are discussed in the following two threads:
- http://www.hongfire.com/forum/showthread.php/434987
- https://8ch.net/slavmaker/res/1.html

I go by the handle beaver14 - feel free to contact me via hongfire.
I may not notice right away, but eventually I will.

My plan is to take a break from developing on this, but if you find a bug, then submit it - I will notice, and try to fix it, when time allows.

Download
========
Here is a temporary link to a Windows build (2015-05-04): https://mega.co.nz/#!cNxylCJB!0piV72u-7vEWmZKmWjPI4aPA6WhR37fO_mNTepBlht0 

If link is expired, and you can not build it yourself, then create an issue. It's okay.

Basically to build it. Install Qt 5.x, clone project (there are no releases), open in Qt creator, and click build and run.

All the best,

Have fun! :-)
