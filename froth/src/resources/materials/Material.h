#pragma once

namespace Froth {
class Material {
public:
  Material() {};
  Material &operator=(Material &&o) {
    return *this;
  };

  void cleanup() {
  }

private:
};

} // namespace Froth
