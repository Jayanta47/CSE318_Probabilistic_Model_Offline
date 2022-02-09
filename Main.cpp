#include "GhostTracker.cpp"


int main(int argc, char **argv) {
    auto start_time =chrono::high_resolution_clock::now();
    consoleStream = true;
    if (argc >= 2) {
        if (argc != 3) {
            cout<<"Insufficient command line arguments."<<endl;
            cout<<"Format: [object file] [input file name] [output file name]"<<endl;
            exit(1);
        }
        inputFileName = new string(argv[1]);
        outputFileName = new string(argv[2]); 
        inpFile = new fstream();
        outFile = new fstream();
        inpFile->open (*inputFileName, ios::in);
        outFile->open (*outputFileName, ios::out);

        redirectStream();
    }

    
    int n, m, k;
    cin>>n>>m>>k;

    vector<vector<double>> *grid = new vector<vector<double>> (n);
    double init_prob = 1.0/(n*m-k);
    for (int l=0;l<n;l++) {
        grid->at (l) = vector<double>(m, init_prob);
    }

    int i, j;
    for (int l=0; l<k ;l++) {
        cin>>i>>j;
        grid->at(i).at(j) = 0; 
    } 
    
    GhostTracker gtk(grid, k);
    gtk.printState();
    
    while(cin.peek() != char_traits<char>::eof()) {
        char s;
        cin>>s;
        
        if (s == 'R') {
            int row, col, sensorData;
            cin>>row>>col>>sensorData;
            gtk.calculate_bstate_p();
            gtk.printBstate_t();
            cout<<"Received sensor Reading at cell ("<<row<<","<<col<<"): "<<(sensorData==1?"Casper Present":"Casper Absent")<<endl;
            
            gtk.update_curr_bstate (row, col, sensorData==1);
            
            gtk.printState();
        }
        else if (s== 'C') {
            gtk.printLikelyPos();
        }
        else if (s == 'Q') {
            cout<< "Bye Casper!" <<endl;
            break;
        }
        else {
            printf("Invalid command %c\n", s);
        }
    } 

    
    

    redirectStream();
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    cout<<"Simulation Run Successfully"<<endl<<"Total Time: "<<duration.count()<< " ms"<<endl;
    return 0;
}