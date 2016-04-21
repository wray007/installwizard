#ifndef SRDTK_OBJECT_AUTO_ARRAY_H_
#define SRDTK_OBJECT_AUTO_ARRAY_H_

#pragma once

#ifdef _MSC_VER
#pragma pack(push, SRDTK_PACKAGE)
#pragma warning(push, 3)
// C++ Exception Specification ignored
// A function was declared using exception specification.
// At this time the implementation details of exception
// specification have not been standardized,
// and are accepted but not implemented in Microsoft Visual C++.
#pragma warning(disable: 4290)
#endif

#include <vector>
#include <limits>

namespace srdtk {

template <class T>
class auto_array {
 public:
  typedef T     value_type;
  typedef T&      reference_type;
  typedef T*      poniter_type;
  typedef const T&  const_reference;
  typedef T const*  const_pointer;

 public:
  auto_array() : p_(NULL), size_(0) {
  }

  auto_array(size_t size) throw(std::bad_alloc) : p_(NULL), size_(size) {
    if (0 == size) 
      return;
    p_ = new T[size];
    if (NULL == p_) 
      throw std::bad_alloc();
  }

  explicit auto_array(T* p) {
    p_ = p;
  }

  ~auto_array() {
    if (NULL != p_) {
      delete [] p_;
      p_ = NULL;
      size_ = 0;
    }
  }

  T* get() {
    return p_;
  }

  operator T* () {
    return p_;
  }

  T& at(size_t pos) {
    return p_[pos];
  }

  const T& at(size_t pos) const {
    return p_[pos];
  }

  auto_array<T>& operator=(const auto_array<T>& array) {
    if (*this == array) return *this;
    if (array.size() == 0) {
      size_ = 0;
      if (NULL != p_) {
        delete [] p_;
        p_ = NULL;
      }
    } else {
      T* p = new T[array.size()];
      if (NULL == p) throw std::bad_alloc();

      size_t index = array.size();
      while (index-- > 0) {
        p[index] = array[index];
      }
      size_ = array.size();
      delete [] p_;
      p_ = p;
    }

    return *this;
  }

  size_t size() {
    return size_;
  }

  void resize(size_t new_size) {
    using namespace std;

    assert(new_size >= 0);
    if (new_size == size_) return;

    T* p = NULL;
    if (new_size > 0) {
      p = new T[new_size];
      if (NULL == p) throw std::bad_alloc();

      size_t n = min(size_, new_size);
      while (n-- > 0) {
        p[n] = p_[n];
      }
    }

    if (NULL != p_) delete [] p_;
    p_ = p;
    size_ = new_size;
  }

  void clear() {
    resize(0);
  }

 private:
  // contained pointer
  T* p_;

  // array size
  size_t size_;
};


} // namespace srdtk


#ifdef _MSC_VER
#pragma warning(default: 4290)
#pragma warning(pop)
#pragma pack(pop, SRDTK_PACKAGE)
#endif

#endif /* SRDTK_OBJECT_AUTO_ARRAY_H_ */
