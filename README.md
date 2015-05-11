FastQSP (jack edition)
======================

This is as modified version of FastQSP, with some strong-coupling to the game Jack-o-nine-tales.
This is not meant to be a general QSP player, though it may work with other games.

So far it adds improvements (or hacks if you will :-)) to the following areas of game:
- Audio system
- JSON
- Video playback
- Ignore CRC check
- File existence

Download
========
Here is a temporary link to a Windows build (2015-05-11): https://mega.co.nz/#!FAYCBKzD!f1Opq25fEIEzkwsNUbRzisB3d6IwDYg9uuOngzf5u_I

If link is expired, and you can not build it yourself, then create an issue. It's okay.

Basically to build it. Install Qt 5.x, clone project (there are no releases), open in Qt creator, and click build and run.
Be sure to compile with a compiler that supports C++11.

Discussion
==========
The game and a bit about this engine, are discussed in the following two threads:
- http://www.hongfire.com/forum/showthread.php/434987
- https://8ch.net/slavmaker/res/1.html

I go by the handle beaver14 - feel free to contact me via hongfire.

Audio system
============
- Ability to play multiple audio streams
- Audio streams can be looped and told to exclusively play a single file
'''
Syntax is play 'path/to/file', int_volume, '!@'
! means stop other sounds besides this one, and @ means loop sound.
'''
the word "only", can be used insted of !. The word "loop" can be used insted of @.
- Volume, and loop + single stream flags, can be changed during play.
- Volume can be muted from the GUI. Mute audio is found under the "Other" menu.

JSON
====
- Override variables from external JSON files, residing in the subdirectory "json" in the game directory.
The json file, must have the extension json.
The json directory is recursively scanned - you can create any directory structure you like.
To specify an array indice, name the file as a number, or prepend the filename with a number followed by a underscore.
Like this:
5.json or 5_this_goes_to_indice_5.json
```json
The format of the file, should be:
{
  "string_variable_name": "string_value",
  "int_variable_name": 1,
  "img_array_name": ["path/to/img1", "path/to/img1", "path/to/img1"] // This only works for "interaction" images.
}
```

Video playback
==============
- Show a video instead of an image.
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


All the best,

Have fun! :-)
