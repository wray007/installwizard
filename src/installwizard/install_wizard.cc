#include "installwizard/install_wizard.h"
#include <Shlobj.h>
#include <process.h>
#include "third_party/nsis/pluginapi.h"
//#include "third_party/nsis/api.h"
#include "srdtk/string/string_macros.h"
#include "third_party/zlib/google/zip.h"

#define UM_PROGRESS WM_USER + 1

extern extra_parameters* g_plugin_parms;

namespace {

void UnzipCallback(int total, int current) {
  InstallWizardFrame* iwf = InstallWizardFrame::GetWizard();
  if (iwf != NULL) {
    ::PostMessage(iwf->GetHWND(), UM_PROGRESS, total, current);
  }
}

} // namespace

InstallWizardFrame* InstallWizardFrame::instance_ = NULL;

InstallWizardFrame::InstallWizardFrame()
  : skin_path_(_T("")),
    layout_(_T("")) {
  instance_ = this;
}

InstallWizardFrame::~InstallWizardFrame() {
}

void InstallWizardFrame::Init() {
}

void InstallWizardFrame::BindCallback(const TCHAR* control_name,
                                      int callback_id) {
  try {
    callbacks_[control_name] = callback_id;
  } catch (...) {
  }
}

void InstallWizardFrame::SetControlAttribute(const TCHAR* name,
                                             const TCHAR* attr,
                                             const TCHAR* value) {
  ui::CControlUI* control = m_pm.FindControl(name);
  if (control != NULL) {
    control->SetAttribute(attr, value);
  }
}

void InstallWizardFrame::GetControlAttribute(const TCHAR* name,
                                             const TCHAR* attr,
                                             std::string& value) {
  ui::CControlUI* control = m_pm.FindControl(name);
  if (control == NULL)
    return;

  srdtk::tstring result = _T("");
  if (_tcscmp(attr, _T("text")) == 0) {
    result = control->GetText();
  }
 
  t2a(value, result);
}

bool InstallWizardFrame::IsChecked(const TCHAR* control_name) {
  ui::CControlUI* control = m_pm.FindControl(control_name);
  ui::COptionUI* opt = dynamic_cast<ui::COptionUI*>(control);
  if (opt != NULL)
    return opt->IsSelected();
  ui::CCheckBoxUI* chk = dynamic_cast<ui::CCheckBoxUI*>(control);
  if (chk != NULL)
    return chk->GetCheck();
  return false;
}

bool InstallWizardFrame::ShowWizardStep(const TCHAR* control_name, int index) {
  ui::CTabLayoutUI* tab = static_cast<ui::CTabLayoutUI*>(
      m_pm.FindControl(control_name));
  if (tab != NULL && (index >0 && index < tab->GetCount())) {
    tab->SelectItem(index);
    return true;
  }
  return false;
}

void InstallWizardFrame::SetProgressRange(int low, int high) {
  if (progress_name_.IsEmpty())
    return;
  ui::CProgressUI* p = static_cast<ui::CProgressUI*>(
      m_pm.FindControl(progress_name_));
  if (p != NULL) {
    p->SetMinValue(low);
    p->SetMaxValue(high);
  }
}

void InstallWizardFrame::SetProgressValue(int value) {
  if (progress_name_.IsEmpty())
    return;
  ui::CProgressUI* p = static_cast<ui::CProgressUI*>(
      m_pm.FindControl(progress_name_));
  if (p != NULL) {
    p->SetValue(value);
  }
}

void InstallWizardFrame::SetInstallPath(const TCHAR* control_name, 
                                        const TCHAR* path) {
  install_path_ = path;
  ui::CControlUI* control = m_pm.FindControl(control_name);
  if (control != NULL && !install_path_.IsEmpty()) {
    control->SetText(install_path_);
  }
}

