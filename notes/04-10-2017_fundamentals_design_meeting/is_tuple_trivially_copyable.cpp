#include <tuple>
#include <type_traits>

static_assert(std::is_trivially_copyable<std::tuple<double, double>>::value, "");

