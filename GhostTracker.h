#ifndef GHOSTTRACKER_H
#define GHOSTTRACKER_H

#include<iostream>
#include<vector>
#include<sstream>
#include<string>
#include<fstream>
#include<iomanip>
#include<cstdlib>
#include<chrono>

#define true_positive 0.85
#define edge_cell_mov_prob 0.9

using namespace std;

extern bool consoleStream;

extern streambuf *old_stream_buf_cin;
extern streambuf *old_stream_buf_cout;

extern string *inputFileName;
extern string *outputFileName;

extern fstream *inpFile;
extern fstream *outFile;

struct blockInfo {
    int edge_adj_blocks;
    int non_edge_blocks;
};

class GhostTracker
{
private:
    int num_obs, n_row, n_column, t;
    bool isBstate_tCalc;
    vector<vector<double>> *br_grid;
    
    double **trans_dist_matrix;

    double **next_belief_matrix;

    void calculateTransition();
    void printBlockInfo (blockInfo block_info_arr[]);
    void printTransitionProbMatrix();
    void printBoard();
public:
    GhostTracker(vector<vector<double>> *br_grid, int num_obs);
    void calculate_bstate_p();
    bool update_curr_bstate(int row, int col, bool isCasperPresent);
    void printBstate_t ();
    void printState ();
    void printLikelyPos();
    ~GhostTracker();
};

void redirectStream();




#endif // GHOST_TRACKER_H