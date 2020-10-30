# FM-Index
This project implements the rank and select operations for succint data structures.
Furthermore, an implementation of the FM Index is presented with the ability to query different patterns in the FM-Index.
The following library is used for building the BWT of the input:
https://github.com/kurpicz/saca-bench/tree/master/libdivsufsort

#Instructions to install the project
```
> git clone https://github.com/mohsenzakeri/FM-Index.git
> cd FM-Index
> mkdir build
> cd build
> cmake ../
> make
```

#How to run
To build the FM index on an input text file, you should run:
```
> bwocc build <input string> <output file>
```
To query a set of patterns which are stored in a text file, you should run:
```
> bwocc query <saved fmindex> <query patterns>
```
