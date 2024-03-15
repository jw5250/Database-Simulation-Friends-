#
# Define any additional compilation flags you want to use while compiling
# and testing your code.  For instance, you could use "OPTS=-DDEBUG" as a
# command-line option to 'make' to predefine the CPP macro symbol 'DEBUG',
# which could be used to conditionally compile in some debugging code.
# Another use might be "OPTS=-Werror", which would pass the -Werror option
# to GCC, telling it to treat all warning messages as if they were fatal
# coompilation errors.
#
OPTS =

#
# Compilation options
#
CFLAGS = -ggdb -std=c99 -Wall -Wextra -pedantic $(OPTS)

#
# Library options
#
# The first version of CLIBFLAGS tells the linker to look in the
# course account for a library file named "libhash.a" and link in 
# precompiled code from that library.  The library contains a working
# implementation of the HashADT module which you can use during
# development of your solution.
#
# To use the supplied version, DO NOT copy your HashADT.c file into the
# project source directory; create your Makefile with gmakemake, and
# compile and link the program.
#
# Once you are ready to test your program with your own HashADT
# implementation, do the following:
#
#   1. Copy your HashADT.c file into the project directory
#   2. Edit header.mak; comment out the first CLIBFLAGS statement
#      and uncomment the second one
#   3. Re-run gmakemake to create a new Makefile
#   4  Run 'make realclean' and then 'make'.
#

#
# This version links your code against the precompiled HashADT library
#
#CLIBFLAGS = -L/home/course/csci243/pub/projects/02 -lhash -lm 

#
# This version doesn't use the precompiled HashADT library; instead,
# your implementation will be used.
#
 CLIBFLAGS = -lm
