#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <time.h>
#include <limits>
#include <ctime>
#include <vector>

using namespace std;

//GLOBAL VARIABLES
int numVertex,numEdges,k;
int numberOfVariables;

vector<vector<int> >  edgeMatrixGraph; //ksubgraph, edgeSpanMatrix
vector<vector<int> > numksubgraph, numedgeMatrixGraph, numedgeSpanMatrix;

vector<vector<vector<int> > > edgeChecker, subgraphChecker; 

//Take Input here.
//read the graph edges as adjacency matrix(GIVE APPROPRIATE NAME FOR MATRIX: SO THAT I CAN GET IT)
void splitString(string message, string delimiter, string result[], int n) {
    int i = 0, pos = 0, length = 0, temp;
    temp = message.find ( delimiter.c_str ( ), pos );
    while ( temp != -1 )
    {
        length = temp - pos;
        result[i] = message.substr ( pos, length );
        pos = temp + delimiter.size ( );
        temp = message.find ( delimiter.c_str ( ), pos );
        i++;
    }
    result[i] = message.substr ( pos );
    i++;
    if ( i != n )
    {
        cout << "The similarity matrix does not have the correct format.";
        exit ( 0 );
    }
}

void takeInput(string filename){
    filename=filename+".graph";
    vector<string> lines;
    string line;
    ifstream myfile ( filename.c_str () );
    if ( myfile.is_open ( ) )
    {
        while ( getline ( myfile, line ) )
         { //  cout<<"Line read:"<<line<<endl;
            lines.push_back ( line );
        }
        myfile.close ( );
    }
    else
    {
        cout << "Unable to open input file";
        exit ( 0 );
    }
        
    string tempLine = lines[0];
    string *elements = new string[3];
    splitString ( tempLine, " ", elements, 3 );
    numVertex = atof ( elements[0].c_str () ); //(0 - numVertex-1) 
    numEdges = atof ( elements[1].c_str () );
    k = atof ( elements[2].c_str () );

    edgeMatrixGraph.resize(numVertex, vector<int> (numVertex, 0));
    for(int i=0;i<numEdges;i++){
        string tempLine = lines[i+1];
        string *elements = new string[2];
        splitString ( tempLine, " ", elements, 2 );
        int row = atof ( elements[0].c_str () );
        int col = atof ( elements[1].c_str () );    
        edgeMatrixGraph[row-1][col-1]=1;
        edgeMatrixGraph[col-1][row-1]=1;
    }
    // cout<<"numVertex printed:"<<numVertex<<" "<<numEdges<<" "<<k<<endl;

///lets assume i have starting 3 parameters as: numV, numE,k
///i filled edgeMatrixGraph by 0/1
    int count=1;

    numksubgraph.resize(k, vector<int> (numVertex, 0));
    numedgeMatrixGraph.resize(numVertex, vector<int> (numVertex, 0));
    numedgeSpanMatrix.resize(numVertex, vector<int> (numVertex, 0));
    edgeChecker.resize(k,vector<vector<int > > (numVertex, vector<int> (numVertex, 0)));
    subgraphChecker.resize(k,vector<vector<int > > (k, vector<int> (numVertex, 0)));
   
    for(int i=0;i<k;i++){
        for(int j=0;j<numVertex;j++){
            numksubgraph[i][j]= count;
            // cout<<count<<" ";
            count++;
        }
        // cout<<endl;
    }
        // cout<<numVertex<<"---------- "<<numEdges<<" "<<k<<endl;
    for(int i=0;i<numVertex;i++){
        for(int j=0;j<numVertex;j++){
            numedgeMatrixGraph[i][j]= count;
            // cout<<count<<" ";
            count++;
        }
        // cout<<endl;
    }
    // cout<<numVertex<<"********** "<<numEdges<<" "<<k<<endl;
    // for(int i=0;i<numVertex;i++){
    //     for(int j=0;j<numVertex;j++){
    //         numedgeSpanMatrix[i][j]= count;
    //         count++;
    //     }
    // }
    for(int i=0;i<k;i++){
        for(int nV1=0;nV1<numVertex;nV1++){
            for(int nV2=0; nV2<numVertex ;nV2++){
                edgeChecker[i][nV1][nV2]=count;
                count++;
            }
        }
    }
    for(int i=0;i<k;i++){
        for(int j=0;j<k;j++){
            for(int nV=0; nV<numVertex ;nV++){
                subgraphChecker[i][j][nV]=count;
                count++;
            }
        }
    }


        numberOfVariables = count-1;
        cout << "yaay:numberOfVariables:"<<numberOfVariables; // in 1 indexing 0-numVariables

}


