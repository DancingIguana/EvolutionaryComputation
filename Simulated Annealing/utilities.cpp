#include <iostream>
#include <fstream>
#include <vector>
#include <string>

template<typename T, typename Allocator>
void printVector(const std::vector<T, Allocator> &v) {
    using namespace std;
    int n = v.size();
    cout << "[";
    for(int i = 0; i < n; i++) {
        cout << v[i];
        if(i == n - 1) continue;
        cout <<", ";
    }
    cout << "]";
}