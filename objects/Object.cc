/*
 * Object.cc
 *
 *  Created on: 2012-5-13
 *      Author: gtxx3600
 */

#include "objects/Object.h"

const float Object::kThreshold = 0.00001;

Object::Object(Material m)
    : material(m) {

}

Object::~Object() {

}

