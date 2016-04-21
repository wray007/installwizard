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
INSTALLER_API void APIENTRY InitWizard(HWND hwndParent,
                                       int string_size,
                                       char* variables,
                                       stack_t** stacktop,
                                       extra_parameters* extra);

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
INSTALLER_API void APIENTRY SetWindowTitle(HWND hwndParent,
                                           int string_size,
                                           char* variables,
                                           stack_t** stacktop,
                                           extra_parameters* extra);

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
INSTALLER_API void APIENTRY SetInstallDir(HWND hwndParent,
                                          int string_size,
                                          char* variables,
                                          stack_t** stacktop,
                                          extra_parameters* extra);

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
INSTALLER_API void APIENTRY GetInstallDir(HWND hwndParent,
                                          int string_size,
                                          char* variables,
                                          stack_t** stacktop,
                                          extra_parameters* extra);


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
INSTALLER_API void APIENTRY ShowWizard(HWND hwndParent,
                                       int string_size,
                                       char* variables,
                                       stack_t** stacktop,
                                       extra_parameters* extra);

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
INSTALLER_API void APIENTRY ShowWizardStep(HWND hwndParent,
                                           int string_size,
                                           char* variables,
                                           stack_t** stacktop,
                                           extra_parameters* extra);

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
INSTALLER_API void APIENTRY BindCallback(HWND hwndParent,
                                         int string_size,
                                         char* variables,
                                         stack_t** stacktop,
                                         extra_parameters* extra);

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
INSTALLER_API void APIENTRY SetControlAttribute(HWND hwndParent,
                                                int string_size,
                                                char* variables,
                                                stack_t** stacktop,
                                                extra_parameters* extra);

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
INSTALLER_API void APIENTRY GetControlAttribute(HWND hwndParent,
                                                int string_size,
                                                char* variables,
                                                stack_t** stacktop,
                                                extra_parameters* extra);

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
INSTALLER_API void APIENTRY IsChcecked(HWND hwndParent,
                                       int string_size,
                                       char* variables,
                                       stack_t** stacktop,
                                       extra_parameters* extra);

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
INSTALLER_API void APIENTRY ShowMessageBox(HWND hwndParent,
                                           int string_size,
                                           char* variables,
                                           stack_t** stacktop,
                                           extra_parameters* extra);

#endif // INSTALLER_DUI_INSTALLER_API_H_
