#include "reference_info.hpp"

#include <sstream>

using namespace graph;
using namespace std;

ReferenceInfo::ReferenceInfo() { clear(); }

ReferenceInfo::ReferenceInfo(int referenceKind, long referrerTag,
                             long referrerClassTag, bool writeToGraph) {
    clear();
    this->referenceKind = referenceKind;
    this->referrerTag = referrerTag;
    this->referrerClassTag = referrerClassTag;
    this->writeToGraph = writeToGraph;
}

ReferenceInfo::~ReferenceInfo() { clear(); }

void ReferenceInfo::clear() {
    referrerTag = 0;
    referrerClassTag = 0;
    referrerRelation = "";
    writeToGraph = false;
    fieldIndex = -1;
}

string ReferenceInfo::toString() const {
    if (!referrerRelation.empty()) {
        stringstream sstream;
        sstream << referrerTag << "," << referenceKind << ","
                << referrerRelation;
        return sstream.str();
    } else {
        return "";
    }
}
