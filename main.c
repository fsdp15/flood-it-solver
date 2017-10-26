#include <stdio.h>
#include <stdlib.h>
#include <graphviz/cgraph.h>
#include <time.h>
#include <unistd.h>

typedef struct{
	int color;
	int checked;
	int list;
} board_game;

typedef struct {
	int lines, columns, n_colors;
	board_game **board;
} game;

typedef struct {
	int x;
	int y;
} coordinates;

void BFS(Agraph_t *g, int *largest_distance, Agnode_t **node_queue, int *i, int *node_queue_size, char **path) {
	int distance;
	int j = 1;
	Agnode_t *v, *v2;
	Agedge_t *e;
	while (*i < *node_queue_size) {
		v = node_queue[*i];
			for (e = agfstedge(g, v); e; e = agnxtedge(g,e,v)) {
				v2 = e->node;
				if (*(agget(v2, "v")) == '0') {
					node_queue[*node_queue_size] = v2;
					++*node_queue_size;
					distance = *(agget(v, "d")) - 48 + 1;
					if (distance > *largest_distance) {
						path[j] = (agnameof(v2));
						++j;
						*largest_distance = distance;
					}
					char *distance_c = malloc(sizeof(char));
					sprintf(distance_c, "%d", distance);
					agset(v2, "v", "1");
					agset(v2, "d", distance_c);
				}
			}
		++*i;
	}
}

void merge_nodes(Agraph_t *g, Agnode_t *k, Agnode_t *n, Agnode_t *new_node, int *num_nodes) {
	for (Agedge_t *e = agfstedge(g, k); e; e = agnxtedge(g,e,k)) {
		agedge(g, new_node, e->node, "",TRUE);
	}

	for (Agedge_t *e = agfstedge(g, n); e; e = agnxtedge(g,e,n)) {
		agedge(g, new_node, e->node, "",TRUE);
	}	

	Agedge_t *a = agedge(g, new_node, new_node, "",FALSE);
	agdeledge(g, a);
	agdelnode(g, k);
	agdelnode(g, n);
	--*num_nodes;
}
									
void minimize_graph(Agraph_t *g, Agnode_t *k, char **steps, int *num_steps) { //Finds the most distant node from the graph root and minimizes the distance to it using a BFS
																				// Repeat this until there is only 1 node
	Agnode_t **node_queue;
	Agnode_t *n, *new_node;
	int i, largest_distance, node_queue_size;
	int aux = -1;
	int flag = 0;
	int num_nodes = agnnodes(g);
	char *color_aux;
	char **path;

	while (num_nodes > 1) {
		node_queue = malloc(sizeof(Agnode_t*) * num_nodes);
		agset(k, "v", "1");
		node_queue[0] = k;
		largest_distance = 0;
		i = 0;
		node_queue_size = 1;
		path = malloc(sizeof(char*) * num_nodes);
		path[0] = (agnameof(k));
		BFS(g, &largest_distance, node_queue, &i, &node_queue_size, path);

	  	for (Agnode_t *v=agfstnode(g); v; v=agnxtnode(g,v)) {
			agset(v, "v", "0");
			agset(v, "d", "0");
	  	} 

	  	char *aux_c = malloc(sizeof(char));
		sprintf(aux_c, "%d", aux);

		n = agnode(g, path[1], FALSE);
		new_node = agnode(g, aux_c, TRUE);
		aux--;
		color_aux = agget(n, "c");
		agset(new_node, "c", color_aux);
		steps[*num_steps] = color_aux;
		++*num_steps;

		free(aux_c);
		free(node_queue);
		free(path);

		while (42) { // Check if there is linked nodes which are of the same color and "merge" them together
			for (Agedge_t *e = agfstedge(g, k); e; e = agnxtedge(g,e,k)) {
				if (strcmp(color_aux, (agget(e->node, "c"))) == 0) {
					aux_c = malloc(sizeof(char));
					sprintf(aux_c, "%d", aux);
					new_node = agnode(g, aux_c, TRUE);
					aux--;
					agset(new_node, "c", (agget(e->node, "c")));
					merge_nodes(g, k, e->node, new_node, &num_nodes);
					free(aux_c);
					k = new_node;
					flag = 1;
					break;
				}
			}
			if (flag == 0) {
				break;
			}
			flag = 0;
		} 
	}

	n = agfstnode(g);
	if ((strcmp(agget(n, "c"), steps[*num_steps - 1])) != 0) {
		steps[*num_steps] = (agget(n, "c"));
		++*num_steps;
	}
}

void add_edge(Agraph_t *g, int list1, int list2, int color1, int color2) { // If two blocks are neighbors in the board, then an edge links them on the graph													
	Agnode_t *n, *n2;
	char *list1_c = malloc(sizeof(char));
	char *list2_c = malloc(sizeof(char));
	char *color1_c = malloc(sizeof(char) * 2);
	char *color2_c = malloc(sizeof(char) * 2);
	sprintf(list1_c, "%d", list1);
	sprintf(list2_c, "%d", list2);
	sprintf(color1_c, "%d", color1);
	sprintf(color2_c, "%d", color2);
	n = agnode(g, list1_c, TRUE);
	agset(n, "c", color1_c);
	n2 = agnode(g, list2_c, TRUE);
	agset(n2, "c", color2_c);
	agedge(g, n, n2, "",TRUE);
}

