//#include "installwizard/install_wizard_api.h"
#include <Windows.h>
#include <map>
#include "third_party/nsis/pluginapi.h"
#include "installwizard/install_wizard.h"
#include "srdtk/string/string_macros.h"

// Globals
extern HINSTANCE g_hInstance;
HWND g_hwndParent;
extra_parameters* g_plugin_parms;
InstallWizardFrame* g_frame =  NULL;
BOOL g_message_loop_running = TRUE;

typedef std::map<HWND, WNDPROC> HwndProcMap;
HwndProcMap g_hwnd_proc;

static UINT_PTR PluginCallback(enum NSPIM msg) {
  return 0;
}


/**
 * ��ʼ����װҳ��
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void InitWizard(HWND hwndParent,
                                                 int string_size,
                                                 char* variables,
                                                 stack_t** stacktop,
                                                 extra_parameters* extra) {
  EXDLL_INIT();

  {
#ifdef _DEBUG
    MessageBox(NULL, _T("Debug..."), _T("Debug install wizard"), MB_OK);
#endif
    g_plugin_parms = extra;
    extra->RegisterPluginCallback(g_hInstance, PluginCallback);

    g_hwndParent = hwndParent;

    char p1[MAX_PATH] = {0};
    char p2[MAX_PATH] = {0};
    char p3[MAX_PATH] = {0};
    srdtk::tstring skin_path = _T("");
    srdtk::tstring layout_name = _T("");
    srdtk::tstring wizard_tab_name = _T("");

    popstring(p1);
    popstring(p2);
    popstring(p3);
    a2t(skin_path, std::string(p1));
    a2t(layout_name, std::string(p2));
    a2t(wizard_tab_name, std::string(p3));


    ui::CPaintManagerUI::SetInstance(g_hInstance);
    ui::CPaintManagerUI::SetResourcePath(skin_path.c_str());
    ui::CPaintManagerUI::SetResourceZip(_T("skin.zip"));

    g_frame = new InstallWizardFrame();
    if (g_frame == NULL)
      return;
    g_frame->SetSkinPath(skin_path.c_str());
    g_frame->SetLayoutName(layout_name.c_str());
    HWND hwnd = g_frame->Create(NULL, _T("InstallWizard"),
                                UI_WNDSTYLE_DIALOG,
                                WS_EX_STATICEDGE | WS_EX_APPWINDOW,
                                0, 0, 0, 0);
    if (hwnd == NULL)
      return;
    g_frame->CenterWindow();
    g_frame->ShowWindow(true, true);

    // push hwnd
    pushint(reinterpret_cast<int>(hwnd));
  }
}

/**
 * ���ô��ڱ���
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void SetWindowTitle(HWND hwndParent,
                                                     int string_size,
                                                     char* variables,
                                                     stack_t** stacktop,
                                                     extra_parameters* extra) {
  EXDLL_INIT();

  {
    g_hwndParent = hwndParent;

    char title[MAX_PATH] = {0};
    popstring(title);

    //InstallWizardFrame::GetWizard()->setwin
  }
}

/**
 * ���ô��ڴ�С
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void SetWindowSize(HWND hwndParent,
                                                    int string_size,
                                                    char* variables,
                                                    stack_t** stacktop,
                                                    extra_parameters* extra) {
  EXDLL_INIT();

  {
    g_hwndParent = hwndParent;

    int p1 = popint();
    int p2 = popint();
    int p3 = popint();

    HWND hwnd = reinterpret_cast<HWND>(p1);
    ASSERT(hwnd == g_frame->GetHWND());

    RECT bound;
    ::GetWindowRect(hwnd, &bound);
    BOOL result = MoveWindow(hwnd, bound.left, bound.top, p2, p3, TRUE);

    pushint(result);
  }
}

/**
 * ���ð�װ·��
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void SetInstallDir(HWND hwndParent,
                                                    int string_size,
                                                    char* variables,
                                                    stack_t** stacktop,
                                                    extra_parameters* extra) {
  EXDLL_INIT();

  {
    g_hwndParent = hwndParent;
    
    char p1[MAX_PATH] = {0};
    char p2[MAX_PATH] = {0};

    popstring(p1);
    popstring(p2);

    srdtk::tstring name;
    srdtk::tstring path;
    a2t(name, std::string(p1));
    a2t(path, std::string(p2));

    g_frame->SetInstallPath(name.c_str(), path.c_str());
  }
}

/**
 * ��ȡ��װ·��
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void GetInstallDir(HWND hwndParent,
                                                    int string_size,
                                                    char* variables,
                                                    stack_t** stacktop,
                                                    extra_parameters* extra) {
  EXDLL_INIT();

  {
    g_hwndParent = hwndParent;

    std::string value = "";
    srdtk::tstring path = g_frame->install_path();
    t2a(value, path);

    pushstring(value.c_str());
  }
}


/**
 * ��ʾ��װ��
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void ShowWizard(HWND hwndParent,
                                                 int string_size,
                                                 char* variables,
                                                 stack_t** stacktop,
                                                 extra_parameters* extra) {
  EXDLL_INIT();

  {
    g_hwndParent = hwndParent;
    
    HWND hwnd = g_frame->GetHWND();
    ::ShowWindow(hwnd, SW_SHOW);

    MSG msg = {0};
    while (::GetMessage(&msg, NULL, 0, 0) && g_message_loop_running) {
      ::TranslateMessage(&msg);
	  ::DispatchMessage(&msg);
    }
  }
}

/**
 * ��ʾ��װ�򵼲���
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void ShowWizardStep(HWND hwndParent,
                                                     int string_size,
                                                     char* variables,
                                                     stack_t** stacktop,
                                                     extra_parameters* extra) {
  EXDLL_INIT();

  {
    g_hwndParent = hwndParent;

    int index = 0;
    char p1[MAX_PATH] = {0};
    popstring(p1);
    index = popint();

    srdtk::tstring name;
    a2t(name, std::string(p1));

    g_frame->ShowWizardStep(name.c_str(), index);
  }
}

/**
 * �󶨻ص�
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void BindCallback(HWND hwndParent,
                                                   int string_size,
                                                   char* variables,
                                                   stack_t** stacktop,
                                                   extra_parameters* extra) {
  EXDLL_INIT();

  {
    g_hwndParent = hwndParent;

    char p1[MAX_PATH] = {0};
    popstring(p1);

    srdtk::tstring name;
    a2t(name, std::string(p1));

    g_frame->BindCallback(name.c_str(), popint());
  }
}

/**
 * ���ÿؼ�����
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void SetControlAttribute(HWND hwndParent,
                                                          int string_size,
                                                          char* variables,
                                                          stack_t** stacktop,
                                                          extra_parameters* extra) {
  EXDLL_INIT();

  {
    g_hwndParent = hwndParent;

    char p1[MAX_PATH] = {0};
    char p2[MAX_PATH] = {0};
    char p3[MAX_PATH] = {0};
    srdtk::tstring name = _T("");
    srdtk::tstring attr = _T("");
    srdtk::tstring value = _T("");

    popstring(p1);
    popstring(p2);
    popstring(p3);
    a2t(name, std::string(p1));
    a2t(attr, std::string(p2));
    a2t(value, std::string(p3));

    g_frame->SetControlAttribute(name.c_str(), attr.c_str(), value.c_str());
  }
}

/**
 * ���ÿؼ�����
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void GetControlAttribute(HWND hwndParent,
                                                          int string_size,
                                                          char* variables,
                                                          stack_t** stacktop,
                                                          extra_parameters* extra) {
  EXDLL_INIT();

  {
    g_hwndParent = hwndParent;

    char p1[MAX_PATH] = {0};
    char p2[MAX_PATH] = {0};
    srdtk::tstring name = _T("");
    srdtk::tstring attr = _T("");
    std::string value = "";

    popstring(p1);
    popstring(p2);
    a2t(name, std::string(p1));
    a2t(attr, std::string(p2));

    g_frame->GetControlAttribute(name.c_str(), attr.c_str(), value);
    pushstring(value.c_str());
  }
}

/**
 * ��ѡ���Ƿ�ѡ��
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void IsChecked(HWND hwndParent,
                                                 int string_size,
                                                 char* variables,
                                                 stack_t** stacktop,
                                                 extra_parameters* extra) {
  EXDLL_INIT();

  {
    g_hwndParent = hwndParent;

    char p1[MAX_PATH] = {0};
    popstring(p1);

    srdtk::tstring name = _T("");
    a2t(name, std::string(p1));

    bool flag = g_frame->IsChecked(name.c_str());
    pushint(flag ? 1 : 0);
  }
}

/**
 * ��ʾ��ʾ��
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void ShowMessageBox(HWND hwndParent,
                                                     int string_size,
                                                     char* variables,
                                                     stack_t** stacktop,
                                                     extra_parameters* extra) {
  EXDLL_INIT();
}

/**
 * �����
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void Maximize(HWND hwndParent,
                                               int string_size,
                                               char* variables,
                                               stack_t** stacktop,
                                               extra_parameters* extra) {
  EXDLL_INIT();

  {
    g_hwndParent = hwndParent;

    LRESULT lr = ::SendMessage(g_frame->GetHWND(), 
                               WM_SYSCOMMAND, SC_MAXIMIZE, 0);
    pushint(static_cast<int>(lr));
  }
}

/**
 * ��С����
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void Minimize(HWND hwndParent,
                                               int string_size,
                                               char* variables,
                                               stack_t** stacktop,
                                               extra_parameters* extra) {
  EXDLL_INIT();

  {
    g_hwndParent = hwndParent;

    LRESULT lr = ::SendMessage(g_frame->GetHWND(), 
                               WM_SYSCOMMAND, SC_MINIMIZE, 0);
    pushint(static_cast<int>(lr));
  }
}

/**
 * �ָ���
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void Restore(HWND hwndParent,
                                               int string_size,
                                               char* variables,
                                               stack_t** stacktop,
                                               extra_parameters* extra) {
  EXDLL_INIT();

  {
    g_hwndParent = hwndParent;

    LRESULT lr = ::SendMessage(g_frame->GetHWND(), 
                               WM_SYSCOMMAND, SC_RESTORE, 0);
    pushint(static_cast<int>(lr));
  }
}

/**
 * �ر���
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void CloseWizard(HWND hwndParent,
                                                  int string_size,
                                                  char* variables,
                                                  stack_t** stacktop,
                                                  extra_parameters* extra) {
  EXDLL_INIT();

  {
    g_hwndParent = hwndParent;

    BOOL result = ::PostMessage(g_frame->GetHWND(), WM_CLOSE, 0, 0);
    pushint(static_cast<int>(result));
  }
}

/**
 * ��ʼ��װ
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void StartInstall(HWND hwndParent,
                                                   int string_size,
                                                   char* variables,
                                                   stack_t** stacktop,
                                                   extra_parameters* extra) {
  EXDLL_INIT();

  {
    g_hwndParent = hwndParent;

    char p1[MAX_PATH] = {0};
    char p2[MAX_PATH] = {0};

    srdtk::tstring install_path = _T("");
    srdtk::tstring install_zip = _T("");
    
    popstring(p1);
    popstring(p2);
    a2t(install_path, std::string(p1));
    a2t(install_zip, std::string(p2));
    
    g_frame->StartInstall(install_path.c_str(), install_zip.c_str());
  }
}

/**
 * Ŀ¼ѡ��
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void SelectInstallDir(HWND hwndParent,
                                                       int string_size,
                                                       char* variables,
                                                       stack_t** stacktop,
                                                       extra_parameters* extra) {
  EXDLL_INIT();

  {
    g_hwndParent = hwndParent;

    char p1[MAX_PATH] = {0};
    srdtk::tstring name = _T("");
    std::string install_path = "";

    popstring(p1);
    a2t(name, std::string(p1));

    g_frame->SelectInstallDir(name.c_str(), install_path);
    pushstring(install_path.c_str());
  }
}

/**
 * ���ý���������
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void SetProgressName(HWND hwndParent,
                                                       int string_size,
                                                       char* variables,
                                                       stack_t** stacktop,
                                                       extra_parameters* extra) {
  EXDLL_INIT();

  {
    g_hwndParent = hwndParent;

    char p1[MAX_PATH] = {0};
    popstring(p1);

    srdtk::tstring name = _T("");
    a2t(name, std::string(p1));

    g_frame->SetProgressName(name.c_str());
  }
}

/**
 * ���ý�������Χ
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void SetProgressRange(HWND hwndParent,
                                                       int string_size,
                                                       char* variables,
                                                       stack_t** stacktop,
                                                       extra_parameters* extra) {
  EXDLL_INIT();

  {
    g_hwndParent = hwndParent;

    int p1 = popint();
    int p2 = popint();

    g_frame->SetProgressRange(p1, p2);
  }
}

/**
 * ���ý�������ǰֵ
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void SetProgressValue(HWND hwndParent,
                                                       int string_size,
                                                       char* variables,
                                                       stack_t** stacktop,
                                                       extra_parameters* extra) {
  EXDLL_INIT();

  {
    g_hwndParent = hwndParent;

    char p1[MAX_PATH] = {0};

    popstring(p1);
    int p2 = popint();

    g_frame->SetProgressValue(p2);
  }
}

/**
 * ����ٷֱ�
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void CalculatePercent(HWND hwndParent,
                                                       int string_size,
                                                       char* variables,
                                                       stack_t** stacktop,
                                                       extra_parameters* extra) {
  EXDLL_INIT();

  {
    g_hwndParent = hwndParent;

    int p1 = popint();
    int p2 = popint();
    int p3 = popint();

    int percent = (float)(p1 * 1.0) / p2 * p3; 
    pushint(percent);
  }
}

/**
 * ��ʽ���ַ���
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
extern "C" __declspec(dllexport) void FormatPercentString(HWND hwndParent,
                                                          int string_size,
                                                          char* variables,
                                                          stack_t** stacktop,
                                                          extra_parameters* extra) {
  EXDLL_INIT();

  {
    g_hwndParent = hwndParent;

    char p1[MAX_PATH] = {0};

    popstring(p1);
    int p2 = popint();

    srdtk::tstring format = _T("");
    a2t(format, std::string(p1));

    std::string var = "";
    t2a(var, srdtk::Format(format.c_str(), p2));
    pushstring(var.c_str());
  }
}
