#pragma once

#include "src/resources/Resource.h"

#include <filesystem>

namespace Froth {

struct ResourceMetadata {
  ResourceType Type = ResourceType::None;
  std::filesystem::path FilePath;
  size_t FileHash;
  std::filesystem::file_time_type LastWriteTime;

  operator bool() const { return Type != ResourceType::None; }
};

} // namespace Froth
