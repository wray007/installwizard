#ifndef SRDTK_OBJECT_SMART_PTR_H_
#define SRDTK_OBJECT_SMART_PTR_H_

#pragma once

#ifdef _MSC_VER
#pragma pack(push, 8)
#endif

namespace srdtk {

template <class T>
class deallocator {
 public:
  static void destory(T* p) {
    delete p;
  }
};

template <class T, class D = deallocator<T> >
class smart_ptr {
 public:
  typedef T     value_type;
  typedef T&      reference_type;
  typedef T*      pointer_type;
  typedef const T&  const_reference;
  typedef T const*  const_pointer;
  typedef D     deallocator_type;

 public:
  smart_ptr() : p_(NULL), n_(NULL) {
  }

  explicit smart_ptr(T* p) : p_(NULL), n_(NULL) {
    if (p) {
      p_ = p;
      n_ = new int;
      *n_ = 1;
    }
  }

  smart_ptr(const smart_ptr<T, D>& ptr) : p_(NULL), n_(NULL) {
    if (NULL != ptr.n_) {
      p_ = ptr.p_;
      n_ = ptr.n_;
      (*n_)++;
    }
  }

  ~smart_ptr() {
    release();
  }

  smart_ptr<T,D>& operator=(const smart_ptr<T,D>& ptr) {
    if (*this == ptr) 
      return *this;
    if (ptr.p_ != p_) 
      release();
    if (NULL != ptr.n_) {
      p_ = ptr.p_;
      n_ = ptr.n_;
      (*n_)++;
    } else {
      p_ = NULL;
      n_ = NULL;
    }

    return *this;
  }

  smart_ptr<T,D>& operator=(T* p) {
    if (p_ != p) release();
    if (p) {
      p_ = p;
      n_ = new int;
      *n_ = 1;
    } else {
      p_ = NULL;
      n_ = NULL;
    }

    return *this;
  }

  T& operator*() const {
    return *p_;
  }

  T* operator->() const {
    return p_;
  }

  operator T* () const {
    return p_;
  }

  operator void* () const {
    return p_;
  }

  operator const void* () const {
    return p_;
  }

  T* ptr() {
    return p_;
  }

  bool operator==(const smart_ptr<T,D>& ptr) const {
    return (ptr.p_ == p_);
  }

  bool operator!=(const smart_ptr<T,D>& ptr) const {
    return (ptr.p_ != p_);
  }

  bool operator==(T* p) const {
    return (p_ == p);
  }

  bool operator!=(T* p) const {
    return (p_ != p);
  }

  void release() {
    if (n_ != NULL && --(*n_) == 0) {
      deallocator_type::destory(p_);
      p_ = NULL;
      delete n_;
      n_ = NULL;
    }
  }

 private:
  // contained pointer
  T* p_;

  // reference counter
  int* n_;
};


} // namespace srdtk


#ifdef _MSC_VER
#pragma pack(pop)
#endif


#endif /* SRDTK_OBJECT_SMART_PTR_H_ */
