#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <omp.h>
#include "main.h"
#include "bh.h"


void init_bh_node(struct bh_node* node, struct bh_node* parent_node){

node->pos_start[0]=0.;
node->pos_start[1]=0.;
node->pos_start[2]=0.;
node->pos_end[0]=0.;
node->pos_end[1]=0.;
node->pos_end[2]=0.;
node->n_bodies=0;
node->initialized=1;
node->has_center_of_mass=0;
node->external=1;
node->body_number=0;
node->UNE = NULL;
node->USE = NULL;
node->USW = NULL;
node->UNW = NULL;
node->LNE = NULL;
node->LSE = NULL;
node->LSW = NULL;
node->LNW = NULL;
node->UP = parent_node;
node->cms_pos[0]=0.;
node->cms_pos[1]=0.;
node->cms_pos[2]=0.;
node->cms_mass=0.;

}

/*

void build_bh_tree(bh_node bh_tree[], planet objects[], settings *sim_set){

// Allocate space for the root node
bh_tree = (bh_node *)malloc(sizeof(bh_node)*1);
sim_set->bh_treesize++;

// Init root node
void init_node(void);

// Insert bodies
int i;
for (i=0; i<sim_set->n_bodies; i++) {

	

}

free(bh_tree);

}

*/
