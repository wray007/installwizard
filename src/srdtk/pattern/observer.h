/**
 * @file
 * @brief
 *    Implementing a Subject/Observer pattern with templates
 *    Using C++ Templates to overcome some of the original Subject/Observer
 *    design pattern problems
 * @attention
 *
 * @author
 *    Patje\n
 *    David Wong(davidwong@163.net)
 * @date
 *    2006/08/13
 * @version
 *    1.0
 * @since
 *
 * @par Copyright:
 *    Copyright (C)1999-2008, Smallred Software Studio, All Rights Reserved.
 * @see
 *    http://www.codeproject.com/cpp/observer_with_templates.asp
 * @cond 0
 * ----------------------------------------------------------------------------
 *                              _____     _____    _____
 *                             / ___/    / ___/   / ___/
 *                          __/ /       / /      / /
 * Copyright (C)1999-2008, /___/mallred Software Studio, All Rights Reserved.
 * ----------------------------------------------------------------------------
 * $Id: Observer.h,v 1.21 2007/09/14 09:45:53 davidwong Exp $
 * @endcond
 */
#ifndef SRDTK_PATTERN_OBSERVER_H_
#define SRDTK_PATTERN_OBSERVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "srdtk/object/smart_ptr.h"

#ifdef _MSC_VER
#pragma pack(push, SRDTK_PACKING)
#pragma warning(push, 3)
#endif  /* _MSC_VER */

/**
 * @addtogroup pattern
 * @{
 *
 */

namespace srdtk {

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// class Observer
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 * @brief
 *  A C++ template class can implement the generic observer interface when
 *  it wants to be informed of changes in observable objects.
 */
template <class T, typename P>
class Observer {
 public:
  typedef T observable_t; /**< @brief oberverable type that is to be attached */
  typedef P notify_t;   /**< @brief notify type of oberverable that is to be attached */
  typedef srdtk::smart_ptr<typename notify_t> notify_ptr;

  /**
   * @brief
   *  Destructor
   *
   */
  virtual ~Observer() {}

 public:
  /**
   * @brief
   *  This method is called whenever the observed object is changed.
   *
   * @param observable
   *  The pointer point to observable that was attached.
   *
   * @param notify
   *  The pointer point to notify data of observable.
   *
   * @return long
   *  The update result defined by customer.
   */
  virtual long update(observable_t* observable = NULL,
                      const notify_ptr& notify = srdtk::smart_ptr<void>(NULL)) = 0;
};

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// class Observable
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 * @brief
 *  This C++ template class represents an observable object,
 *  or "data" in the model-view paradigm.
 *  It can be reused to represent an object that the application wants to
 *  have observed.
 *
 */
template <class T, typename P>
class Observable {
 public:
  typedef T         observable_t; /**< @brief oberverable type */
  typedef P         notify_t;   /**< @brief notify type */
  typedef srdtk::smart_ptr<typename notify_t> notify_ptr;

  /**< @brief observer type that is to be attached */
  typedef Observer<T, P> observer_t;   

 public:
  /**
   * @brief
   *  Constructor
   */
  Observable() {
    changed_ = false;
  }

//protected:
  /**
   * @brief
   *  Destructor
   */
  virtual ~Observable() {
    removeAllObservers();
  }

 public:
  /**
   * @brief
   *  Adds an observer to the set of observers for this object,
   *  provided that it is not the same as some observer already in the set.
   *
   * @param observer
   *  The pointer point to an observer to be added
   *
   * @return size_t
   *  Returns the number of observers of this Observable object.
   */
  size_t addObserver(observer_t* observer) {
    std::vector<observer_t* >::iterator itor;

    bool exist = false;
    for (itor = observers_.begin(); itor != observers_.end(); itor++) {
      if (*itor == observer) {
        exist = true;
        break;
      }
    }

    if (!exist)
      observers_.push_back(observer);

    return observers_.size();
  }

  /**
   * @brief
   *  Gets an observer by index.
   *
   * @param index
   *  index
   *
   * @return observer_t*
   *  The pointer point to obervser.
   */
  observer_t* getObserver(size_t index) const {
    if (index < 0 || index >= observers_.size()) {
      ASSERT(false);
      return 0;
    }

    return observers_.at(index);
  }

  /**
   * @brief
   *  Deletes an observer from the set of observers of this object.
   *
   * @param observer
   *  The pointer point to an observer to be deleted
   */
  void removeObserver(observer_t* observer) {
    std::vector<observer_t* >::iterator itor;

    for (itor = observers_.begin(); itor != observers_.end(); itor++) {
      if (*itor == observer) {
        observers_.erase(itor);
        break;
      }
    }
  }

  /**
   * @brief
   *  Clears the observer list so that this object no longer has any observers.
   */
  void removeAllObservers() {
    observers_.clear();
  }

  /**
   * @brief
   *  Returns the number of observers of this Observable object.
   *
   * @return size_t
   *  the number of observers of this Observable object
   */
  size_t countObservers() const {
    return observers_.size();
  }

  /**
   * @brief
   *  If this object has changed, as indicated by the hasChanged method,
   *  then notify special observer.
   *
   * @param observer
   * @param object
   */
  void notifyObserver(observer_t* observer,
                      const notify_ptr& notify = srdtk::smart_ptr<void>(NULL)) {
    observer ? observer->update(static_cast<T* >(this), notify) : ((void)0);
  }

  /**
   * @brief
   *  If this object has changed, as indicated by the hasChanged method,
   *  then notify special observer.
   *
   * @param index
   * @param object
   */
  void notifyObserver(size_t index,
                      const notify_ptr& notify = srdtk::smart_ptr<void>(NULL)) {
    if (index < 0 || index >= observers_.size()) {
      ASSERT(false);
      return;
    }

    observers_.at(index)->update(static_cast<T* >(this), notify);
  }

  /**
   * @brief
   *  If this object has changed, as indicated by the hasChanged method,
   *  then notify all of its observers and then call the setChanged() method to indicate
   *  that this object has no longer changed.
   *
   * @param object
   */
  void notifyAllObservers(const notify_ptr& notify = dtk::object::smart_ptr<void>
                          (NULL)) {
    std::vector<observer_t* >::iterator itor;

    for (itor = observers_.begin(); itor != observers_.end(); itor++) {
      (*itor)->update(static_cast<T* >(this), notify);
      if (observers_.size() <= 0)
        break;
    }
  }

  /**
   * @brief
   *  Marks this Observable object as having been changed.
   *  the IsChanged method will now return true.
   *
   */
  void setChanged() {
    changed_ = true;
  }

  /**
   * @brief
   *  Tests if this object has changed.
   *
   * @return bool
   *  The method returns true if this object has changed.
   *
   */
  bool hasChanged() const {
    return changed_;
  }

  /**
   * @brief
   *  Indicates that this object has no longer changed, or that it has already notified
   *  all of its observers of its most recent change,
   *  so that the hasChanged method will now return false.
   */
  void clearChanged() {
    changed_ = false;
  }

 private:
  std::vector<observer_t* > observers_;   /**< @brief */
  bool volatile         changed_;   /**< @brief */
};

} // namespace srdtk


#ifdef _MSC_VER
#pragma warning(pop)
#pragma pack(pop, SRDTK_PACKING)
#endif /* _MSC_VER */

#endif //SRDTK_PATTERN_OBSERVER_H_
