#include <iostream>
#include <math.h>
#include <tuple>
#include <vector>
#include <fstream>
#include <tuple>
#include <random>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <stdlib.h>
#include "utilities.h"

/*
Function to read file input
Input: fileName
Output: tuple with
        - initial temperature (double)
        - final temperature (double)
        - number of objects (int)
        - backpack capacity
        - values of objects (vector<double>)
        - weights of objects 
*/
std::tuple<double,double, int, double, std::vector<double>, std::vector<double>> readInput(std::string file) {
    using namespace std;
    ifstream infile; //Use instead of cin to read input from file
    infile.open(file);

    double initialTemp, finalTemp, c;
    int N;
    vector<double> p, w;


    infile >> initialTemp >> finalTemp; //Initial and final temperatures
    infile >> N; //Number of objects
    infile >> c; //Backpack capacity

    for(int i = 0; i < N; i++) {
        double pi, wi;
        infile >> pi >> wi;
        p.push_back(pi); // Value from object i
        w.push_back(wi); // Weight from object i
    }

    return {initialTemp, finalTemp, N, c, p, w};
}

/*
Generate a random uniformly distributed double number between an interval
*/
double randomDouble(double lowerRange, double upperRange) {
    return ((double)rand() * (upperRange - lowerRange)) / (double)RAND_MAX + lowerRange;
}

/*
Helper function to return a vector of indexes where the value is true given a boolean a vector
Input: boolean vector
Output: int vector
*/
std::vector<int> getIndexFromBool(std::vector<bool> x) {
    using namespace std;
    vector<int> idxs;
    for(int i = 0; i < x.size(); i++) {
        if(x[i])
            idxs.push_back(i);
    }
    return idxs;
}

/*
Calculate value of the solution
Input:
    - boolean vector of solution, x
    - vector of values
Output:
    - value of the solution
*/
double f(std::vector<bool> x, std::vector<double> p) {
    int n = p.size();
    double curSum = 0;
    for(int i = 0; i < n; i++) curSum += (x[i]*p[i]);
    return curSum;
}

/*
Calculate the weight of the solution
Input:
    - boolean vector of solution, x
    - vector of weights
Output:
    - weight of the solution
*/
double g(std::vector<bool> x, std::vector<double> w) {
    int n = w.size();
    double weight;
    for(int i = 0; i < n; i++) weight += (x[i]*w[i]);
    return weight;
}

/*
Temperature function defined by 
T(t+1) = 0.99 * T(t)
*/
double T(double t) {
    return 0.99*t;
}

/*
Get the initial solution by randomly choosing a valid solution
Input: 
    - capacity of the backpack 
    - weights of the items
Output:
    - boolean vector that indicates which items to pick up
*/
std::vector<bool> initialSolution(double c, std::vector<double> p, std::vector<double> w) {
    using namespace std;
    int n = w.size();
    vector<int> idxs;
    for(int i = 0; i < n; i++) idxs.push_back(i);

    // Shuffle the index vector
    
    auto rd = std::random_device {}; 
    auto rng = std::default_random_engine { rd() };
    std::shuffle(std::begin(idxs), std::end(idxs), rng);    
    //cout << "Random positions: "; printVector(idxs); cout << endl;

    
    double weight = 0; //Current weight in the backpack
    vector<bool> iSol(n,0); //initial solution with 0 items in the backpack

    for(auto i : idxs) {
        if(weight + w[i] <= c) {
            iSol[i] = 1;
            weight += w[i];
        }
    }
    return iSol;
}

/*
Get just one neighboring valid solution by randomly generating one from the neighborhood
Input:
    - Solution x
    - Weights of items
    - Backpack capacity
Output:
    - boolean vector of neighboring solution
*/
std::vector<bool> neighbor(std::vector<bool> x, std::vector<double> w, double c) {
    int n = x.size(); 
    int pos = rand() % (n); //Choose a random position from 0 to n - 1
    std::vector<bool> nSol = x;
    do {
        nSol = x;
        pos = rand() % n;
        if(x[pos] == 1) {
            nSol[pos] = 0;
        } else {
            nSol[pos] = 1;
        }
    } while (g(nSol, w) > c);

    return nSol;
}

/*
More intelligent way to choose a neighbor solution. Now it can add a 1 to the solution
even if it exceeds backpack limit, by making 0 any other random elements until the 
solution is valid
Input:
    - Solution x
    - Weights of items
    - Backpack capacity
Output:
    - boolean vector of neighboring solution
*/
std::vector<bool> neighborImproved(std::vector<bool> x, std::vector<double> w, double c) {
    using namespace std;
    int n = x.size(); 
    int pos = rand() % (n); //Choose a random position from 0 to n - 1
    vector<bool> nSol = x;

    if(nSol[pos] == 0) {
        nSol[pos] = 1;

        //Generate a vector of all indexes that have a value of 1 except the currently chosen one
        vector<int> idxs = getIndexFromBool(nSol);
        std::vector<int>::iterator position = std::find(idxs.begin(), idxs.end(), pos);
        if (position != idxs.end()) 
            idxs.erase(position);

        auto rd = std::random_device {}; 
        auto rng = std::default_random_engine { rd() };
        std::shuffle(std::begin(idxs), std::end(idxs), rng);

        int i = 0;
        while(g(nSol, w) > c) {
            nSol[idxs[i]] = 0;
            i++;
        }
        
    } else {
        nSol[pos] = 0;
    }

    return nSol;
}

/*
Function to replicate the simulated annealing algorithm
Input: 
    - Values of items
    - Weights of items
    - Backpack maximum capacity
    - initial temperature
    - final temperature
    - improved neighbor method (boolean)
Output:
    - Solution boolean vector
    - Value of the solution
    - Weight of the solution
*/
std::tuple<std::tuple<std::vector<int>,std::vector<bool>>, double, double> simulatedAnnealing(std::vector<double> p, std::vector<double> w, double c, double temp, double fTemp, bool improvedNeighbor = false) {
    std::srand(std::time(0));
    using namespace std;
    vector<bool> x = initialSolution(c,p,w);
    double fx = f(x,p);
    while(temp > fTemp) {
        vector<bool> y;
        if(!improvedNeighbor) y = neighbor(x,w,c);
        else y = neighborImproved(x,w,c);

        double fy = f(y,p);
        if(fy >= fx) {
            x = y;
            fx = f(x,p);
        } else {
            double rn = randomDouble(0,1);
            if(rn < exp((fy - fx)/temp)) {
                x = y;
                fx = f(x,p);
            } 
        }
        temp = T(temp);
    }

    return {{getIndexFromBool(x),x},f(x,p),g(x,w)};
}