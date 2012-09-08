PorkScript
==========

> "An extensible game definition language and engine written in C++ and powered by Python."

This project is currently unfinished. Contributors are welcome, and much help is currently needed in the PorkParser area (my parser is awful, and it needs to be redone -- check PorkParser/PorkParser.cpp if you'd like to see the damage :P ).


Theory
------

PorkScript is a high-level definition language. Its purpose: to act as an easy-to-use language to define parts of a game. Currently, PorkScript 1.0 is focused on text-based adventure games (which may be referred to as TBAG). PorkScript definition scripts work with compiled Python modules to get input from a game and then, based on this input, manipulate the game itself.


What Everything Is
------------------

###GameContainer
This piece compiles as a statically linked library. The library contains PGameContainer, and all its parts. This class represents one game, and all the other classes used inside of it represent parts of the game (maps, items, properties). PorkScript instantiates this class in order to fill it with game information, which it then serializes for later un-serialization and usage by the game itself.

###PorkPy
This piece compiles as a statically linked library. It acts as the bridge between its embedded Python, and PorkParser/PorkV2. It also extends its embedded Python by adding new types (map, item) in conjunction with new methods that manipulate the game.

###PorkParser
This piece compiles as a dynamically linked library. It contains the definitions and code for the PorkScript parser. It takes PorkScript .pmf definitions (an example can be found in PorkParserWin/test.pmf) and "compiles" it into a PGameContainer. This is serialized and (in theory) will be added to a 7z/zip/lmza archive along with all compiled Python modules as a .pgf game file to be opened by the game.

###PorkParserWin
This piece is the Windows executable driver for PorkParser. It uses the class definitions in PorkParser to actually parse an input .pmf script. Drivers for other platforms will need to be created.

###PorkV2
This barely touched library shall be a dynamically linked library containing definitions and all inner workings for the Pork text based adventure game. Like PorkParser, it will require a platform driver. Why V2 in the name? It is a total rewrite of a much earlier project which can be found [here](http://porkadventures.wordpress.com/).

###PythonBase
This is not a library, but rather a collection of test modules (murder.py and kill.py) exemplifying how custom commands are created in Python. PythonBase/PorkScript directory contains the Python modules that define custom command classes and also import the PorkPy module.


How It Works
------------

A .pmf script is written of a particular standard (in this case, standard v1.0 -- Text-Based Adventure Games) and is then put into the PorkParser version that compiles that standard.

###Why Standards?
**Since different types of games often have very different elements or required properties (e.g. items in a TBAG won't have a 3D model assigned to them, but a 3D game probably would -- so we would want to create a built-in property for items allowing models, thereby needing a new version of the standard to be defined), new standards (or versions) of PorkScript may need to be defined. If a better solution can be found (perhaps through Python) we could possibly do away with the standards concept.**

Once this .pmf script is compiled into a serialized PGameContainer object, it is stored in a .pgf 7z/zip/lmza archive (which we could possibly encrypt) along with compiled .pyc Python modules, and any other assets if applicable (sounds, models, animations, etc.).

###Why Python? And how?
**Python is a wonderful, extensible language, allowing easy extension by way of C/C++ extension of embedded Python, or extension through imported pure-Python modules. It is fast, and, of course, it can be embedded as an extension language. Python is used to define custom game commands (e.g. eat, kill, go, look, use, etc.). PorkV2 will not have any actual commands defined, instead they are defined in  Python modules.**

**PorkV2 takes user input (a command, with arguments, e.g. eat dog with spoon), and then uses the first word to decide the module and command (one module defines only ONE custom command of the same name) and then passes the other arguments on to the class' instantiation procedure. Here the Python module runs, and then uses PorkPy functions to call the game back, telling it to change itself (e.g. removing an item from the current map, or changing the user's inventory).**

This .pgf archive is then loaded into a game (PorkV2), from which it can then be played and enjoyed!


PorkScript Features
-------------------

PorkScript is built to read like a sentence as one writes in it. It offers many features that make it easy to create one's own adventure game! Remember that example code resides in PorkParserWin/test.pmf.

###Easy Property Setting
With a simple `set x y` format, a map/item property may be set. Properties may include built-in ones such as names, descriptions, itemlists, etc., or custom properties that are defined alongside custom commands in Python.

####Custom Properties?
**Yes, custom properties. These are properties created along with custom commands in Python. These properties have to do specifically with the command they are defined with. (See PythonBase/kill.py for an example.) For example, kill.py contains the custom properties Msg and NewItem. These are both applied to items in the .pmf script to define what item a "killed" item will yield, as well as what message to print to the screen. As seen, these "custom properties" are defined alongside custom commands, and have to do specifically with the command they are defined with.**

###States
Every map can have multiple states, which can be "triggered" by textual events fired by Python. When fired, a "listening" map will change their state to the one with the fired trigger (if defined). States can also be definitive, meaning that once a map has reached that state, it is impossible to exit it. Itemlists in maps also have special properties applicable only in states (i.e. `anItem reliesOn anotherItem from someState as present`). This enables a user to have some items only reveal in a triggered state under particular conditions (such as whether or not another item exists in some other state).

###Ease of Use
Anyone can use PorkScript. Due to its simplicity and reliance on words rather than operators, it enables a person with no knowledge of actual programming languages to create their own adventure game. Granted, it helps to know Python, but is not necessary if the user can attain some basic command scripts (which I plan to make available).


Pork's Name and History
-----------------------

Pork started as a small side project in 2010. The name is derived from "parody of Zork." I developed a small dinky engine with a crappy definition language. A friend of mine used the language to create a whole game, which was then distributed and sold on flash drives for a charity cause.

After that, I saw how much potential a better version of the game could have. So, in the summer of 2011, I started planning the new PorkScript, along with the GameContainer. Finally, I started coding parts of it here and there, but struggled to create a good parser. Now, I realize that not only am I having trouble with that, but the engine itself will not be able to live up to its full potential if only I work on it. My dream for Pork is for it to expand to other places, away from the TBAG world. But for these extensions to come about, this engine had to be released into the open source.

From here, if enough contribute, PorkScript can rise up and expand, and become a wonderful way for people to create their own adventure games (and maybe more).