void flood_fill(game game_instance, coordinates square, int color, int *i, int list, Agraph_t *g) {
		coordinates aux;
		game_instance.board[square.x][square.y].checked = 1;
		game_instance.board[square.x][square.y].list = list;
		++*i;
		
		if (square.x > 0) {
			if (game_instance.board[square.x - 1][square.y].checked == 0) {
				if (game_instance.board[square.x - 1][square.y].color == color) {
					aux = square;
					aux.x--;
					flood_fill(game_instance, aux, color, i, list, g);
				}
			}

			else {
				if (game_instance.board[square.x - 1][square.y].color != color) {
					aux = square;
					aux.x--;
					add_edge(g, game_instance.board[square.x - 1][square.y].list, list, game_instance.board[square.x - 1][square.y].color, color);
				}	
			}
		}

		if (square.x < (game_instance.lines - 1)) {
			if (game_instance.board[square.x + 1][square.y].checked == 0) {
				if (game_instance.board[square.x + 1][square.y].color == color) {
					aux = square;
					aux.x++;
					flood_fill(game_instance, aux, color, i, list, g);
				}
			}

			else {
				if (game_instance.board[square.x + 1][square.y].color != color) {
					aux = square;
					aux.x++;
					add_edge(g, game_instance.board[square.x + 1][square.y].list, list, game_instance.board[square.x + 1][square.y].color, color);
				}	
			}
		}

		if (square.y > 0) {
			if (game_instance.board[square.x][square.y - 1].checked == 0) {
				if (game_instance.board[square.x][square.y - 1].color == color) {
					aux = square;
					aux.y--;
					flood_fill(game_instance, aux, color, i, list, g);
				}
			}

			else {
				if (game_instance.board[square.x][square.y - 1].color != color) {
					aux = square;
					aux.y--;
					add_edge(g, game_instance.board[square.x][square.y - 1].list, list, game_instance.board[square.x][square.y - 1].color, color);
				}	
			}
		}

		if (square.y < (game_instance.columns - 1)) {
			if (game_instance.board[square.x][square.y + 1].checked == 0) {
				if (game_instance.board[square.x][square.y + 1].color == color) {
					aux = square;
					aux.y++;
					flood_fill(game_instance, aux, color, i, list, g);
				}
			}

			else {
				if (game_instance.board[square.x][square.y + 1].color != color) {
					aux = square;
					aux.y++;
					add_edge(g, game_instance.board[square.x][square.y + 1].list, list, game_instance.board[square.x][square.y + 1].color, color);
				}	
			}
		}

	return;
}

void create_graph(game game_instance, Agraph_t *g) { // Iterate through the board finding blocks of the same color. Each block is a node in the graph
													
	coordinates square;
	int i, j, n;
	int list = 0;

	for (i = 0; i < game_instance.lines; i++) {
		for (j = 0; j < game_instance.columns; j++) {
			if (!game_instance.board[i][j].checked) {
				square.x = i;
				square.y = j;
				n = 0;
				flood_fill(game_instance, square, game_instance.board[i][j].color, &n, list, g);
				++list;
			}
		}
	}
}

game read_g() {
	game game_instance;
	scanf("%d", &(game_instance.lines));
	scanf("%d", &(game_instance.columns));
	scanf("%d", &(game_instance.n_colors));
	game_instance.board = malloc(sizeof(board_game*) * game_instance.lines);
	int i;
	int j;
	for (i = 0; i < game_instance.lines; i++) {
		game_instance.board[i] = malloc(sizeof(board_game) * game_instance.columns);
		for (j = 0; j < game_instance.columns; j++) {
			scanf("%d", &(game_instance.board[i][j].color));
			game_instance.board[i][j].checked = 0;
		}
	}
	return game_instance;
}

/* void print(game game_instance) {
	int i;
	int j;
	for (i = 0; i < game_instance.lines; i++) {
		for (j = 0; j < game_instance.columns; j++) {
			printf("%d ", game_instance.board[i][j].color);
		}
		printf("\n");
	}
} */

int main() {
	char *steps[2000];
	int num_steps = 0;
	game game_instance = read_g();

	Agraph_t *g;
	g = agopen("G", Agundirected, NULL);
	agattr(g, AGNODE, "c", "0");
	agattr(g, AGNODE, "v", "0");
	agattr(g, AGNODE, "d", "0");
	//print(game_instance);
	
	create_graph(game_instance, g);
  	minimize_graph(g, agfstnode(g), steps, &num_steps);
  	agclose(g);

  	printf("%d\n", num_steps);

  	for(int i = 0; i < num_steps; i++) {
		printf("%s ", steps[i]); 
  	}	
  	
  	return 0;
}	