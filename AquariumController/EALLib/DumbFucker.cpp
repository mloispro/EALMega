/*
* DumbFucker.cpp
*
* Created: 5/27/2016 1:02:32 PM
* Author: lenovo
*/


#include "DumbFucker.h"

// default constructor
DumbFucker::DumbFucker(int bb) {
    _bb = bb;
} //DumbFucker

// default destructor
DumbFucker::~DumbFucker() {
} //~DumbFucker

void DumbFucker::Begin() {
    _bb = 1;
}

void DumbFucker::Print(int f) {
    _bb++;
}

