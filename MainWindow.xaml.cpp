#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include "microsoft.ui.xaml.window.h"
#define BUILD_WINDOWS
#include "winrt/Microsoft.UI.Interop.h"
#undef UNICODE
#include <windows.h>
#include <string>
#include <shlobj.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <future>
#pragma comment(lib,"shell32.lib")

using namespace winrt;
using namespace Microsoft::UI::Xaml;


static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM, LPARAM lpData)
{

    if (uMsg == BFFM_INITIALIZED)
    {
        std::string tmp = (const char*)lpData;
        std::cout << "path: " << tmp << std::endl;
        SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
    }

    return 0;
}

std::string BrowseFolder(std::string saved_path)
{
    TCHAR path[MAX_PATH];

    const char* path_param = saved_path.c_str();

    BROWSEINFO bi = { 0 };
    bi.lpszTitle = ("Select Fortnite build folder");
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.lpfn = BrowseCallbackProc;
    bi.lParam = (LPARAM)path_param;

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if (pidl != 0)
    {
        //get the name of the folder and put it in path
        SHGetPathFromIDList(pidl, (LPSTR) path);

        //free memory used
        IMalloc* imalloc = 0;
        if (SUCCEEDED(SHGetMalloc(&imalloc)))
        {
            imalloc->Free(pidl);
            imalloc->Release();
        }

        return (LPSTR) path;
    }

    return "";
}

namespace winrt::Paradise::implementation
{
    void MainWindow::InitializeComponent() {
        MainWindowT::InitializeComponent();
        ExtendsContentIntoTitleBar(true);
        
        auto windowNative{ this->try_as<::IWindowNative>() };
        winrt::check_bool(windowNative);
        windowNative->get_WindowHandle(&hWnd);

        HMONITOR hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

        MONITORINFO info{ sizeof(info) }; // set cbSize member and fill the rest with zero
        if (::GetMonitorInfoA(hMonitor, &info))
        {
            int wd = (int) ((info.rcWork.left + info.rcWork.right) / 1.5), h = (int)((info.rcWork.top + info.rcWork.bottom) / 1.5);
            int x = (info.rcWork.left + info.rcWork.right) / 2 - wd / 2;
            int y = (info.rcWork.top + info.rcWork.bottom) / 2 - h / 2;

            SetWindowPos(hWnd, 0, x, y, wd, h, SWP_SHOWWINDOW);
        }
    }

    void Inject(HANDLE proc, const char* path) {
        void* nameRegion = VirtualAllocEx(proc, nullptr, strlen(path) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        WriteProcessMemory(proc, nameRegion, path, strlen(path) + 1, NULL);

        HMODULE k32 = GetModuleHandleA("kernel32.dll");
        FARPROC LLA = GetProcAddress(k32, "LoadLibraryA");
        HANDLE tr = CreateRemoteThread(proc, 0, 0, (LPTHREAD_START_ROUTINE)LLA, nameRegion, 0, 0);
        WaitForSingleObject(tr, (DWORD) -1);
        CloseHandle(tr);
        VirtualFreeEx(proc, nameRegion, strlen(path) + 1, MEM_RELEASE);
    }

    void InternalLaunch(std::string Backend, std::string Email, std::string Password) {
        if (Backend.c_str()[0] == 0 || Email.c_str()[0] == 0 || Password.c_str()[0] == 0) {
            return (void)MessageBoxA(MainWindow::hWnd, "Please enter in your details!", "Paradise", MB_ICONERROR | MB_OK);
        }
        auto fn = BrowseFolder("");
        std::ifstream f(fn + "\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe");
        if (!f.is_open()) {
            return fn != "" ? (void)MessageBoxA(MainWindow::hWnd, "Folder is not a valid fortnite install!", "Paradise", MB_ICONERROR | MB_OK) : (void) 0;
        }
        std::string params = fn + "\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe -epicapp=Fortnite -epicenv=Prod -epicportal -skippatchcheck -nobe -fromfl=eac -fltoken=3db3ba5dcbd2e16703f3978d -caldera=eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCJ9.eyJhY2NvdW50X2lkIjoiYmU5ZGE1YzJmYmVhNDQwN2IyZjQwZWJhYWQ4NTlhZDQiLCJnZW5lcmF0ZWQiOjE2Mzg3MTcyNzgsImNhbGRlcmFHdWlkIjoiMzgxMGI4NjMtMmE2NS00NDU3LTliNTgtNGRhYjNiNDgyYTg2IiwiYWNQcm92aWRlciI6IkVhc3lBbnRpQ2hlYXQiLCJub3RlcyI6IiIsImZhbGxiYWNrIjpmYWxzZX0.VAWQB67RTxhiWOxx7DBjnzDnXyyEnX7OljJm-j2d88G_WgwQ9wrE6lwMEHZHjBd1ISJdUO1UVUqkfLdU5nofBQ";
        STARTUPINFOA info = { sizeof(info) };
        PROCESS_INFORMATION processInfo;
        CreateProcessA((fn + "\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe").c_str(), (char*)(params + " -AUTH_LOGIN=" + Email + " -AUTH_PASSWORD=" + Password + " -AUTH_TYPE=epic -backend=" + Backend).c_str(), NULL, NULL, true, CREATE_NO_WINDOW, nullptr, fn.c_str(), &info, &processInfo);
        STARTUPINFOA Nuh = { sizeof(info) };
        PROCESS_INFORMATION Uh;
        CreateProcessA((fn + "\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping_EAC.exe").c_str(), "", NULL, NULL, true, CREATE_NO_WINDOW | CREATE_SUSPENDED, nullptr, fn.c_str(), &Nuh, &Uh);
        STARTUPINFOA Plo = { sizeof(info) };
        PROCESS_INFORMATION Osh;
        CreateProcessA((fn + "\\FortniteGame\\Binaries\\Win64\\FortniteLauncher.exe").c_str(), "", NULL, NULL, true, CREATE_NO_WINDOW | CREATE_SUSPENDED, nullptr, fn.c_str(), &Plo, &Osh);
        TCHAR p[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, (LPSTR)p);
        Inject(processInfo.hProcess, (std::string((char*)p) + "\\Assets\\Starfall.dll").c_str());
        WaitForSingleObject(processInfo.hProcess, (DWORD)-1);

        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
        TerminateProcess(Uh.hProcess, 0);
        CloseHandle(Uh.hProcess);
        CloseHandle(Uh.hThread);
        TerminateProcess(Osh.hProcess, 0);
        CloseHandle(Osh.hProcess);
        CloseHandle(Osh.hThread);
    }

    std::string hstringToString(winrt::hstring s) {
        auto st = s.c_str();
        std::string os;
        for (int i = 0; i < wcslen(st); i++) {
            os += (char) st[i];
        }

        return os;
    }


    void MainWindow::Launch(IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&) {
        std::thread(InternalLaunch, hstringToString(a().Text()), hstringToString(b().Text()), hstringToString(c().Password())).detach();
        //CreateThread(0, 0, (LPTHREAD_START_ROUTINE) InternalLaunch, &d, 0, 0);
    }
}
