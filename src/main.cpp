#include "camera.h"
#include "hittables/hittable_list.h"
#include "hittables/sphere.h"
#include "math/vec3.h"
#include "utils.h"
#include <memory>

int main() {

  hittable_list world;

  world.add(std::make_shared<sphere>(point3{0, 0, -1}, 0.5));
  world.add(make_shared<sphere>(point3{0, -100.5, -1}, 100));

  camera cam;

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 2560;

  cam.render(world);
}
