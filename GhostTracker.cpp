#include "GhostTracker.h"

void redirectStream() {
    if (consoleStream) {
        old_stream_buf_cin = cin.rdbuf();   
        old_stream_buf_cout = cout.rdbuf();
        if (outFile)
            cout.rdbuf (outFile->rdbuf ());
        if (inpFile) cin.rdbuf (inpFile->rdbuf ());
    }
    else {
        // redirecting cout and cin to console 
        if (old_stream_buf_cin != nullptr) {
            cin.rdbuf (old_stream_buf_cin);
        }

        if (old_stream_buf_cout != nullptr) {
            cout.rdbuf (old_stream_buf_cout);
        }
    }
    consoleStream = !consoleStream;
}

void redirectOutputStream(bool toOutBuf, fstream *out = nullptr) {
    if (toOutBuf) cout.rdbuf (out->rdbuf());
    else cout.rdbuf(outFile->rdbuf());
}

GhostTracker::GhostTracker(vector<vector<double>> *br_grid, int num_obs)
{
    this->br_grid = br_grid;
    this->num_obs = num_obs;
    this-> n_row = br_grid->size();
    this-> n_column = br_grid->at(0).size();
    this-> t = 0;
    printBoard();
    int transM_row = this->n_row*this->n_column;
    int transM_col = 9;

    this->trans_dist_matrix = new double*[transM_row];

    for (int i=0;i<transM_row;i++) {
        trans_dist_matrix[i] = new double[9];
    }

    calculateTransition();

    this->next_belief_matrix = new double *[n_row];

    for (int i=0;i<n_row;i++) {
        next_belief_matrix[i] = new double[n_column];
    }

    this->isBstate_tCalc = false;

}

void GhostTracker::printBoard() {
    fstream *TPM_file = new fstream();
    TPM_file->open("Grid.txt", ios::out);
    redirectOutputStream(true, TPM_file);
    cout<< "Grid : ("<<n_row<<" * "<<n_column<<")"<<endl<<endl;
    cout<<"   ";
    for (int j=0;j<n_column;j++) {
        cout<<fixed<<"  "<<j<<" ";
    }
    cout<<endl;
    for (int i=0;i<n_row;i++) {
        cout<<i<<": ";
        for (int j=0; j<n_column; j++) {
            cout<<fixed<<"| "<<(br_grid->at(i).at(j)==0.0?"O":" ")<<" ";
        }
        cout<<" |"<<endl;
    }

    redirectOutputStream(false);
    TPM_file->close();
    delete TPM_file;
}

void GhostTracker::calculateTransition() {
    int block_info_size = n_row * n_column;
    blockInfo block_info_arr[block_info_size];

    for (int i=0;i<n_row;i++) {
        for(int j=0;j<n_column;j++) {
            int n_edgeAdj = 0;
            int n_cornerAdj = 0;
            if (br_grid->at(i).at(j) == 0.0) {
                block_info_arr[i*n_column+j].edge_adj_blocks = 0;
                block_info_arr[i*n_column+j].non_edge_blocks = 0;
                for (int t=0;t<9;t++) {
                    trans_dist_matrix[i*n_column+j][t] = 0.0;
                }
                continue;
            }
            for(int k=i-1;k<=i+1;k++) {
                for (int l=j-1;l<=j+1;l++) {
                
                    if (k<0 || k>=n_row) continue;
                    if (l<0 || l>=n_column) continue;
                    if (br_grid->at(k).at(l) == 0) continue;

                    if (k==i && l==j) n_cornerAdj++;
                    else if (k==i) n_edgeAdj++;
                    else if (l==j) n_edgeAdj++;
                    else n_cornerAdj++;

                }
            }
            block_info_arr[i*n_column+j].edge_adj_blocks = n_edgeAdj;
            block_info_arr[i*n_column+j].non_edge_blocks = n_cornerAdj;
            
        }
    }

    for (int i=0;i<n_row;i++) {
        for(int j=0;j<n_column;j++) {
            if (br_grid->at(i).at(j) == 0.0) {
                
                continue;
            }
            double p_edge;
            double p_corner;
            for(int k=i-1;k<=i+1;k++) {
                for (int l=j-1;l<=j+1;l++) {
                    // cout<<k<<" "<<l<<endl;
                    if (k<0 || k>=n_row) continue;
                    if (l<0 || l>=n_column) continue;
                    int mat_idx = (k-i+1)*3+(l-j+1);
                    if (br_grid->at(k).at(l) == 0) {
                        
                        trans_dist_matrix[i*n_column+j][mat_idx] = 0.0;
                        continue;
                    }
                    int non_edge_blocks = block_info_arr[k*n_column+l].non_edge_blocks;
                    int edge_blocks = block_info_arr[k*n_column+l].edge_adj_blocks;
                    double p_edge_total = edge_cell_mov_prob; 
                    // double p_corner_total = (1-p_edge_total);
                    if (edge_blocks==0 && non_edge_blocks==0) {
                        p_corner = 0.0;
                        p_edge = 0.0;
                    }
                    else if (edge_blocks == 0) {
                        p_edge = 0;
                        p_corner = 1.0;
                    }
                    else if (non_edge_blocks==0) {
                        p_edge = 1.0;
                        p_corner = 0.0;
                    }
                    else {
                        p_corner = (1-p_edge_total)/non_edge_blocks;
                        p_edge = p_edge_total/edge_blocks;
                    }
                    // p_corner = (1-p_edge_total)/non_edge_blocks;
                    // p_edge = p_edge_total/edge_blocks;
                    // cout<<k<<"-"<<l<<" "<<p_corner<<" "<<p_edge<<endl;
                    if (k==i && l==j) {
                        // cout<<k<<" "<<l<<endl;
                        // cout<<mat_idx<<endl;
                        
                        trans_dist_matrix[i*n_column+j][mat_idx] = p_corner;
                    }
                    else if (k==i || l==j) {
                        
                        trans_dist_matrix[i*n_column+j][mat_idx] = p_edge;
                    }
                    else {
                        trans_dist_matrix[i*n_column+j][mat_idx] = p_corner;
                    }
                    
                }
            }
        }
    }

    printBlockInfo(block_info_arr);
    printTransitionProbMatrix();
    
}