void InstallWizardFrame::SelectInstallDir(const TCHAR* control_name,
                                          std::string& install_dir) {
  install_dir.clear();

  BROWSEINFO bi;
  memset(&bi, 0, sizeof(BROWSEINFO));
  bi.hwndOwner = GetHWND();
  bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
  bi.lpszTitle = _T("选择文件夹");
  bi.lParam = (LPARAM)(install_path_.GetData());
  LPITEMIDLIST idl = SHBrowseForFolder(&bi);
  if (!idl)
    return;

  TCHAR dir[MAX_PATH] = {0};
  SHGetPathFromIDList(idl, dir);
  t2a(install_dir, srdtk::tstring(dir));
  install_path_ = dir;

  ui::CControlUI* control = m_pm.FindControl(control_name);
  if (control != NULL && !install_path_.IsEmpty()) {
    control->SetText(install_path_);
  }
}

void InstallWizardFrame::StartInstall(const TCHAR* install_dir,
                                      const TCHAR* app) {
  if (install_dir == NULL || app == NULL)
    return;
  install_path_ = install_dir;
  install_zip_ = app;
  install_thread_ = (HANDLE)_beginthreadex(NULL, 0, 
     InstallWizardFrame::InstallThread,
     reinterpret_cast<void*>(this),
     0, &install_thread_id_);
}

unsigned int InstallWizardFrame::Install() {
  zip::UnzipWithCallback(install_zip_.GetData(), 
                         install_path_.GetData(), 
                         UnzipCallback);
  return 0;
}



InstallWizardFrame* InstallWizardFrame::GetWizard() {
  return instance_;
}

// ui::INotifyUI methods
void InstallWizardFrame::Notify(ui::TNotifyUI& msg) {
  if (msg.sType == _T("windowinit")) {
    OnInit(msg);
  } else if (msg.sType == _T("click") || (msg.sType == _T("textchanged"))) {
    ui::CDuiString control_name = msg.pSender->GetName();
    StringIntMap::const_iterator itor = callbacks_.find(control_name);
    if (itor != callbacks_.end()) {
      if (g_plugin_parms)
        g_plugin_parms->ExecuteCodeSegment(itor->second - 1, 0);
    }
  }
}

// ui::CWindowWnd methods
LPCTSTR InstallWizardFrame::GetWindowClassName() const {
  return _T("DuiInstallerFrame");
}

UINT InstallWizardFrame::GetClassStyle() const {
  return CS_DBLCLKS;
}

void InstallWizardFrame::OnFinalMessage(HWND hWnd) {
  m_pm.RemoveNotifier(this);
  delete this;
}

LRESULT InstallWizardFrame::HandleMessage(UINT uMsg,
                                          WPARAM wParam,
                                          LPARAM lParam) {
  LRESULT lr = S_OK;
  BOOL bHandled = TRUE;

  switch (uMsg) {
  case WM_CREATE:
    lr = OnCreate(uMsg, wParam, lParam, bHandled);
    break;
  case WM_CLOSE:
    lr = OnClose(uMsg, wParam, lParam, bHandled);
    break;
  case WM_DESTROY:
    lr = OnDestroy(uMsg, wParam, lParam, bHandled);
    break;
  case WM_NCACTIVATE:
    lr = OnNcActivate(uMsg, wParam, lParam, bHandled);
    break;
  case WM_NCCALCSIZE:
    lr = OnNcCalcSize(uMsg, wParam, lParam, bHandled);
    break;
  case WM_NCPAINT:
    lr = OnNcPaint(uMsg, wParam, lParam, bHandled);
    break;
  case WM_NCHITTEST:
    lr = OnNcHitTest(uMsg, wParam, lParam, bHandled);
    break;
  case WM_SIZE:
    lr = OnSize(uMsg, wParam, lParam, bHandled);
    break;
  case WM_GETMINMAXINFO:
    lr = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled);
    break;
  case WM_SYSCOMMAND:
    lr = OnSysCommand(uMsg, wParam, lParam, bHandled);
    break;
  case UM_PROGRESS:
    lr = OnProgress(uMsg, wParam, lParam, bHandled);
    break;
  default:
    bHandled = FALSE;
    break;
  }

  if (bHandled) return lr;
  if (m_pm.MessageHandler(uMsg, wParam, lParam, lr))
    return lr;
  return ui::CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

