

struct bh_node {
double pos_start[3];
double pos_end[3];
int n_bodies;
int initialized;
int has_center_of_mass;
int depth;
int body_number;
int external;
struct bh_node *UNE;
struct bh_node *USE;
struct bh_node *USW;
struct bh_node *UNW;
struct bh_node *LNE;
struct bh_node *LSE;
struct bh_node *LSW;
struct bh_node *LNW;
struct bh_node *UP;
double cms_pos[3];
double cms_mass;
};


//void init_bh_node(bh_node* node);
//void build_bh_tree(bh_node bh_tree[], planet objects[], settings* sim_set);
