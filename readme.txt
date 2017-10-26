Flood-It

This program aims to generate a solution to a given instance of the game Flood-It.

For the implementation, it was used the C language along with the Graphviz library. Below is a short description of the algorithm:

We know that the Flood-It board has i x j fields, where i is the number of lines and j is the number of columns. Each field in the board can be painted with a color that goes through [1...n] colors. This solver first runs through the board checking "blocks" in the board (neighbor fields of the same color). Each block is then a node in a undirected and unweighted graph G, and, if two blocks are neighbors in the board, an edge links them in G.

After that, a Breadth First Search is used to identify which node has the farthest distance from the node that represents the initial block in the board (the block which has the field [0,0]). So, a color that minimizes this distance is applied on the board, and a new graph is generated. 
This procedure is repeated until the board has only one color.

Operating:

make
./floodit < input.txt > output.txt
input.txt should contain one first line containing the number of lines, columns and colors, and then the board itself. For example:

An instance with 3 lines, 3 columns and 3 colors.
3 3 3
1 2 3
1 3 2
1 3 1

Limitations:

The minimum number of colors is 2.
The maximum number of steps in a solution that this program can generate is 2000. If the input is a board which needs more than 2000 steps to be solved, the program will accuse error.

Dependencies:

It is necessary to have the graphviz package on your system.