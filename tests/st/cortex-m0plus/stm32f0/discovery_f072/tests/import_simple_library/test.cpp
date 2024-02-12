#include <cstdio>
#include <tuple>
#include <utility>

std::size_t get_lot_for(std::size_t pc)
{
  printf("Getting LOT for: %x\n", pc);
  return 0;
}

int call8_wrapped(int a, int b, int c, int d, int e, int f, int g, int h)
{
  printf("%d %d %d %d %d %d %d %d\n", a, b, c, d, e, f, g, h);
  return 10;
}

template <typename T, typename... Args>
inline T call_any(T (*fun)(Args...), Args &&...args)
{
  std::size_t pc;
  asm inline("push {r4, r5}\n\t");
  asm inline("mov r4, r9\n\t");
  asm inline("push {r4, r5}\n\t");
  asm inline("mov %[result], pc\n\t" : [result] "=r"(pc));
  asm inline("mov r4, %[value]\n\t" : : [value] "r"(get_lot_for(pc)));
  T r = fun(std::forward<Args>(args)...);
  asm inline("pop {r4, r5}\n\t");
  asm inline("mov r9, r4\n\t");
  asm inline("pop {r4, r5}\n\t");
  return r;
}

template <typename>
struct function_args
{
};

template <typename R, typename... Args>
struct function_args<R(Args...)>
{
  static constexpr std::size_t numberOfArgs = sizeof...(Args);
  using args                                = std::tuple<Args...>;
  using ReturnType                          = R;
};

auto __attribute__((noinline)) call8(auto... args)
{
  return call_any(&call8_wrapped, std::move(args)...);
}

extern "C"
{
  function_args<decltype(call8_wrapped)>::ReturnType call8c(
    std::tuple_element_t<0, function_args<decltype(call8_wrapped)>::args> a,
    std::tuple_element_t<1, function_args<decltype(call8_wrapped)>::args> b,
    std::tuple_element_t<2, function_args<decltype(call8_wrapped)>::args> c,
    std::tuple_element_t<3, function_args<decltype(call8_wrapped)>::args> d,
    std::tuple_element_t<4, function_args<decltype(call8_wrapped)>::args> e,
    std::tuple_element_t<5, function_args<decltype(call8_wrapped)>::args> f,
    std::tuple_element_t<6, function_args<decltype(call8_wrapped)>::args> g,
    std::tuple_element_t<7, function_args<decltype(call8_wrapped)>::args> h)
  {
    static_cast<void>(a);
    return call_any(
      &call8_wrapped,
      static_cast<int>(
        std::move(function_args<decltype(call8_wrapped)>::numberOfArgs)),
      std::move(b),
      std::move(c),
      std::move(d),
      std::move(e),
      std::move(f),
      std::move(g),
      std::move(h));
  }
}
