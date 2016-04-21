#ifndef INSTALLER_DUI_INSTALLER_H_
#define INSTALLER_DUI_INSTALLER_H_

#include <map>
#include "third_party/DuiLib/UIlib.h"

class InstallWizardFrame : public ui::CWindowWnd, public ui::INotifyUI {
 public:
  InstallWizardFrame();
  virtual ~InstallWizardFrame();
  void Init();

  // 绑定NSIS脚本
  void BindCallback(const TCHAR* control_name, int callback_id);
  void SetControlAttribute(const TCHAR* name, 
                           const TCHAR* attr, 
                           const TCHAR* value);
  void GetControlAttribute(const TCHAR* name, 
                           const TCHAR* attr, 
                           std::string& value);
  bool IsChecked(const TCHAR* control_name);
  bool ShowWizardStep(const TCHAR* control_name, int index);

  void SetSkinPath(const TCHAR* skin_path) { skin_path_ = skin_path; }
  ui::CDuiString skin_path() const { return skin_path_; }
  void SetLayoutName(const TCHAR* layout) { layout_ = layout; }
  ui::CDuiString layout() const { return layout_; }
  void SetWizardTabName(const TCHAR* name) { wizard_tab_name_ = name; }
  ui::CDuiString wizard_tab_name() const { return wizard_tab_name_; }
  void SetInstallPath(const TCHAR* control_name, const TCHAR* path);
  ui::CDuiString install_path() const { return install_path_; }
  void SetProgressName(const TCHAR* name) { progress_name_ = name; }
  ui::CDuiString progress_name() const { return progress_name_; }
  void SetProgressRange(int low, int high);
  void SetProgressValue(int value);
  void SelectInstallDir(const TCHAR* control_name, std::string& install_dir);
  void StartInstall(const TCHAR* install_dir, const TCHAR* app);
  unsigned int Install();

  static InstallWizardFrame* GetWizard();
  static unsigned int __stdcall InstallThread(void* param);

  // ui::INotifyUI methods
  virtual void Notify(ui::TNotifyUI& msg);

  // ui::CWindowWnd methods
  virtual void OnFinalMessage(HWND hWnd);
  virtual LPCTSTR GetWindowClassName() const;
  virtual UINT GetClassStyle() const;
  virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

  // 事件处理
  void OnInit(ui::TNotifyUI& msg);
  LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnProgress(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

 private:
  ui::CPaintManagerUI m_pm;
  ui::CDuiString skin_path_;
  ui::CDuiString layout_;
  ui::CDuiString wizard_tab_name_;
  ui::CDuiString install_path_;
  ui::CDuiString progress_name_;
  ui::CDuiString install_zip_;

  typedef std::map<ui::CDuiString, int> StringIntMap;
  StringIntMap callbacks_;

  // 单实例
  static InstallWizardFrame* instance_;

  HANDLE install_thread_;
  unsigned int install_thread_id_;
};

#endif // INSTALLER_DUI_INSTALLER_
