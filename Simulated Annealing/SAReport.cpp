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
#include "simulatedAnnealing.h"

bool sortbysec(const std::tuple<std::tuple<std::vector<int>,std::vector<bool>>,double,double>& a,  
                const std::tuple<std::tuple<std::vector<int>,std::vector<bool>>,double,double>& b)
{
    return (std::get<1>(a) < std::get<1>(b));
}


/*
Return the mean of all the solutions
Input: vector of tuple with solution elements
Output: mean of the value of the solutions
*/
double solutionsMean(std::vector<std::tuple<std::tuple<std::vector<int>,std::vector<bool>>,double,double>> solutions) {
    double curSum = 0;
    double m = solutions.size();
    for(int i = 0; i < solutions.size(); i++) {
        curSum += std::get<1>(solutions[i]);
    }
    return curSum/m;
}

/*
Return the standard deviation of all the solutions
Input: vector of tuple woth solution elements
Output: Standard deviation of the value of the solutions
*/
double solutionsSD(std::vector<std::tuple<std::tuple<std::vector<int>,std::vector<bool>>,double,double>> solutions) {
    double SS = 0;
    double M = solutions.size();
    double mu = solutionsMean(solutions);
    for(int i = 0; i < M; i++) {
        SS += pow(double(std::get<1>(solutions[i])) - mu,2);
    }

    return sqrt(double(SS/double(M)));
}
/*
Choose to solve the problem once (1) and print the solution or M times and display an statistic report (2)
*/
int chooseMethod() {
    using namespace std;
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

bool chooseNeighborMethod() {
    using namespace std;
    cout << "Do you want to use an improved neighbor method?" << endl;
    cout << "1. Yes" << endl;
    cout << "2. No" << endl;

    int i = 0;
    cout << "Option: ";
    cin >> i;
    while(true) {
        if(i == 1 || i == 2) break;
        cin.clear();
        cin.ignore();
        cin >> i;
    }

    if(i == 1) return 1;
    return 0;
}

/*
Function to input a string of the file name
*/
std::string inputFile() {
    std::string file;
    std::cout << "Name of file: ";
    std::cin >> file;
    return file;
}

void printSolution(std::tuple<std::tuple<std::vector<int>,std::vector<bool>>,double,double> solution) {
    using namespace std;
    
    printVector(get<0>(get<0>(solution)));
    cout << " "; for(auto i:get<1>(get<0>(solution))) cout << i; cout << "\n";
    cout << get<1>(solution) << "\n" << get<2>(solution) << "\n";
    
}

void generateReport(std::vector<std::tuple<std::tuple<std::vector<int>,std::vector<bool>>,double,double>> solutions) {
    using namespace std;
    int M = solutions.size();
    sort(solutions.begin(),solutions.end(),sortbysec);
    tuple<tuple<vector<int>,vector<bool>>,double,double> worst, best, median;
    double mean, sd;

    best = solutions[M-1];
    worst = solutions[0];
    median = solutions[(M-1)/2];
    cout << "M = " << M << endl;
    cout << "1. Best solution: " << endl;
    printSolution(best); 
    cout << "2. Worst solution: " << endl;
    printSolution(worst);
    cout << "3. Median of solutions: " << endl;
    printSolution(median); 
    cout << "4. Mean of solutions: " << solutionsMean(solutions) << endl << endl;
    cout << "5. Standard deviation of solutions: " << solutionsSD(solutions) << endl << endl;
}

int main() {
    using namespace std;
    double initialTemp, finalTemp, c;
    int N;
    vector<double> p, w;
    vector<bool> sol, nSol;
    while(true) {
        int method = chooseMethod();
        if(method == 1) {
            string fileName = inputFile();

            tie(initialTemp, finalTemp, N, c, p, w) = readInput(fileName);

            tuple<tuple<vector<int>,vector<bool>>,double,double> solution;
            int nMethod = chooseNeighborMethod();

            solution = simulatedAnnealing(p,w,c,initialTemp,finalTemp,nMethod);
            printSolution(solution);

        } else if(method == 2) {

            vector<tuple<tuple<vector<int>,vector<bool>>,double,double>> solutions;
            int M;
            string file = inputFile();

            tie(initialTemp,finalTemp,N,c,p,w) = readInput(file);
            cout << "Choose a value of M\nM: ";
            cin >> M;

            int nMethod = chooseNeighborMethod();

            for(int i = 0; i < M; i++) {
                tuple<tuple<vector<int>,vector<bool>>,double,double> sol;
                sol = simulatedAnnealing(p,w,c,initialTemp,finalTemp,nMethod);
                solutions.push_back(sol);
            }

            generateReport(solutions);
        }
    
    }

    return 0;
}