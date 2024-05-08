#pragma once

#include "MainWindow.g.h"
namespace winrt::Paradise::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        inline static HWND hWnd{ 0 };
        MainWindow()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

        void InitializeComponent();

        void Launch(IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::Paradise::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
