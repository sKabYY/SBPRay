/*
 * Scene.h
 *
 *  Created on: 2012-5-13
 *      Author: gtxx3600
 */

#ifndef SCENE_H_
#define SCENE_H_

#include <list>
using namespace std;

class Object;
class Ray;
class Vec;
class Intersect;
class Material;
class Scene {
public:
  Scene();
  ~Scene();
  void CreateSphere(const Vec &p, double r, Material m);
  void CreatePlane(float a, float b, float c, float d, Material m);
  void CreateTriangle(const Vec &p1, const Vec &p2, const Vec &p3, Material m);
  void GetIntersect(const Ray &r, Intersect *out_ptr) const;

private:
  list<Object*> object_list;
};

#endif /* SCENE_H_ */
