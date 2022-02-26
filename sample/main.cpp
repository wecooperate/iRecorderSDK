#include <iRecorder.h>
#include <locale>

using namespace iRecorder;

class RecorderCallback : public IWindowMonitorCallback
{
public:
	RecorderCallback()
	{
		m_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	void OnWindowChanged(WindowInfo* window, WindowInfo* new_window) override
	{
		if (!window->Window || !window->Process)
			return;

		LPCWSTR process_info = window->Process->FileDescription;
		if (!process_info || !process_info[0]) {
			process_info = window->Process->ProcessName;
		}

		printf("%6d [PID:%6d] %S\n", window->Duration, window->ProcessId, process_info);

		if (window->ExtensionType != emWindowExtensionNone) {
			SetConsoleTextAttribute(m_ConsoleHandle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			printf("\t\t>>> %S\n", window->ExtensionUrl);
			SetConsoleTextAttribute(m_ConsoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}
	}

	void OnWindowContinued(WindowInfo* window) override
	{
	}

protected:
	HANDLE m_ConsoleHandle = NULL;
};

int main(int argc, const char* argv[])
{
	setlocale(LC_ALL, "");

	Recorder recoder;

	HRESULT hr = recoder.Initialize();

	if (hr != S_OK)
		return 0;

	RecorderCallback callback;
	hr = recoder.StartWindowMonitor(&callback);

	if (hr != S_OK)
		return 0;

	WaitForSingleObject(GetCurrentThread(), INFINITE);

	recoder.StopWindowMonitor();

	return 0;
}
