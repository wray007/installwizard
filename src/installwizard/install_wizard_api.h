#ifndef INSTALLER_DUI_INSTALLER_API_H_
#define INSTALLER_DUI_INSTALLER_API_H_

#include <Windows.h>
#include "third_party/nsis/pluginapi.h"

#ifdef __cplusplus
#define INSTALLER_API extern "C" __declspec(dllexport)
#else
#define INSTALLER_API __declspec(dllexport)
#endif

///////////////////////////////////////////////////////////////////////////////
//
//  Installer api's
//

/**
 * 初始化安装页面
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
INSTALLER_API void APIENTRY InitWizard(HWND hwndParent,
                                       int string_size,
                                       char* variables,
                                       stack_t** stacktop,
                                       extra_parameters* extra);

/**
 * 设置窗口标题
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
INSTALLER_API void APIENTRY SetWindowTitle(HWND hwndParent,
                                           int string_size,
                                           char* variables,
                                           stack_t** stacktop,
                                           extra_parameters* extra);

/**
 * 设置安装路径
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
INSTALLER_API void APIENTRY SetInstallDir(HWND hwndParent,
                                          int string_size,
                                          char* variables,
                                          stack_t** stacktop,
                                          extra_parameters* extra);

/**
 * 获取安装路径
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
INSTALLER_API void APIENTRY GetInstallDir(HWND hwndParent,
                                          int string_size,
                                          char* variables,
                                          stack_t** stacktop,
                                          extra_parameters* extra);


/**
 * 显示安装向导
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
INSTALLER_API void APIENTRY ShowWizard(HWND hwndParent,
                                       int string_size,
                                       char* variables,
                                       stack_t** stacktop,
                                       extra_parameters* extra);

/**
 * 显示安装向导步骤
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
INSTALLER_API void APIENTRY ShowWizardStep(HWND hwndParent,
                                           int string_size,
                                           char* variables,
                                           stack_t** stacktop,
                                           extra_parameters* extra);

/**
 * 绑定回调
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
INSTALLER_API void APIENTRY BindCallback(HWND hwndParent,
                                         int string_size,
                                         char* variables,
                                         stack_t** stacktop,
                                         extra_parameters* extra);

/**
 * 设置控件属性
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
INSTALLER_API void APIENTRY SetControlAttribute(HWND hwndParent,
                                                int string_size,
                                                char* variables,
                                                stack_t** stacktop,
                                                extra_parameters* extra);

/**
 * 设置控件属性
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
INSTALLER_API void APIENTRY GetControlAttribute(HWND hwndParent,
                                                int string_size,
                                                char* variables,
                                                stack_t** stacktop,
                                                extra_parameters* extra);

/**
 * 复选框是否选中
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
INSTALLER_API void APIENTRY IsChcecked(HWND hwndParent,
                                       int string_size,
                                       char* variables,
                                       stack_t** stacktop,
                                       extra_parameters* extra);

/**
 * 显示提示框
 *
 * @param hwndParent
 * @param string_size
 * @param variables
 * @param stacktop
 * @param extra
 *
 * @return
 */
INSTALLER_API void APIENTRY ShowMessageBox(HWND hwndParent,
                                           int string_size,
                                           char* variables,
                                           stack_t** stacktop,
                                           extra_parameters* extra);

#endif // INSTALLER_DUI_INSTALLER_API_H_