void printInOutoutFile(vector<string> clauses, int numberOfVariables, string filename){

    ofstream fout;
    int numberOfClauses=clauses.size();
    filename=filename+".satinput";  
    fout.open(filename, ios::out | ios::app);
    fout <<"p cnf "<<numberOfVariables<<" "<<numberOfClauses<<"\n";
    for(int i=0;i<numberOfClauses;i++){
        fout<<clauses[i]<<" 0";
        if(i<numberOfClauses-1)fout<<"\n" ; //Just to remove last \n
        // cout<<"\n"<<clauses[i];
    }
    fout.close();


}

int main(int argc, char** argv ) {

	    string inputfilename ( argv[1] );
		takeInput(inputfilename);

        //ready with graph indexes as 1-n(vertex) and edges as matriz[n+1][n+1] (I think 1 indexed would be easier; Think first then implemnt)
		//CNF for 3 properties-->(span the global graph in nodes and edges), (complete graph), (NO subgraph of each other)
		//k subgraohs represented as (k*n bool/ (0-1) matrix); every row represents vertices corresponding to it.
	
    vector<string> clauses (0);

/**********    EDGE MATRIX GRAPH    **********/
for(int i=0;i<numVertex;i++){
    for(int j=0;j<numVertex;j++){
        string temp;
        if(edgeMatrixGraph[i][j]==1){temp = to_string(numedgeMatrixGraph[i][j]);}
        else if(edgeMatrixGraph[i][j]==0){temp = to_string(-1*(numedgeMatrixGraph[i][j]));}        
        clauses.push_back(temp);
    }
}

/**********    SPAN VERTEX    **********/
	for(int i=0;i<numVertex;i++){
        string temp="";
		for(int j=0;j<k;j++){
            temp=temp+to_string(numksubgraph[j][i]) ;//EXTRA " " space is printed at the end TAKE A NOTE numksubgraph[k][n] 
		    if(j<k-1)temp=temp+" ";
        }
        clauses.push_back(temp);
	}

/**********    NON EMPTY SUBGRAPH    **********/
    for(int i=0;i<k;i++){
        string temp="";
        for(int j=0;j<numVertex;j++){
            temp=temp+to_string(numksubgraph[i][j]) ;//EXTRA " " space is printed at the end TAKE A NOTE numksubgraph[k][n] 
            if(j<numVertex-1)temp=temp+" ";
        }
        clauses.push_back(temp);
    }


/**********    SPAN EDGE    **********/
    

    for(int p=0;p<k;p++){
        for(int i=0;i<numVertex;i++){
            for(int j=0; j<numVertex ;j++){
                if(i!=j){
                    string temp="";
                    temp=temp+to_string(-1*numksubgraph[p][i])+" ";
                    temp=temp+to_string(-1*numksubgraph[p][j])+" ";
                    temp=temp+to_string(edgeChecker[p][i][j]);
                    clauses.push_back(temp);

                    string temp1="";
                    temp1=temp1+to_string(numksubgraph[p][i])+" ";
                    temp1=temp1+to_string(-1*edgeChecker[p][i][j]);
                    clauses.push_back(temp1);

                    string temp2="";
                    temp2=temp2+to_string(numksubgraph[p][j])+" ";                
                    temp2=temp2+to_string(-1*edgeChecker[p][i][j]);
                    clauses.push_back(temp2);
                }
            }
        }
    }



   
    for(int i=0;i<numVertex;i++){
        for(int j=0;j<numVertex;j++){
            if(i!=j){   
                string temp1 = to_string(-1*numedgeMatrixGraph[i][j]) +" ";
                for(int p=0;p<k;p++){
                    if(p!=k-1) temp1 += to_string(edgeChecker[p][i][j]) +" ";
                    else temp1 += to_string(edgeChecker[p][i][j]);

                    string temp2="";
                    temp2 = temp2 + to_string(numedgeMatrixGraph[i][j]) +" ";
                    temp2= temp2 + to_string(-1*edgeChecker[p][i][j]);
                    clauses.push_back(temp2);
                }
                clauses.push_back(temp1);
            } 
        }
    }








/**********    COMPLETE    **********/
    for(int i=0;i<k;i++){
        for(int nV1=0;nV1<numVertex;nV1++){
            for(int nV2=0; nV2<numVertex;nV2++){
                if(nV1!=nV2){
                                string temp="";
                                temp=temp+to_string(numedgeMatrixGraph[nV1][nV2])+" ";
                                temp=temp+to_string(-1*numksubgraph[i][nV1])+" ";
                                temp=temp+to_string(-1*numksubgraph[i][nV2]);
                                clauses.push_back(temp);
                                
                            //     string temp1="";
                            //     temp1=temp1+to_string(-1*numedgeMatrixGraph[nV1][nV2])+" ";
                            //     temp1=temp1+to_string(numksubgraph[i][nV1]);
                            //     clauses.push_back(temp1);
                                
                            //     string temp2="";
                            //     temp2=temp2+to_string(-1*numedgeMatrixGraph[nV1][nV2])+" ";
                            //     temp2=temp2+to_string(numksubgraph[i][nV2]);
                            //     clauses.push_back(temp2);                
                            // }
            }
        }
    }
}
/**********    NO SUBGRAPH    **********/
    for(int i=0;i<k;i++){
        for(int j=0;j<k;j++){
                
            if(i!=j){
                string temp="";
                for(int nV=0; nV<numVertex ;nV++){
                    temp=temp+to_string(-1*numksubgraph[i][nV])+" ";
                    temp=temp+to_string(numksubgraph[j][nV])+" ";
                    temp=temp+to_string(subgraphChecker[i][j][nV])+" ";
                    clauses.push_back(temp);
                    temp="";
                    temp=temp+to_string(numksubgraph[i][nV])+" ";
                    temp=temp+to_string(-1*subgraphChecker[i][j][nV])+" ";
                    clauses.push_back(temp);
                    temp="";
                    temp=temp+to_string(-1*numksubgraph[j][nV])+" ";
                    temp=temp+to_string(-1*subgraphChecker[i][j][nV])+" ";
                    clauses.push_back(temp);
                }
            }
        }
    }
    for(int i=0;i<k;i++){
        for(int j=0;j<k;j++){
            if(i!=j){
                string temp="";
                for(int nV=0; nV<numVertex ;nV++){
                    temp=temp+to_string(subgraphChecker[i][j][nV])+" ";
                }
                clauses.push_back(temp);
            }
        }
    }


printInOutoutFile(clauses, numberOfVariables, inputfilename);

}


