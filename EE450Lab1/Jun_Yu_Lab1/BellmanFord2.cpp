//
//  BellmanFord2.cpp
//  EE450lab1
//
//  Created by 余珺 on 5/22/19.
//  Copyright © 2019 Jun. All rights reserved.
//

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <climits>
#include <regex>

using namespace std;

//read the graph data in the given file
vector<vector<int> > readfile(const string file) {
    vector<vector<int> > graph;
    ifstream infile(file);
    string line;
    
    if(!infile){
        cerr << "WARNING: Error when open the file " << file << endl;
    }
    
    while (getline(infile, line)) {
        stringstream iss(line);
        vector<string> tmp;
        vector<int> vertex;
        string str;
        
        //read the graph data line by line
        while (iss.good()) {
            getline(iss, str, ',');
            tmp.push_back(str);
        }
        
        for (string target : tmp) {
            if (target.find("inf") != string::npos){  // if read "inf", the result stored as -1
                vertex.push_back(-1);
            }
            else {
                target = target.substr(0, target.size()-1);
                vertex.push_back(stoi(target));   // store the path value
            }
        }
        
        graph.push_back(vertex);
    }
    
    return graph;
}



//implement BellmanFord Algorithm to find the shortest path in the graph given in file
vector<int> BellmanFordImp(vector<vector<int> > graph) {
    int numNodes = (int)graph.size();
    vector<int> pathLength(0);
    
    //initialization
    for (int i = 1; i < numNodes; i++){
        pathLength.push_back(INT_MAX);
    }
    
    vector<int> curr_pl(pathLength); // store the result of last iteration
    vector<int> visited(pathLength); // store node that visited
    vector<int> prev(numNodes, 0); // store previous value
    
    int count = 0; // count num of iterations
    for (int i = 0; i < numNodes-1; i++) {
        count++;
        for (int s = 0; s< visited.size(); s++ ) {
            for (int t = 0; t < numNodes; t++) {
                if (graph[s][t] < 0) continue;
                if (pathLength[t] > pathLength[s] + graph[s][t] && pathLength[s] != INT_MAX) {
                    pathLength[t] = pathLength[s] + graph[s][t]; // new pathlength is shorter than current one
                    prev[t] = s;  // set source node as the previous of the ended one
                    visited.push_back(t);
                }
            }
        }
        
        if (pathLength == curr_pl) break;
        else curr_pl = pathLength;
        
        //detect the negative loop
        bool flag=1;
        for(int s=0; s<numNodes; s++){
            for(int t=0 ; t< graph[s].size(); t++){
                if (pathLength[t] > pathLength[s] + graph[s][t] && pathLength[s] != INT_MAX) {
                    flag=0;
                    break;
                }
            }
        }
    }
    
    
    //print out the result path
    vector<string> pathsGen(numNodes, "");
    pathsGen[0] = "0";
    for (int i = 1; i < numNodes; i++) {
        int tmp = prev[i];
        while (tmp != 0) {
            pathsGen[i] = "->" + to_string(tmp) + pathsGen[i];
            tmp = prev[tmp];
        }
        pathsGen[i] = "0" + pathsGen[i];
        pathsGen[i] += ("->" + to_string(i));
    }
    
    //print out the results into a txt file
    ofstream file;
    file.open ("output-N" + to_string(numNodes) +".txt");
    for (int l : pathLength){
        file << l << ",";
    }
    file << endl;
    for (const string &str : pathsGen){
        file << str << endl;
    }
    file << "Iteration: " << count << endl;
    file.close();
    
    return pathLength;
}


int main(int argc, char **argv) {
    vector<vector<int> > info = readfile(argv[1]);
    vector<int> l = BellmanFordImp(info);
 
    return 0;
}
