Program to compute various clustering coefficients

'mygraph.hpp' wrapper to igraph library, which must be installed on your machine to compile.

'main.cpp' computes a few different clustering coefficients and prints to command line

To compile, use the included Makefile (requires the Igraph C library)

'*.el' sample graphs in edge list format which can be used for testing

Usage: main 'input graph filename' 
       or main 'graph description'

       Graph descriptions:
       ER<n>:<p> -- generates an Erdos-renyi random graph with <n> nodes with edge probability <p>
       WS<n>:<p> -- generates a Watts-Strogatz small-world network with <n> nodes and edge rewiring probability <p>
