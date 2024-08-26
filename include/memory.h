#pragma once

#include <cstddef>
#include <cstring>
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

template <size_t N> class arena {
  static const size_t alignment = MAX(alignof(std::max_align_t), 16);

  alignas(alignment) char _buf[N];
  char *_ptr;

  bool ptr_in_buffer(char *p) { return _buf <= p && p <= _buf + N; }

public:
  arena() : _ptr(_buf) {}
  ~arena() { _ptr = nullptr; }

  char *alloc(size_t n) {
    if (_buf + N - _ptr >= n) {
      char *r = _ptr;
      _ptr += n;
      return r;
    }
    return static_cast<char *>(::operator new(n));
  }

  void dealloc(char *p, size_t n) {
    if (ptr_in_buffer(p)) {
      if (p + n == _ptr) {
        _ptr = p;
      }
    } else {
      ::operator delete(p);
    }
  }

  size_t used() const { return static_cast<size_t>(_ptr - _buf); }

  static arena &instance() {
    static arena *self = new arena{};
    return *self;
  }
};

class string {
  using pool = arena<1024>;
  size_t _size;
  size_t _cap;
  char *_m;

public:
  string(const char *str)
      : _size(strlen(str)), _cap(_size + 1), _m(pool::instance().alloc(_cap)) {
    strcpy(_m, str);
  }

  explicit string(const string &s)
      : _size(s.size()), _cap(_size + 1), _m(pool::instance().alloc(_cap)) {
    strcpy(_m, s.c_str());
  }

  size_t size() const { return _size; }

  void resize(size_t n) {
    if (n >= _cap) {
      string copy(*this);
      pool::instance().dealloc(_m, _size);
      _m = pool::instance().alloc(n + 1);
      strcpy(_m, copy._m);
      _cap = n;
    }
    _size = n;
    _m[_size] = '\0';
  }

  string &append(const string &other) {
    size_t n = other.size() + size();
    if (n >= _cap) {
      resize(n);
      strcat(_m, other._m);
    }
    return *this;
  }
  const char *c_str() const { return _m; }
};

template <typename T> class vector {
  using pool = arena<1024>;
  size_t _size;
  size_t _cap;
  T *_m;

public:
  vector(size_t sz) : _size(0), _cap(sz), _m(pool::instance().alooc(_cao)) {}
  ~vector() {}
  void push_back()
};