// 事件处理
void InstallWizardFrame::OnInit(ui::TNotifyUI& msg) {
  //ui::CControlUI* control = m_pm.FindControl(_T("txtpath"));
  //if (control != NULL && !install_path_.IsEmpty()) {
  //  control->SetText(install_path_);
  //}
}

LRESULT InstallWizardFrame::OnCreate(UINT uMsg,
                                     WPARAM wParam,
                                     LPARAM lParam,
                                     BOOL& bHandled) {
  LONG dwStyle = ::GetWindowLong(GetHWND(), GWL_STYLE);
  dwStyle &= ~WS_CAPTION;
  //dwStyle |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
  ::SetWindowLong(GetHWND(), GWL_STYLE, dwStyle);

  m_pm.Init(m_hWnd);
  ui::CDialogBuilder builder;
  ui::CControlUI* pRoot = builder.Create(layout().GetData(),
                                         (UINT)0,  NULL, &m_pm);
  ASSERT(pRoot && _T("Failed to parse XML"));
  m_pm.AttachDialog(pRoot);
  m_pm.AddNotifier(this);

  Init();

  return S_OK;
}

LRESULT InstallWizardFrame::OnClose(UINT uMsg,
                                    WPARAM wParam,
                                    LPARAM lParam,
                                    BOOL& bHandled) {
  bHandled = FALSE;
  return 0;
}

LRESULT InstallWizardFrame::OnDestroy(UINT uMsg,
                                      WPARAM wParam,
                                      LPARAM lParam,
                                      BOOL& bHandled) {
  ::PostQuitMessage(0L);

  bHandled = FALSE;
  return 0;
}

LRESULT InstallWizardFrame::OnNcActivate(UINT uMsg,
                                         WPARAM wParam,
                                         LPARAM lParam,
                                         BOOL& bHandled) {
  if (::IsIconic(*this))
    bHandled = FALSE;
  return (wParam == 0) ? TRUE : FALSE;
}

LRESULT InstallWizardFrame::OnNcCalcSize(UINT uMsg,
                                         WPARAM wParam,
                                         LPARAM lParam,
                                         BOOL& bHandled) {
  return 0;
}

LRESULT InstallWizardFrame::OnNcPaint(UINT uMsg,
                                      WPARAM wParam,
                                      LPARAM lParam,
                                      BOOL& bHandled) {
  return 0;
}

LRESULT InstallWizardFrame::OnNcHitTest(UINT uMsg,
                                        WPARAM wParam,
                                        LPARAM lParam,
                                        BOOL& bHandled) {
  POINT pt;
  pt.x = LOWORD(lParam);
  pt.y = HIWORD(lParam);
  ::ScreenToClient(*this, &pt);

  RECT rcClient;
  ::GetClientRect(*this, &rcClient);

  if (!::IsZoomed(*this)) {
    RECT rcSizeBox = m_pm.GetSizeBox();
    if (pt.y < (rcClient.top + rcSizeBox.top)) {
      if (pt.x < (rcClient.left + rcSizeBox.left))
        return HTTOPLEFT;
      if (pt.x > (rcClient.right - rcSizeBox.right))
        return HTTOPRIGHT;
      return HTTOP;
    } else if (pt.y > (rcClient.bottom - rcSizeBox.bottom)) {
      if (pt.x < (rcClient.left + rcSizeBox.left))
        return HTBOTTOMLEFT;
      if (pt.x > (rcClient.right - rcSizeBox.right))
        return HTBOTTOMRIGHT;
      return HTBOTTOM;
    }
    if (pt.x < (rcClient.left + rcSizeBox.left))
      return HTLEFT;
    if (pt.x > (rcClient.right - rcSizeBox.right))
      return HTRIGHT;
  }

  RECT rcCaption = m_pm.GetCaptionRect();
  if ((pt.x >= rcClient.left + rcCaption.left)
      && (pt.x < rcClient.right - rcCaption.right)
      && (pt.y >= rcCaption.top && pt.y < rcCaption.bottom)) {
    ui::CControlUI* pControl = NULL;
    pControl = static_cast<ui::CControlUI*>(m_pm.FindControl(pt));
    if (pControl
        && (_tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0)
        && (_tcscmp(pControl->GetClass(), _T("OptionUI")) != 0)
        && (_tcscmp(pControl->GetClass(), _T("TextUI")) != 0)) {
      return HTCAPTION;
    }
  }

  return HTCLIENT;
}