void GhostTracker::printBlockInfo (blockInfo block_info_arr[]) {
    fstream *blockInfofile = new fstream();
    blockInfofile->open("BlockInfo.txt", ios::out);
    redirectOutputStream(true, blockInfofile);

    for (int i=0;i<n_row;i++) {
        for (int j=0;j<n_column;j++) {
            cout<<"("<<block_info_arr[i*n_column+j].edge_adj_blocks<<", "
                << block_info_arr[i*n_column+j].non_edge_blocks<<") | ";
            // printf("(%d, %d) ", block_info_arr[i*n_column+j].edge_adj_blocks, 
            // block_info_arr[i*n_column+j].non_edge_blocks);
        }
        // printf("\n");
        cout<<endl;
    }
    redirectOutputStream(false);
    blockInfofile->close();
    delete blockInfofile;
}

void GhostTracker::printTransitionProbMatrix() {
    fstream *TPM_file = new fstream();
    TPM_file->open("TransitionProbabilityMatrix.txt", ios::out);
    redirectOutputStream(true, TPM_file);
    cout<<"P_t\t\tP_t+1\tVal"<<endl;
    cout<<"=====\t=====\t====="<<endl;
    for (int i=0;i<n_row;i++) {
        for(int j=0;j<n_column;j++) {
            // printf("%d,%d-->", i, j);
            if (br_grid->at(i).at(j) == 0.0) {
                continue;
            }
            for (int k=i-1;k<=i+1;k++) {
                for (int l=j-1;l<=j+1;l++) {
                    if (k<0 || k>=n_row) continue;
                    if (l<0 || l>=n_column) continue;
                    int mat_idx = (k-i+1)*3+(l-j+1);
                    // printf("%d,%d -->",k, l);
                    double p_t = trans_dist_matrix[i*n_column+j][mat_idx];
                    if (p_t>0.0) 
                        cout<<fixed<<setprecision(5)<<"P("<<k<<","<<l<<")\t"<<"P("<<i<<","<<j<<")\t"<<p_t<<endl;
                }
            }

        }
    }

    redirectOutputStream(false);
    TPM_file->close();
    delete TPM_file;
}

void GhostTracker::calculate_bstate_p(){
    // cout<< "Updating the bstate_prime"<<endl;
    this->isBstate_tCalc = true;
    for(int i=0;i<n_row;i++) {
        // printf("inside\n");
        for (int j=0;j<n_column;j++) {
            if (br_grid->at(i).at(j) == 0.0) {
                continue;
            }
            // printf("\n\n");
            // cout<<"calculating for "<<i<<","<<j;
            // printf("%d,%d-->", i, j);
            double sum_br =0.0;
            for (int k=i-1;k<=i+1;k++) {
                for (int l=j-1;l<=j+1;l++) {
                    if (k<0 || k>=n_row) continue;
                    if (l<0 || l>=n_column) continue;
                    int mat_idx = (k-i+1)*3+(l-j+1);
                    // printf("%d,%d -->",k, l);
                    double p_t = trans_dist_matrix[i*n_column+j][mat_idx];
                    double b_r = br_grid->at(k).at (l);
                    double b_r_p_val = p_t*b_r;
                    sum_br += b_r_p_val;
                    // if (p_t>0.0 && b_r >0.0) printf("%lf * %lf(%d, %d)->(%d) +", p_t, b_r, k, l, mat_idx);
                }
            }
            next_belief_matrix[i][j] = sum_br;
            // cout<<"->"<<sum_br<<endl;
            // printf("\n");
        }
        
    }
}

