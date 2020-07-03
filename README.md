Cute Gomoku
===========

Cute Gomoku is a graphical user interface, command-line interface and a library
for playing gomoku. It is a derived from project [Cute Chess](https://cutechess.com/), which is 
originally designed for playing chess. Both Cute Gomoku and Cute Chess are
written in C++ using the [Qt framework](https://www.qt.io/).

Compiling
---------

Cute Gomoku requires Qt 5.7 or greater, a compiler with C++11 support and `qmake`.
In the simplest case you only have to issue:

    $ qmake
    $ make

For detailed build instruction on various operating systems please visit:
* [Making a release](https://github.com/cutechess/cutechess/wiki/Making-a-release)

Running
-------

The `cutegomoku-cli` program is run from the command line to play games between
gomoku engines. A gomoku engine is a computer program that can play gomoku by interact with
an interface/manager program following a text protocol. So far for gomoku, 
only Gomocup protocol is supported. See more details about Gomocup engines and 
protocols in the [GomoCup offical site](https://gomocup.org/).

Assuming that you already have an engine Pela, and to play ten games between two 
Pela engines with a time control of 40 moves in 60 seconds:

    $ cutegomoku-cli -variant gomoku -boardsize 15 -engine cmd=./<path-to-engine>/pbrain-pela -engine cmd=./<path-to-engine>/pbrain-pela -each proto=gomocup tc=40/60 -rounds 10

Note that there are three key differences between  `cutegomoku-cli` and `cutechess-cli`.
First, the `-variant gomoku` is required, otherwise cutegomoku will assume that you are playing chess.
Second, the protocol of engines should be `gomocup`.
Third, you can specify the board size by `-boardsize <N>`, which leads to a NxN board. 
The default gomoku board size is 15x15.

For other original options from Cute Chess, please see `cutegomoku-cli -help` for 
descriptions of the supported options or manuals for full documentation.

Acknowledgements
----------------

Thanks for the all developers of Cute Chess for providing an exellent tool for 
the computer chess community. This project intends to expand this tool to 
computer gomoku community so that it could benefit more computer game programmers.


License
-------

Following Cute Chess, Cute Gomoku is released under the GPLv3+ license except 
for the components in the `projects/lib/components` and `projects/gui/components` 
directories which are released under the MIT License.

Credits
-------

- Cute Gomoku: Chao Ma and [contributors](https://github.com/cutechess/cutechess/graphs/contributors)
- Cute Chess: Ilari Pihlajisto, Arto Jonsson and [contributors](https://github.com/cutechess/cutechess/graphs/contributors)
