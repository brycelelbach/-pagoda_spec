#include <array>
#include <type_traits>

static_assert(std::is_trivially_copyable<std::array<double, 64>>::value, "");

