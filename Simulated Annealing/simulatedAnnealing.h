#ifndef SIMULATEDANNEALING_H
#define SIMULATEDANNEALING_H

std::tuple<double,double, int, double, std::vector<double>, std::vector<double>> readInput(std::string file);
std::tuple<std::tuple<std::vector<int>,std::vector<bool>>, double, double> simulatedAnnealing(std::vector<double> p, std::vector<double> w, double c, double temp, double fTemp, bool improvedNeighbor);
#include "simulatedAnnealing.cpp"

#endif