template <char...> struct test {
  template <class Test> bool operator=(const Test &test) {
    test();
    return true;
  }
};

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wgnu-string-literal-operator-template"
#endif

template <class T, T... Chars> constexpr auto operator""_test() {
  return test<Chars...>{};
};
