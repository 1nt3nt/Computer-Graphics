#include "hitlist.hpp"
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;


Hit& HitList::getMin() {
    return hits[0];
}

void HitList::add(const Hit& hit) {
}
