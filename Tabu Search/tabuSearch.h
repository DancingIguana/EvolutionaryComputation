#ifndef TABUSEARCH_H
#define TABUSEARCH_H
#include <bits/stdc++.h>
#include <ctime>
#include <fstream>
std::vector<std::vector<int>> defineGraph(int n);
std::tuple<std::vector<int>, int> tabuSearch(std::vector<std::vector<int>> graph, int n, int maxIter, int tabuTime);
std::tuple<int,int,std::vector<std::vector<int>>> readInput(std::string file);
void printGraph(std::vector<std::vector<int>> graph, int n);
void printVector(std::vector<int> v);
#endif