/*********** Main Ended **********/



















// for(int i=0;i<k;i++){
//         for(int j=0;j<k;j++){
                
            
//             // if(i>j){
//             //     for(int nV=0; nV<numVertex ;nV++){
                    
//             //         // string temp="";
//             //         // temp=temp+to_string(-1*subgraphChecker[i][j][nV])+" ";
//             //         // temp=temp+to_string(subgraphChecker[j][i][nV])+" ";
//             //         // clauses.push_back(temp);
//             //         // temp="";
//             //         // temp=temp+to_string(subgraphChecker[i][j][nV])+" ";
//             //         // temp=temp+to_string(-1*subgraphChecker[j][i][nV])+" ";
//             //         // clauses.push_back(temp);
                    
//             //         ////////
//             //         string temp="";
//             //         temp=temp+to_string(-1*numksubgraph[i][nV])+" ";
//             //         temp=temp+to_string(numksubgraph[j][nV])+" ";
//             //         temp=temp+to_string(subgraphChecker[i][j][nV])+" ";
//             //         clauses.push_back(temp);
//             //         temp="";
//             //         temp=temp+to_string(numksubgraph[i][nV])+" ";
//             //         temp=temp+to_string(-1*subgraphChecker[i][j][nV])+" ";
//             //         clauses.push_back(temp);
//             //         temp="";
//             //         temp=temp+to_string(-1*numksubgraph[j][nV])+" ";
//             //         temp=temp+to_string(-1*subgraphChecker[i][j][nV])+" ";
//             //         clauses.push_back(temp);
//             //     }
//             // }
//             // else if(i<j){
//             //     for(int nV=0; nV<numVertex ;nV++){
//             //         string temp="";
//             //         temp=temp+to_string(numksubgraph[i][nV])+" ";
//             //         temp=temp+to_string(-1*numksubgraph[j][nV])+" ";
//             //         temp=temp+to_string(subgraphChecker[i][j][nV])+" ";
//             //         clauses.push_back(temp);
//             //         temp="";
//             //         temp=temp+to_string(-1*numksubgraph[i][nV])+" ";
//             //         temp=temp+to_string(-1*subgraphChecker[i][j][nV])+" ";
//             //         clauses.push_back(temp);
//             //         temp="";
//             //         temp=temp+to_string(numksubgraph[j][nV])+" ";
//             //         temp=temp+to_string(-1*subgraphChecker[i][j][nV])+" ";
//             //         clauses.push_back(temp);
//             //     }
//             // }
//         }
//     }

