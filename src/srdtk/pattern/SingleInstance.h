#ifndef SRDTK_PATTERN_SINGLE_INSTANCE_H_
#define SRDTK_PATTERN_SINGLE_INSTANCE_H_

#pragma once

// this code is from Q243953 in case you lose the article and wonder
// where this code came from...
class LimitSingleInstance {
 public:
  LimitSingleInstance(LPCTSTR strMutexName) {
    //be sure to use a name that is unique for this application otherwise
    //two apps may think they are the same if they are using same name for
    //3rd parm to CreateMutex
    m_hMutex = CreateMutex(NULL, FALSE, strMutexName); //do early
    m_dwLastError = GetLastError(); //save for use later...
  }

  ~LimitSingleInstance() {
    //don't forget to close handles...
    if (m_hMutex) {
      CloseHandle(m_hMutex); //do as late as possible
      m_hMutex = NULL; //good habit to be in
    }
  }

  bool IsAnotherInstanceRunning() const {
    return (ERROR_ALREADY_EXISTS == m_dwLastError);
  }

 private:
  DWORD  m_dwLastError;
  HANDLE m_hMutex;
};


#endif /* SRDTK_PATTERN_SINGLE_INSTANCE_H_ */