bool GhostTracker::update_curr_bstate(int row, int col, bool isCasperPresent) {
    if (row<0 || row>=n_row || col<0 || col>n_column) return false;
    this->t++;
    bool visited[n_row][n_column];
    for (int i=0;i<n_row;i++) {
        for (int j=0;j<n_column;j++) {
            visited[i][j] = false;
        }
    }
    double multiplier = isCasperPresent? true_positive : (1.0-true_positive);
    
    for (int i=row-1;i<=row+1;i++) {
        for (int j=col-1;j<=col+1;j++) {
            // printf("hello\n");
            if (i<0||i>=n_row||j<0||j>=n_column) continue;
            // br_grid->at(i).at(j) = br_grid->at(i).at(j)*multiplier;
            br_grid->at(i).at(j) = next_belief_matrix[i][j]*multiplier;
            
            visited[i][j] = true;
        }
    }
    
    double total_sum = 0.0;
    for (int i=0;i<n_row;i++) {
        for (int j=0;j<n_column;j++) {
            if (!visited[i][j]) 
                br_grid->at(i).at(j) = next_belief_matrix[i][j]*(1.0-multiplier);
            total_sum += br_grid->at(i).at(j);
        }
    }

    for (int i=0;i<n_row;i++) {
        for (int j=0;j<n_column;j++) {
            br_grid->at(i).at(j) = br_grid->at(i).at(j)/(total_sum);
        }
    }

    // cout<<"Updated Belief state"<<endl;
    // for (int i=0;i<n_row;i++) {
    //     for (int j=0;j<n_column;j++) {
    //         cout<<std::fixed<<setprecision(5)<< br_grid->at(i).at(j)<<" | ";
    //     }
    // cout<<endl;
    // for (int j=0;j<n_column;j++) {
    //     cout<< "======="<<" | ";
    // }
    // cout<<endl;
    // }
    // cout<<endl;

    return true;
}

void GhostTracker::printState () {
    if (t==0) {
        cout<<"Initial Probability, Time: "<<t<<endl;
    }
    else {
        cout<<"Probability Update, Time: "<<t<<endl;
    }
    for (int i=0;i<n_row;i++) {
        for (int j=0;j<n_column;j++) {
            cout<<std::fixed<<setprecision(5)<< br_grid->at(i).at(j)<<" | ";
        }
        cout<<endl;
        for (int j=0;j<n_column;j++) {
            cout<< "======="<<" | ";
        }
        cout<<endl;
    }
        cout<<endl;
}

void GhostTracker::printBstate_t () {
    if (this->isBstate_tCalc) {
        fstream *TPM_file = new fstream();
        TPM_file->open("ExpectedBelief.txt", (t==0)?ios::out:ios::out|ios::app);
        redirectOutputStream(true, TPM_file);

        cout<<"Calculated expected belief state at time = "<<t<<endl;
        for (int i=0;i<n_row;i++) {
            for (int j=0;j<n_column;j++) {
                cout<<std::fixed<<setprecision(5)<< next_belief_matrix[i][j]<<" | ";
            }
        cout<<endl;
        for (int j=0;j<n_column;j++) {
            cout<< "======="<<" | ";
        }
        cout<<endl;
        }
        cout<<endl;
        redirectOutputStream(false);
        TPM_file->close();
        delete TPM_file;
    }
}

void GhostTracker::printLikelyPos () {
    int r=0, c=0;
    double max = 0.0;
    for (int i=0;i<n_row;i++) {
        for (int j=0;j<n_column;j++) {
            if (br_grid->at(i).at(j) > max) {
                r = i;
                c = j;
                max = br_grid->at(i).at(j);
            }
        }
    }
    cout<<endl;
    cout<<"======================================"<<endl;
    cout<<"Casper is likely at Cell: {"<<r<<","<<c<<"}"<<endl;
    cout<<"======================================"<<endl;
    cout<<endl;
}

GhostTracker::~GhostTracker()
{
}