#include "emulator_window.h"


namespace shell {

    void EmulatorWindow::onCreate() {
        Window::onCreate();

        mChildStdOutRead = nullptr;
        mChildStdOutWrite = nullptr;

        ::ZeroMemory(&mProcInfo, sizeof(PROCESS_INFORMATION));
    }

    bool EmulatorWindow::command(std::wstring command)
    {
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

        std::unique_ptr<wchar_t[]> cmdline(new wchar_t[command.length() + 1]);
        cmdline[command.length()] = L'\0';
        command._Copy_s(cmdline.get(), command.length() + 1, command.length());

        ::ZeroMemory(&startInfo, sizeof(STARTUPINFO));
        startInfo.cb = sizeof(STARTUPINFO);
        startInfo.hStdError = mChildStdOutWrite;
        startInfo.hStdOutput = mChildStdOutWrite;
        startInfo.dwFlags |= STARTF_USESTDHANDLES;
        startInfo.wShowWindow = SW_HIDE;

        BOOL success = ::CreateProcessW(
            nullptr,
            cmdline.get(),    // command line
            nullptr,          // process security attributes
            nullptr,          // primary thread security attributes
            TRUE,             // handles are inherited
            CREATE_NO_WINDOW,     // creation flags
            nullptr,          // use parent's environment
            nullptr,          // use parent's current directory
            &startInfo,    // STARTUPINFO pointer
            &mProcInfo);   // receives PROCESS_INFORMATION

        return (success == TRUE) ? true : false;
    }

    void EmulatorWindow::shutdown()
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

    std::wstring EmulatorWindow::startReading()
    {
        char chBuf[4096];
        DWORD numBytesRead = 0;
        DWORD totalBytesAvailable = 0;
        DWORD bytesLeftThisMessage = 0;

        std::string output;
        std::wstring outputw;

        WaitForSingleObject(mProcInfo.hProcess, 1000 * 60);

        for (;;)
        {
            BOOL success = ::PeekNamedPipe(
                mChildStdOutRead, chBuf, 1, &numBytesRead,
                &totalBytesAvailable, &bytesLeftThisMessage);
            if (!success)
                break;

            if (numBytesRead != 0)
            {
                success = ::ReadFile(mChildStdOutRead, chBuf, 4095, &numBytesRead, nullptr);
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

}