LRESULT InstallWizardFrame::OnSize(UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam,
                                   BOOL& bHandled) {
  SIZE corner = m_pm.GetRoundCorner();
  if (!::IsIconic(*this) && (corner.cx != 0 || corner.cy != 0)) {
    ui::CDuiRect rcWnd;
    ::GetWindowRect(*this, &rcWnd);
    rcWnd.Offset(-rcWnd.left, -rcWnd.top);
    rcWnd.right++;
    rcWnd.bottom++;
    HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left,
                                     rcWnd.top,
                                     rcWnd.right,
                                     rcWnd.bottom,
                                     corner.cx,
                                     corner.cy);
    ::SetWindowRgn(*this, hRgn, TRUE);
    ::DeleteObject(hRgn);
  }

  bHandled = FALSE;
  return 0;
}

LRESULT InstallWizardFrame::OnGetMinMaxInfo(UINT uMsg,
                                            WPARAM wParam,
                                            LPARAM lParam,
                                            BOOL& bHandled) {
  MONITORINFO mi = {};
  mi.cbSize = sizeof(mi);
  ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &mi);
  ui::CDuiRect rcWork = mi.rcWork;
  rcWork.Offset(-mi.rcMonitor.left, -mi.rcMonitor.top);

  LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
  lpMMI->ptMaxPosition.x  = rcWork.left;
  lpMMI->ptMaxPosition.y  = rcWork.top;
  lpMMI->ptMaxSize.x    = rcWork.right;
  lpMMI->ptMaxSize.y    = rcWork.bottom;

  bHandled = FALSE;
  return 0;
}

LRESULT InstallWizardFrame::OnSysCommand(UINT uMsg,
                                         WPARAM wParam,
                                         LPARAM lParam,
                                         BOOL& bHandled) {
  // 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
  if (wParam == SC_CLOSE) {
    ::PostQuitMessage(0L);
    bHandled = TRUE;
    return 0;
  }

  BOOL bZoomed = ::IsZoomed(this->GetHWND());
  if ((wParam & 0xFFF0) == SC_MAXIMIZE || (wParam & 0xFFF0) == SC_SIZE) {
    bHandled = TRUE;
    return 0;
  }

  LRESULT lr = ui::CWindowWnd::HandleMessage(uMsg, wParam, lParam);
  return lr;
}

LRESULT InstallWizardFrame::OnProgress(UINT uMsg,
                                       WPARAM wParam,
                                       LPARAM lParam,
                                       BOOL& bHandled) {
  bHandled = TRUE;
  StringIntMap::const_iterator itor = callbacks_.find(_T("installcallback"));
  if (itor != callbacks_.end()) {
    if (g_plugin_parms) {
      int p1 = static_cast<int>(wParam);
      int p2 = static_cast<int>(lParam);
      pushint(p1);
      pushint(p2);
      g_plugin_parms->ExecuteCodeSegment(itor->second - 1, 0);

      //postinstall callback 
      if (p1 == p2) {
        StringIntMap::const_iterator it = callbacks_.find(_T("postinstall"));
        if (it != callbacks_.end()) {
          g_plugin_parms->ExecuteCodeSegment(it->second - 1, 0);
        }
      }
    }
  }
  return S_OK;
}

unsigned int __stdcall InstallWizardFrame::InstallThread(void* param) {
  InstallWizardFrame* self = reinterpret_cast<InstallWizardFrame*>(param);
  if (self == NULL)
    return -1;
  return self->Install();
}
