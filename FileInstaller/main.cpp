#include <windows.h>
#include <vector>

#include "FileInstaller.h"

std::vector<LPCWSTR> installation_files = { L"pch.cpp", L"a.txt", L"rotem.txt" };
LPCWSTR installation_dir = L"target";

int main()
{
	FileInstaller f = FileInstaller(installation_files, installation_dir);
	f.install();
}