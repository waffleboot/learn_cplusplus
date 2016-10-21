
#include <iostream>
#include <memory>

using namespace std;

#define _move(name,from) do { cout << this << " move " #name " <- " << &from << endl; } while (0)
#define copy(name,from) do { cout << this << " create copy " #name " <- " << &from << endl; } while (0)
#define create(name) do { cout << this << " create " #name << endl; } while (0)
#define destroy(name) do { cout << this << " destroy " #name << endl; } while (0)
#define assign(name,from) do { cout << this << " assign " #name " <- " << &from << endl; } while (0)

struct A
{
  A() {
    create(A0);
  }
  A(const int x) : i {x} {
    create(Ax);
  }
  A(const A& that) : i {that.i} {
    copy(A,that);
  }
  A(A&& that) : A() {
    _move(A,that);
    swap(*this,that);
  }
  ~A() {
    destroy(A);
  }
  A& operator=(A tmp) {
    assign(A,tmp);
    swap(*this,tmp);
    return *this;
  }
  friend void swap(A& first, A& second) {
    cout << "swap A" << endl;
    first.i = second.i;
  }
  int i;
};

A operator+(const A& first, const A& second) {
  cout << "A+" << endl;
  return A(first.i + second.i);
}

struct D
{
  D() {
    create(D0);
  };
  D(const int i) : resource(i) {
    create(Di);
  }
  D(A r) : resource {std::move(r)} {
    create(Da);
  }
  D(const D& that) : resource(that.resource) {
    copy(D,that);
  }
  D(D&& that) : D() {
    _move(D,that);
    swap(*this,that);
  }
  ~D() {
    destroy(D);
  }
  D& operator=(D tmp) {
    assign(D,tmp);
    using std::swap;
    swap(*this,tmp);
    return *this;
  }
  friend void swap(D& first, D& second) {
    cout << "swap D " << &first << " <-> " << &second << endl;
    swap(first.resource, second.resource);
  }
  A resource;
};

D operator+(const D& f, const D& s) {
  cout << "D+" << endl;
  return D(f.resource+s.resource);
}

A foo() {
  cout << "--------------" << endl;
  return A(23);
}

int main()
{
  //D d(foo());
  A a;
  D d(a);
}
