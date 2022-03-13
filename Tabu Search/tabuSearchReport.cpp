#include <bits/stdc++.h>
#include <ctime>
#include <fstream>

using namespace std;
#include "tabuSearch.h"

#define INF 1e9
/*int median (vector<int> costs) {

}
*/
/*
Helping function to sort vector of tuples according to the second element of each tuple

Reference: https://www.geeksforgeeks.org/sorting-vector-tuple-c-ascending-order/
*/
bool sortbysec(const tuple<vector<int>, int>& a, 
               const tuple<vector<int>, int>& b)
{
    return (get<1>(a) < get<1>(b));
}

void printReport(vector<vector<int>> graph, int n, int maxIter) {
    cout << "General data: \n";
    cout <<"\t n = " << n << "\n\t max. Iterations = " << maxIter << "\n Graph of connections between cities" << endl;
    printGraph(graph,n);
}

string inputFile() {
    string file;
    cout << "Name of file: ";
    cin >> file;
    return file;
}

double solutionsMean(vector<tuple<vector<int>,int>> solutions, int M) {
    double costSum = 0;
    for(int i = 0; i < M; i++) {
        costSum += double(get<1>(solutions[i]));
    }
    return double(costSum/double(M));
}

double solutionsSD(vector<tuple<vector<int>,int>> solutions, int M) {
    double SS = 0;
    double mu = solutionsMean(solutions, M);
    for(int i = 0; i < M; i++) {
        SS += pow(double(get<1>(solutions[i])) - mu,2);
    }

    cout << "returning sd of " << SS/double(M) << endl;
    return sqrt(double(SS/double(M)));
}

void printReport(vector<int> minSol, int minCost, vector<int> maxSol, int maxCost, vector<int> medianSol, int medianCost, double mean, double sd, int M) {
    cout << "M = " << M << endl;
    cout << "1. Best solution: " << endl;
    printVector(minSol); 
    cout << "Cost: " << minCost << endl << endl;
    cout << "2. Worst solution: " << endl;
    printVector(maxSol);
    cout << "Cost: " << maxCost << endl << endl;
    cout << "3. Median of solutions: " << endl;
    printVector(medianSol); 
    cout << "Cost: " << medianCost << endl << endl;
    cout << "4. Mean of solutions: " << mean << endl << endl;
    cout << "5. Standard deviation of solutions: " << sd << endl << endl;


}
int chooseMethod() {
    cout << "Choose one of the following options" << endl;
    cout << "1. Solve the problem once given an input file" << endl;
    cout << "2. Solve the problem M times and give a report" << endl;

    int i = 0;
    cout << "Option: ";
    cin >> i;
    while(true) {
        if(i == 1 || i == 2) break;
        cin.clear();
        cin.ignore();
        cin >> i;
    }

    return i;
}

int main() {
    string exit = "0";

    while(true) {
        cout << "To end the program, press 1 at this moment. Otherwise insert any other key.\nExit? ";
        cin >> exit;
        if(exit == "1") break;

        int method = chooseMethod();
        //Print one result
        if(method == 1) {
            string file = inputFile();
            auto[n,maxIter,graph] = readInput(file);
            auto[solution, cost] = tabuSearch(graph,n,maxIter,n/2);
            cout << "Path: ";
            for(int i = 0; i < n; i++) cout << solution[i] << " ";
            cout << endl;
            cout << "Cost: " << cost << endl;

        //Report after iterating M times
        } else {
            int M;
            string file = inputFile();
            auto[n,maxIter,graph] = readInput(file);
            cout << "Choose a value of M\nM: ";
            cin >> M;
            vector<tuple<vector<int>,int>> solutions;

            for(int i = 0; i < M; i++) {
                auto[solution, cost] = tabuSearch(graph,n,maxIter,n/2);
                solutions.push_back({solution,cost});
            }
            sort(solutions.begin(), solutions.end(), sortbysec); // sort solutions by cost in ascending order
            /*for(int i = 0; i < solutions.size(); i++) {
                printVector(get<0>(solutions[i]));
                cout << get<1>(solutions[i]) << endl;
            }*/
            auto[minSol, minCost] = solutions[0]; // Best solution
            auto[maxSol,maxCost] = solutions[M-1]; // Worst solution
            auto[medianSol, medianCost] = solutions[(M-1)/2]; //Median of solutions
            double solMean = solutionsMean(solutions,M);
            double solSD = solutionsSD(solutions,M);
            printReport(minSol,minCost,maxSol,maxCost,medianSol,medianCost,solMean,solSD,M);

        }
    }
    return 0;
}