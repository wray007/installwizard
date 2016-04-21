#pragma once

#ifndef SRDTK_PATTERN_WIN32_MYSUBCLASSWND_H_
#define SRDTK_PATTERN_WIN32_MYSUBCLASSWND_H_

#include "pattern/win32/Subclass.h"
#include "third_party/boost/boost/bind/bind.hpp>
#include "third_party/boost/boost/signals2/signal.hpp"

class MySubclassWnd : public CSubclassWnd {
 public:
  typedef boost::signals2::signal<LRESULT (UINT, WPARAM, LPARAM)> MessageHandler;

  MessageHandler OnMessage;

 private:
  virtual LRESULT WindowProc(UINT msg, WPARAM wp, LPARAM lp) {
    OnMessage(msg, wp, lp);

    return CSubclassWnd::WindowProc(msg, wp, lp);
  }
};

#endif // SRDTK_PATTERN_WIN32_MYSUBCLASSWND_H_