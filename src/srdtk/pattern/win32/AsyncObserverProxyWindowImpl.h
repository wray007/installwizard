#ifndef SRDTK_PATTERN_ASYNC_OBSERVER_PROXY_WINDOW_IMPL_
#define SRDTK_PATTERN_ASYNC_OBSERVER_PROXY_WINDOW_IMPL_

#pragma once

#include "pattern/observer.h"
#include "pattern/win32/MySubclassWnd.h"

#ifdef WIN32
#include "pattern/win32/Subclass.h"
#endif //def WIN32

namespace srdtk {

template <class Observable, class Notify>
class AsyncObserverProxyWindowImpl : public Observer<Observable, Notify> {
 public:
  typedef typename AsyncObserverProxyWindowImpl<Observable, Notify> super_t;
  typedef typename Observer<Observable, Notify> observer_t;
  typedef typename Observable observable_t;
  typedef typename Notify notify_t;


#if defined(WIN32)
  AsyncObserverProxyWindowImpl(HWND hwnd,
                               const tstring& msgId,
                               observer_t* observer)
    : m_hwnd(hwnd), m_observer(observer), m_counter(0) {
    m_hasWindow = (m_hwnd != NULL);
    if (!m_hasWindow) {
      WNDCLASS wc;

      wc.cbClsExtra = 0;
      wc.cbWndExtra = 0;
      wc.hInstance = 0;
      wc.hIcon = 0;
      wc.hCursor = 0;
      wc.lpszMenuName = NULL;
      wc.lpszClassName = _T("___AsyncObserverProxyWindowImpl_window____");
      wc.style = 0;
      wc.lpfnWndProc = DefWindowProc;
      wc.hbrBackground = 0;

      ::RegisterClass(&wc);

      m_hwnd = ::CreateWindowEx(0, 
                                _T("__AsyncObserverProxyWindowImpl_window__"),
                                _T(""), WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, 
                                CW_USEDEFAULT, CW_USEDEFAULT, 
                                NULL, NULL, NULL, 0);
    }

    ASSERT(::IsWindow(m_hwnd));

    m_msgId = ::RegisterWindowMessage(msgId.c_str());

    m_msgHook.HookWindow(m_hwnd);
    m_msgHook.OnMessage.connect(boost::bind(
      &AsyncObserverProxyWindowImpl<Observable, Notify>::messageHandler, 
        this, _1, _2, _3));
  }

  virtual ~AsyncObserverProxyWindowImpl() {
    m_msgHook.HookWindow((HWND)NULL);
    if (!m_hasWindow) {
      ::DestroyWindow(m_hwnd);
      m_hwnd = NULL;
    }
  }
#endif //ifdef WIN32

  virtual long update(observable_t* observable,
                      const notify_ptr& notify = dtk::object::smart_ptr<void>) {
#if defined(WIN32)
    ASSERT(m_hwnd != NULL);

    if (::GetCurrentThreadId() == ::GetWindowThreadProcessId(m_hwnd, NULL))
      return m_observer->update(observable, notify);
#endif //ifdef WIN32

    ::InterlockedIncrement(&m_counter);

    send_data* data = new send_data;

    data->observer      = m_observer;
    data->observable    = observable;
    data->notify    = notify;
    data->owner     = this;

#if defined(WIN32)
    BOOL ok;

    // 如果消息队列满，强制处理
    // windows最大消息队列为10000，可以修改注册表修改该值
    // HKEY_LOCAL_MACHINE\SOFTWARE \Microsoft \Windows NT \CurrentVersion \Windows\USERPostMessageLimit
    do {
      ok = ::PostMessage(m_hwnd, m_msgId, reinterpret_cast<WPARAM>(data), NULL);
      ::Sleep(0);
      /*
      if (!ok) {
        MSG msg;
        while (::PeekMessage(&msg, m_hwnd, 0, 0, PM_REMOVE)) {
          ::TranslateMessage(&msg);
          ::DispatchMessage(&msg);
        }
      }
      */
    } while (!ok);

    ::Sleep(0);
#endif //ifdef WIN32

    return 1;
  }

  bool empty() const {
    return (m_counter == 0);
  };

 private:
  struct send_data {
    send_data() : observable(NULL), observer(NULL), notify(NULL), owner(NULL) {}

    observable_t*   observable;
    observer_t*     observer;
    notify_ptr      notify;
    super_t*    owner;
  };

 private:
  observer_t* m_observer;
  long        m_counter;
  bool        m_hasWindow;

#if defined(WIN32)
 private:
  LRESULT messageHandler(UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == m_msgId) {
      send_data* data = reinterpret_cast<send_data* >(wp);
      ASSERT(data != NULL);

      data->observer->update(data->observable, data->notify);

      ::InterlockedDecrement(&data->owner->m_counter);

      delete data;
    }

    return 0;
  }

 private:
  MySubclassWnd m_msgHook;
  HWND  m_hwnd;
  unsigned int m_msgId;
#endif //def WIN32
};

} // namespace srdtk


#endif // SRDTK_PATTERN_ASYNC_OBSERVER_PROXY_WINDOW_IMPL_
