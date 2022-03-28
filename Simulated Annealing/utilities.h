#ifndef UTILITIES_H
#define UTILITIES_H
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
template<typename T, typename Allocator>
void printVector(const std::vector<T, Allocator> &v);

#include "utilities.cpp"

#endif