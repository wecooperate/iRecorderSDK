//******************************************************************************
/*++
	FileName:		iRecorder.h
	Description:

--*/
//******************************************************************************
#ifndef __iRecorder_B598B3CD_8438_4C1D_A0CE_7EDD8E28364D__
#define __iRecorder_B598B3CD_8438_4C1D_A0CE_7EDD8E28364D__
//******************************************************************************
// clang-format off
//******************************************************************************
#include <tchar.h>
#include <windows.h>
#include <atlbase.h>
//******************************************************************************
namespace iRecorder {
//******************************************************************************
#ifdef _M_IX86
#define RECORDER_MODULE_NAME _T("iRecorder.dll")
#else
#define RECORDER_MODULE_NAME _T("iRecorder64.dll")
#endif
//******************************************************************************
enum emWindowExtension {
	emWindowExtensionNone,

	emWindowExtensionOfficeBegin = 1,
	emWindowExtensionOfficeWord,
	emWindowExtensionOfficeExcel,
	emWindowExtensionOfficePPT,
	emWindowExtensionOfficeEnd = 100,

	emWindowExtensionUrlBegin = 101,
	emWindowExtensionUrlIE,
	emWindowExtensionUrlChrome,
	emWindowExtensionUrlEnd = 200,

	emWindowExtensionIMBegin = 201,
	emWindowExtensionIMWeChat,
	emWindowExtensionIMEnd = 300,
};
//******************************************************************************
struct ProcessInfo
{
	LPCWSTR				ProcessName;
	LPCWSTR				ProcessPath;
	LPCWSTR				ProcessCommandline;
	LPCWSTR				CompanyName;
	LPCWSTR				ProductName;
	LPCWSTR				FileDescription;
};
//******************************************************************************
struct WindowInfo
{
	ULONGLONG			Time;
	ULONG				Duration;
	ULONG				ProcessId;
	HWND				Window;
	LPCWSTR				WindowName;
	LPCWSTR				WindowClassName;
	emWindowExtension	ExtensionType;
	union {
		LPCWSTR			ExtensionValue;
		LPCWSTR			ExtensionOfficePath;
		LPCWSTR			ExtensionUrl;
		LPCWSTR			ExtensionIMUser;
	};
	ProcessInfo*		Process;
};
//******************************************************************************
struct WindowMonitorConfig
{
	bool				EnableProcessInfo;
	bool				EnableExtensionOffice;
	bool				EnableExtensionUrl;
	bool				EnableExtensionIM;
	ULONG				CheckContinueIntervalMS;

	WindowMonitorConfig()
	{
		EnableProcessInfo = true;
		EnableExtensionOffice = true;
		EnableExtensionUrl = true;
		EnableExtensionIM = true;
		CheckContinueIntervalMS = 5000;
	}
};
//******************************************************************************
interface IWindowMonitorCallback
{
	virtual void		OnWindowChanged		(WindowInfo* window, WindowInfo* new_window) = 0;
	virtual void		OnWindowContinued	(WindowInfo* window) = 0;
};
//******************************************************************************
interface __declspec (uuid("C3C74063-372D-4FC5-9B95-0A5DADDC3FC1")) IWindowMonitor : public IUnknown
{
	virtual HRESULT		Start				(IWindowMonitorCallback* Callback, WindowMonitorConfig Config = WindowMonitorConfig()) = 0;
	virtual HRESULT		Stop				(void) = 0;
};
//******************************************************************************
//
//	helper
//
//******************************************************************************
// clang-format on
//******************************************************************************
class Recorder
{
public:
	Recorder(void)
		: m_RecorderModule(NULL)
	{
	}

	~Recorder(void)
	{
		StopWindowMonitor();

		m_WindowMonitor = NULL;
	}

public:
	HRESULT Initialize(LPCTSTR Path = NULL)
	{
		return LoadMonitor(Path);
	}

	HRESULT StartWindowMonitor(IWindowMonitorCallback* Callback, WindowMonitorConfig Config = WindowMonitorConfig())
	{
		if (!m_WindowMonitor)
			return E_UNEXPECTED;

		return m_WindowMonitor->Start(Callback, Config);
	}

	HRESULT StopWindowMonitor(void)
	{
		if (!m_WindowMonitor)
			return E_UNEXPECTED;

		return m_WindowMonitor->Stop();
	}

protected:
	HRESULT LoadMonitor(LPCTSTR Path)
	{
		if (m_WindowMonitor)
			return S_FALSE;

		if (!Path)
			Path = RECORDER_MODULE_NAME;

		if (!m_RecorderModule) {
			m_RecorderModule = LoadLibraryEx(Path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

			if (!m_RecorderModule)
				return HRESULT_FROM_WIN32(GetLastError());
		}

		typedef HRESULT(STDAPICALLTYPE * PFN_DllGetClassObject)(REFCLSID, REFIID, PVOID*);

		PFN_DllGetClassObject pfn = (PFN_DllGetClassObject)GetProcAddress(m_RecorderModule, "DllGetClassObject");

		if (!pfn)
			return E_FAIL;

		return pfn(CLSID_NULL, __uuidof(IWindowMonitor), (PVOID*)&m_WindowMonitor);
	}

protected:
	HMODULE m_RecorderModule;
	CComPtr<IWindowMonitor> m_WindowMonitor;
};
//******************************************************************************
}; // namespace iRecorder
//******************************************************************************
#endif
