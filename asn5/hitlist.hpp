#if !defined(_HITLIST_H_)

#define _HITLIST_H_

#include <vector>

#include "hit.hpp"

class HitList {
public:
    Hit& getMin();
    void add(const Hit& hit);

    bool isEmpty() { return hits.size() == 0; };
    void clear()   { hits.clear(); };

private:
    std::vector<Hit> hits;
};

#endif
