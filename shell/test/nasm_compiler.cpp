#include "UCommon.h"
#include "NASMCompiler.h"


NASMCompiler::NASMCompiler()
{
	mChildStdOutRead = nullptr;
	mChildStdOutWrite = nullptr;

	::ZeroMemory(&mProcInfo, sizeof(PROCESS_INFORMATION));
}

NASMCompiler::~NASMCompiler()
{
	closeCompiler();
}


bool NASMCompiler::startCompiler(std::wstring command)
{
	BOOL success = FALSE;
	STARTUPINFO startInfo;

	//Set the bInheritHandle flag so pipe handles are inherited.
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = nullptr;

	//Create a pipe for the child process's STDOUT.
	if (!::CreatePipe(&mChildStdOutRead, &mChildStdOutWrite, &saAttr, 0))
		return false;
	//Ensure the read handle to the pipe for STDOUT is not inherited.
	if (!::SetHandleInformation(mChildStdOutRead, HANDLE_FLAG_INHERIT, 0))
		return false;

	wchar_t *cmdline = new wchar_t[command.length() + 1];
	cmdline[command.length()] = L'\0';
	command._Copy_s(cmdline, command.length() + 1, command.length());

	::ZeroMemory(&startInfo, sizeof(STARTUPINFO));
	startInfo.cb = sizeof(STARTUPINFO);
	startInfo.hStdError = mChildStdOutWrite;
	startInfo.hStdOutput = mChildStdOutWrite;
	startInfo.dwFlags |= STARTF_USESTDHANDLES;
	startInfo.wShowWindow = SW_HIDE;

	success = ::CreateProcessW(
		nullptr,
		cmdline,     // command line 
		nullptr,          // process security attributes 
		nullptr,          // primary thread security attributes 
		TRUE,             // handles are inherited 
		CREATE_NO_WINDOW,     // creation flags 
		nullptr,          // use parent's environment 
		nullptr,          // use parent's current directory 
		&startInfo,  // STARTUPINFO pointer 
		&mProcInfo);  // receives PROCESS_INFORMATION 

	delete cmdline;

	return (success == TRUE) ? true : false;
}

void NASMCompiler::closeCompiler()
{
	if (mProcInfo.hThread != nullptr)
		::CloseHandle(mProcInfo.hThread);
	if (mProcInfo.hProcess != nullptr)
		::CloseHandle(mProcInfo.hProcess);
	::ZeroMemory(&mProcInfo, sizeof(PROCESS_INFORMATION));

	if (mChildStdOutRead != nullptr)
	{
		::CloseHandle(mChildStdOutRead);
		mChildStdOutRead = nullptr;
	}

	if (mChildStdOutWrite != nullptr)
	{
		::CloseHandle(mChildStdOutWrite);
		mChildStdOutWrite = nullptr;
	}
}

std::wstring NASMCompiler::startReading()
{
	char chBuf[4096];
	BOOL success = FALSE;
	DWORD numBytesRead = 0;
	DWORD totalBytesAvailable = 0;
	DWORD bytesLeftThisMessage = 0;

	std::string output;
	std::wstring outputw;

	WaitForSingleObject(mProcInfo.hProcess, 1000 * 60);

	for (;;)
	{
		success = ::PeekNamedPipe(
			mChildStdOutRead, chBuf, 1, &numBytesRead, 
			&totalBytesAvailable, &bytesLeftThisMessage);
		if (!success)
			break;

		if (numBytesRead != 0)
		{
			success = ::ReadFile(mChildStdOutRead, chBuf, 4095, &numBytesRead, NULL);
			if (!success)
				break;

			output.append(chBuf, numBytesRead);
		}
		else
			break;
	}

	if (!output.empty())
	{
		size_t needWCharSize;
		mbstowcs_s(&needWCharSize, nullptr, 0, output.c_str(), 0);
		if (needWCharSize > 0)
		{
			wchar_t *wBuffer = new wchar_t[needWCharSize + 1];
			wBuffer[needWCharSize] = L'\0';
			mbstowcs_s(&needWCharSize, wBuffer, needWCharSize, output.c_str(), needWCharSize);
			outputw.append(wBuffer, needWCharSize);
		}
	}

	return outputw;
}