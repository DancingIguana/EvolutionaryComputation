#include <bits/stdc++.h>
#include <ctime>
#include <fstream>
#include "tabuSearch.h"
#define INF 1e9

using namespace std;

/*
Input: graph
Action: print graph in terminal
*/
void printGraph(vector<vector<int>> graph, int n) {
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            cout << graph[i][j] << "\t";
        }
        cout << endl;
    }
}
/*
Input: vector/array
Action: print vector in terminal
*/
void printVector(vector<int> v) {
    for(int i = 0; i < v.size(); i++) cout << v[i] <<" ";
    cout << endl;
}

/*
Input: graph, solution, number of nodes
Output: traveling cost of the solution 
*/
int solutionCost(vector<vector<int>> graph, vector<int> path, int n) {
    int current = 0;
    int cost = 0;
    while(current < n) {
        //cout << path[current] << "->" << path[(current+1)%n] << " " << graph[path[current]][path[(current+1)%n]] << endl;
        cost += graph[path[current]][path[(current+1)%n]];
        current++;
    }
    return cost;
}

vector<vector<int>> updateTabuTime(vector<vector<int>> tabu) {
    int i = 0;
    while(i < tabu.size()) {
        tabu[i][1] -= 1;
        if(tabu[i][1] <= 0) {
            tabu.erase(tabu.begin() + i);
        } else {
            i++;
        }
    }

    return tabu;
}

/*
Input: graph
Action: generate an initial solution starting from city 0 and using a greedy approach (less cost, the better).
Output: permutation of the cities
*/
tuple<vector<int>,int> initialSolution(vector<vector<int>> graph, int n) {
    set<int> visited;
    vector<int> sol;
    int nextCity;

    int currentCity = 0;
    sol.push_back(currentCity);
    visited.insert(currentCity);

    while(visited.size() < n) { //Visit all cities
        int minCost = INF;
        
        // Get the closest non visited city for the current one
        for(int i = 0; i < n; i++) {
            if(i != currentCity && visited.find(i) == visited.end() && graph[currentCity][i] < minCost) {
                nextCity = i;
                minCost = graph[currentCity][i];
            }
        }
        //cout << currentCity << "->" << nextCity << endl;
        //Update based on previous result
        currentCity = nextCity;
        sol.push_back(currentCity);
        visited.insert(currentCity);
    }


    return {sol, solutionCost(graph, sol, n)};
}

/*
Input: Neighborhood solutions
Output: the best solution of said neighborhood {permutation, cost}
*/
tuple<vector<int>, int> findBestSolutionInNeighborhood(vector<tuple<vector<int>,int>> N) {
    vector<int> bestSol;
    int bestF = INF;

    //cout << "Getting best result in neighborhood" << endl;
    for(int i = 0; i < N.size(); i++) {
        
        vector<int> path = get<0>(N[i]);
        int cost = get<1>(N[i]);
        //printVector(path); cout << cost << endl; 
        if(cost < bestF) { //If the cost is less than the current best one
            bestSol = path;
            bestF = cost;
        }
    }

    //cout << "Best result: "; printVector(bestSol); cout <<  bestF << endl << endl;
    return {bestSol, bestF};
}

/*
Input: graph, permutation, number of nodes and tabu list
Action: generate a neighborhood, by choosing a random city that is NOT in the tabu list
Output: neighborhood {{{permutation, cost}, {permutation, cost},...,{...}}, movedCity}
*/
tuple<vector<tuple<vector<int>, int>>, int> neighborhood(vector<vector<int>> graph, vector<int> path, int n, vector<vector<int>> tabu) {
    int cityPos = rand() % (n-1) + 1; //Choose a random position
    int city = path[cityPos];

    set<int> tabuSols;

    for(int i = 0; i < tabu.size();i++) { // Get all tabu cities in a set
        tabuSols.insert(tabu[i][0]);
    }

    while(tabuSols.find(city) != tabuSols.end()) { //choose a city that is NOT in our tabu list
        cityPos = rand() % (n-1) + 1; 
        city = path[cityPos];
    }
    vector<tuple<vector<int>, int>> N;
    vector<int> newPath = path;

    newPath.erase(newPath.begin() + cityPos);

    //Generate all permutations
    for(int i = 1; i < n; i++) {
        if(i == cityPos) continue;
        newPath.insert(newPath.begin() + i, city);
        N.push_back({newPath, solutionCost(graph,newPath,n)}); //Store the permutation, and its cost
        newPath.erase(newPath.begin() + i);
    }
    return {N,city}; // return vector of solutions and the modified city
}

/*
Input: graph, number of nodes, maximum number of iterations, tabu list time
Action: perform tabu search for the traveling salesman problem given a graph of fully connected nodes
Output: permutation and cost of the best found solution
*/
tuple<vector<int>, int> tabuSearch(vector<vector<int>> graph, int n, int maxIter, int tabuTime) {
    tuple<vector<int>,int> x;
    vector<vector<int>> tabu;
    vector<int> bestSolution;
    vector<tuple<vector<int>, int, int>> N;
    int bestF;

    x = initialSolution(graph, n);
    //cout << "Initial Solution:\n";
    //printVector(get<0>(x));
    //cout << "Cost: " << get<1>(x) << endl;
    bestSolution = get<0>(x);
    bestF = get<1>(x); 
    tabu = {};
    for(int k = 0; k < maxIter; k++) {
        //Neighborhood
        tuple<vector<tuple<vector<int>, int>>, int> N = neighborhood(graph, get<0>(x), n, tabu);

        int movedCity = get<1>(N);
        vector<tuple<vector<int>,int>> solutions = get<0>(N);
        
        //Find and store the best solution in the neighborhood
        x = findBestSolutionInNeighborhood(solutions);
        vector<int> bestNP = get<0>(x); //permutation
        int bestNF = get<1>(x); //cost

        // If the solution that we got from the neighborhood is better update the best global result
        if(bestNF < bestF) { 
            bestF = bestNF;
            bestSolution = bestNP;
        }

        //Update tabu list
        tabu = updateTabuTime(tabu);
        if(tabuTime > 0) {
            tabu.push_back({movedCity,tabuTime});
        }
    }
    return {bestSolution,bestF};
}


/*
Input: file

Action: read all of the input from the designated file
*/
tuple<int,int,vector<vector<int>>> readInput(string file) {
    ifstream infile; //Use instead of cin to read input from file
    infile.open(file);
    //string s;

    int n, maxIter;
    infile >> n >> maxIter;
    //cin >> n >> maxIter;
    //vector<vector<int>> cities = defineGraph(n);


    vector<vector<int>> graph(n, vector<int>(n,0)); //Initialize graph


    //Receive inputs (graph as a 2D matrix)
    for(int i = 0; i < n - 1; i++) {
        for(int j = i+1; j < n; j++) {
            int aux;      
            infile >> aux;
            graph[i][j] = graph[j][i] = aux;
            }
    }

    return {n,maxIter,graph};
}
/*
int main () {
    srand(time(NULL)); // Random seed    
    vector<vector<int>> tabu = {};
    auto[n,maxIter,cities] = readInput();
    auto[path, cost] = tabuSearch(cities, n, maxIter, n/2);
    cout << "Final result = " << endl;
    printVector(path);
    cout << cost;
    //printGraph(cities,n);
    return 0;
} 
*/