////  DEBUGG COMMENTS:
//cout<<"strat"<<endl;
// cout<<"after takeinput"<<endl;


////span edge
// for(int i=0;i<k;i++){
    //     for(int nV1=0;nV1<numVertex;nV1++){
    //         for(int nV2=0; nV2<numVertex ;nV2++){
    //             if(nV2!= nV1){
    //                             string temp="";
    //                             temp=temp+to_string(-1*numksubgraph[i][nV2])+" ";
    //                             temp=temp+to_string(-1*numksubgraph[i][nV1])+" ";
    //                             temp=temp+to_string(numedgeSpanMatrix[nV1][nV2]);
    //                             clauses.push_back(temp);
            
    //                             string temp1="";
    //                             temp1=temp1+to_string(numksubgraph[i][nV1])+" ";
    //                             temp1=temp1+to_string(-1*numedgeSpanMatrix[nV1][nV2]);
    //                             clauses.push_back(temp1);
            
    //                             string temp2="";
    //                             temp2=temp2+to_string(numksubgraph[i][nV2])+" ";                
    //                             temp2=temp2+to_string(-1*numedgeSpanMatrix[nV1][nV2]);
    //                             clauses.push_back(temp2);
    //             }
    //         }
    //     }
    // }


    // for(int i=0;i<numVertex;i++){
    //     for(int j=0;j<numVertex;j++){
    //         string temp1="", temp2="" ;
    //         temp1 = temp1 + to_string(numedgeMatrixGraph[i][j]) +" "+ to_string(-1*numedgeSpanMatrix[i][j]);
    //         temp2 = temp2 + to_string(-1*numedgeMatrixGraph[i][j]) +" "+to_string(numedgeSpanMatrix[i][j]);
    //         clauses.push_back(temp1);
    //         clauses.push_back(temp2);
    //     }
    // }
 // for(int i=0;i<numVertex;i++){
    //     for(int j=0;j<numVertex;j++){
    //         if(i!=j){
    //                     string temp1="";
    //                     temp1 = temp1 + to_string(-1*numedgeMatrixGraph[i][j]) +" ";
    //                     for(int p=0;p<k;p++){
    //                         temp1=temp1+ to_string(edgeChecker[p][i][j]);
    //                         if(p<k-1)temp1=temp1+" ";
    //                     } 
    //                     clauses.push_back(temp1);
    //                 }
    //     }
    // }
