#pragma once

class NASMCompiler
{
private:
	HANDLE mChildStdOutRead;
	HANDLE mChildStdOutWrite;
	PROCESS_INFORMATION mProcInfo;

public:
	NASMCompiler();
	~NASMCompiler();

	bool startCompiler(std::wstring command);
	void closeCompiler();

	std::wstring startReading();
};