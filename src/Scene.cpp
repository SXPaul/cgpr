#include "Scene.h"

void Scene::addObject(const std::string &name, const Mesh &mesh,
                      const PBRMaterial &material) {
  SceneObject obj;
  obj.name = name;
  obj.mesh = mesh;
  obj.material = material;
  objects.push_back(std::move(obj));
}

void Scene::addLight(const Light &light) { lights.push_back(light); }

void Scene::removeSelected() {
  if (selectedObjectIndex >= 0 && selectedObjectIndex < (int)objects.size()) {
    objects[selectedObjectIndex].mesh.cleanup();
    objects.erase(objects.begin() + selectedObjectIndex);
    selectedObjectIndex = -1;
  }
}

void Scene::clear() {
  for (auto &obj : objects) {
    obj.mesh.cleanup();
  }
  objects.clear();
  lights.clear();
  selectedObjectIndex = -1;
}

SceneObject *Scene::getSelected() {
  if (selectedObjectIndex >= 0 && selectedObjectIndex < (int)objects.size()) {
    return &objects[selectedObjectIndex];
  }
  return nullptr;
}
