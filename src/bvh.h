
#pragma once

#include <vector>
#include "scene.h"
#include "SceneObject.h"

class BVH {
public:
  enum CONSTRUCTION_TYPE {
    CONSTRUCTION_OBJECT_MEDIAN,
    CONSTRUCTION_OBJECT_SAH,
  };

public:
  explicit BVH() : m_root(NULL), m_bvh_node_size(0) {}
  ~BVH();

  void Construct(const CONSTRUCTION_TYPE type, const std::vector<SceneObject *> &targets);
  bool CheckIntersection(const Ray &ray, Scene::IntersectionInformation &info) const;

  void CollectBoundingBoxes(int depth, std::vector<BoundingBox> &result);

private:
  void Construct_internal(const CONSTRUCTION_TYPE type, const std::vector<SceneObject *> &targets, int index);
  void MakeLeaf_internal(const std::vector<SceneObject *> &targets, int index);

  void CollectBoundingBoxes_internal(int currentDepth, int targetDepth, int index, std::vector<BoundingBox> &result);

private:
  class BVH_structure;
  BVH_structure *m_root;
  int m_bvh_node_size;
};
