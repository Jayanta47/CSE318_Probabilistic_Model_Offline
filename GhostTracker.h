#ifndef GHOST_TRACKER_H
#define GHOST_TRACKER_H

#include<iostream>
#include<vector>
#include<sstream>
#include<string>
#include<fstream>
#include<iomanip>
#include<cstdlib>

#define true_positive 0.85

using namespace std;

struct blockInfo {
    int edge_adj_blocks;
    int non_edge_blocks;
};

class GhostTracker
{
private:
    int num_obs, n_row, n_column;
    bool isBstate_tCalc;
    vector<vector<double>> *br_grid;
    
    double **trans_dist_matrix;

    double **next_belief_matrix;

    void calculateTransition();
public:
    GhostTracker(vector<vector<double>> *br_grid, int num_obs);
    void calculate_bstate_p();
    bool update_curr_bstate(int row, int col, bool isCasperPresent);
    void printBstate_t ();
    ~GhostTracker();
};






#endif // GHOST_TRACKER_H