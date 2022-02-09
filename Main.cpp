#include "GhostTracker.cpp"


int main(int argc, char **argv) {

    string *inputFileName = nullptr;
    string *outputFileName = nullptr;

    fstream *inpFile;
    fstream *outFile;

    streambuf *old_stream_buf_cin;
    streambuf *old_stream_buf_cout;

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

        old_stream_buf_cin = cin.rdbuf();
        old_stream_buf_cout = cout.rdbuf();
        cout.rdbuf (outFile->rdbuf ());
        cin.rdbuf (inpFile->rdbuf ());
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

    while(cin.peek() != char_traits<char>::eof()) {
        char s;
        cin>>s;
        if (s == 'R') {
            int row, col, sensorData;
            cin>>row>>col>>sensorData;
            gtk.calculate_bstate_p();
            gtk.printBstate_t();
            gtk.update_curr_bstate (row, col, sensorData==1);
        }
        else if (s== 'C') {
            continue;
        }
        else if (s == 'Q') {
            cout<< "Bye Casper!" <<endl;
            exit(0);
        }
        else {
            printf("Invalid command %c\n", s);
        }
    } 

    
    


    // redirecting cout and cin to console 
    if (old_stream_buf_cin != nullptr) {
        cin.rdbuf (old_stream_buf_cin);
    }

    if (old_stream_buf_cout != nullptr) {
        cout.rdbuf (old_stream_buf_cout);
    }
    cout<< *inputFileName << " " <<*outputFileName<<endl;
    cout<<n<<" "<<m<<" "<<k<<endl;

    for (int i=0;i<n;i++) {
        for (int j=0;j<m;j++) {
            cout<<std::fixed<<setprecision(4)<< grid->at(i).at(j)<<" | ";
        }
        cout<<endl;
        for (int j=0;j<m;j++) {
            cout<< "======"<<" | ";
        }
        cout<<endl;
    }

}