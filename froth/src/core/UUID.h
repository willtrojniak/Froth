#include <cstddef>
#include <cstdint>
#include <functional>

namespace Froth {

class UUID {
public:
  UUID();
  UUID(uint64_t uuid);
  UUID(const UUID &uuid) = default;

  operator uint64_t() const { return m_UUID; }

private:
  uint64_t m_UUID;
};

} // namespace Froth

namespace std {

template <>
struct hash<Froth::UUID> {
  std::size_t operator()(const Froth::UUID &uuid) const noexcept {
    return hash<uint64_t>()(static_cast<uint64_t>(uuid));
  }
};
} // namespace std
