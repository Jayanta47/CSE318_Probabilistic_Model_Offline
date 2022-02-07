#include "GhostTracker.h"


int main(int argc, char **argv) {

    string *inputFileName = nullptr;
    string *outputFileName = nullptr;

    fstream *inpFile;
    fstream *outFile;

    streambuf *old_stream_buf_cin;
    streambuf *old_stream_buf_cout;

    if (argc<2) {
        
    }
    else {
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
    
    // streambuf* oldCoutStreamBuf = cout.rdbuf();
    // ostringstream strCout;
    // cout.rdbuf (strCout.rdbuf ());

    // cout<< "Hello World";

    // cout.rdbuf (oldCoutStreamBuf);
    // cout<<strCout.str()<<endl;

    
    int n, m, k;
    cin>>n>>m>>k;

    vector<vector<int>> *grid = new vector<vector<int>> (n);

    for (int l=0;l<n;l++) {
        grid->at (l) = vector<int>(m, 1);
    }

    int i, j;
    for (int l=0; l<k ;l++) {
        cin>>i>>j;
        grid->at(i).at(j) = 0; 
    } 

    while(cin.peek() != char_traits<char>::eof()) {
        char s;
        cin>>s;
        cout<<s;
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
            cout<<grid->at(i).at(j)<<" ";
        }
        cout<<endl;
    }

}