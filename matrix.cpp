
// g++ matrix.cpp -o matrix -std=gnu++14 -O2 -DX=1

#include <cstdlib>
#include <iostream>
#include <memory>
#include <new>

template<class C>
struct Aggregator {
  typedef C cell_t;
  void operator()(cell_t&,const cell_t&) const;
};

template<class C = int, class A = Aggregator<C> >
class Matrix {
public:
  typedef C cell_t;
  typedef A aggregator_t;
  typedef const std::size_t count_t;
public:
  template<class... Args>
  Matrix(count_t count, Args&&... args) : count(count) {
    for (auto i = 0; i < count; ++i) {
      new (&data[i]) cell_t (std::forward<Args>(args)...);
    }
    std::cout << "create" << std::endl;
  }
  ~Matrix() {
    for (auto i = 0; i < count; ++i) {
      data[i].~cell_t();
    }
    std::cout << "destroy" << std::endl;
  }
  static const auto bytes(count_t count) {
    //std::cout << sizeof(std::size_t) << std::endl;
    std::size_t x = sizeof(Matrix);
    std::size_t y = sizeof(cell_t);
    std::size_t z = x + count * y;
    //std::cout << x << ',' << y << ',' << z << std::endl;
    return z;
  }
public:
  const cell_t calculate(const cell_t& initial, const aggregator_t& a) const {
    auto r = initial;
    for (auto i = 0; i < count; ++i) {
      a(r,data[i]);
    }
    return r;
  }
public:
  struct unique_ptr_deleter {
    void operator()(Matrix<cell_t>* ptr) const {
      #if defined(__GNUC__) && defined(X)
      ptr->~Matrix();
      #else
      delete ptr;
      #endif
    }
  };
private:
  count_t count;
  cell_t data[0];
};

template<class T = int>
struct MyCell {
  typedef T value_t;
  MyCell(const value_t value) : ptr(std::make_unique<value_t>(value)) {}
  MyCell(const MyCell& other) : ptr(std::make_unique<value_t>(*other.ptr)) {}
  friend std::ostream& operator<<(std::ostream&,const MyCell<T>&);
  std::unique_ptr<value_t> ptr;
} /*__attribute__ ((aligned(16)))*/ ;
static const MyCell<> zero_int = {0};
static const MyCell<float> zero_float = {0.0};
std::ostream& operator<<(std::ostream& o, const MyCell<>& cell) {
  o << *cell.ptr; return o;
}
template<> void Aggregator<MyCell<>>::operator()(cell_t& dst, const cell_t& src) const {
  *dst.ptr += *src.ptr;
}

#if defined(__GNUC__) && defined(X)
#define MATRIX_PTR(COUNT,TYPE,DEFAULT) new (__builtin_alloca(TYPE::bytes(COUNT))) TYPE(COUNT,DEFAULT)
#else
#define MATRIX_PTR(COUNT,TYPE,DEFAULT) new (static_cast<TYPE*>(operator new (TYPE::bytes(COUNT)))) TYPE(COUNT,DEFAULT)
#endif

#define define_matrix(NAME,TYPE,COUNT,DEFAULT) const std::unique_ptr<TYPE,TYPE::unique_ptr_deleter> NAME(MATRIX_PTR((COUNT),TYPE,(DEFAULT)))

void foo(Matrix<MyCell<int>>::count_t count) {
  typedef Matrix<MyCell<int>> mat_t;
  define_matrix(m,mat_t,count,1);
  std::cout << m->calculate(zero_int, Aggregator<mat_t::cell_t>()) << std::endl;
}

int main(int argc, char *argv[]) {
  auto count = argc > 1 ? atoi(argv[1]) : 4;
  foo(count);
}
