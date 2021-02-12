#include "include/window_to_second_display/window_to_second_display_plugin.h"
#include "include/window_utils.h"

// This must be included before many other Windows headers.
#include <windows.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <map>
#include <memory>
#include <sstream>

namespace
{

  class WindowToSecondDisplayPlugin : public flutter::Plugin
  {
    struct MonitorRects
    {
      std::vector<RECT> rcMonitors;

      static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
      {
        // MONITORINFO info;
        // info.cbSize = sizeof(MONITORINFO);
        // GetMonitorInfo(hMon, &info);

        MonitorRects *pThis = reinterpret_cast<MonitorRects *>(pData);
        pThis->rcMonitors.push_back(*lprcMonitor);
        return TRUE;
      }

      MonitorRects()
      {
        EnumDisplayMonitors(0, 0, MonitorEnum, (LPARAM)this);
      }
    };

  public:
    static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

    WindowToSecondDisplayPlugin(flutter::PluginRegistrarWindows *registrar);

    virtual ~WindowToSecondDisplayPlugin();

  private:
    // Called when a method is called on this plugin's channel from Dart.
    void HandleMethodCall(
        const flutter::MethodCall<flutter::EncodableValue> &method_call,
        std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
    // The registrar for this plugin, for accessing the window.
    flutter::PluginRegistrarWindows *registrar_;
  };

  // static
  void WindowToSecondDisplayPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarWindows *registrar)
  {
    auto channel =
        std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
            registrar->messenger(), "window_to_second_display",
            &flutter::StandardMethodCodec::GetInstance());

    auto plugin = std::make_unique<WindowToSecondDisplayPlugin>(registrar);

    channel->SetMethodCallHandler(
        [plugin_pointer = plugin.get()](const auto &call, auto result) {
          plugin_pointer->HandleMethodCall(call, std::move(result));
        });

    registrar->AddPlugin(std::move(plugin));
  }

  WindowToSecondDisplayPlugin::WindowToSecondDisplayPlugin(flutter::PluginRegistrarWindows *registrar)
      : registrar_(registrar) {}

  WindowToSecondDisplayPlugin::~WindowToSecondDisplayPlugin() {}

  void WindowToSecondDisplayPlugin::HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    if (method_call.method_name().compare("moveWindow") == 0)
    {
      std::ostringstream version_stream;
      version_stream << "You have ";

      HWND m_hWnd = registrar_->GetView()->GetNativeWindow();

      HWND hParent = GetAncestor(m_hWnd, GA_PARENT);

      HWND hCurWnd = ::GetForegroundWindow();
      DWORD dwMyID = ::GetCurrentThreadId();
      DWORD dwCurID = ::GetWindowThreadProcessId(hCurWnd, NULL);

      ::AttachThreadInput(dwCurID, dwMyID, TRUE);
      ::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
      ::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
      ::SetForegroundWindow(m_hWnd);
      ::SetFocus(m_hWnd);
      ::SetActiveWindow(m_hWnd);
      ::AttachThreadInput(dwCurID, dwMyID, FALSE);

      MonitorRects monitors;

      LONG left = 0;
      LONG top = 0;
      LONG height = 768;
      LONG width = 1024;

      if (monitors.rcMonitors.size() > 1)
      {

        version_stream << monitors.rcMonitors.size();
        version_stream << " monitors connected.";

        for (auto &rcMonitor : monitors.rcMonitors)
        {
          if (rcMonitor.left != 0 || rcMonitor.top != 0)
          {
            left = rcMonitor.left;
            top = rcMonitor.top;
            height = rcMonitor.bottom;
            width = rcMonitor.right - left;
          }
        }

        version_stream << " Secondary BEGIN ";
        version_stream << " Left: " << left << " Top: " << top;
        version_stream << " Width: " << width << " Height: " << height;
        version_stream << " END ";
      }
      else
      {
        version_stream << " one monitor.";
      }

      // MoveWindow(m_hWnd, (int)left, (int)top, 1000, (int)bottom, FALSE);
      if (hParent && ::SetWindowPos(hParent, 0, left, top, width, height, SWP_DRAWFRAME))
      {
        version_stream << " Moved Window!";
      }
      else
      {
        version_stream << " Window not moved!";
      }

      result->Success(flutter::EncodableValue(version_stream.str()));
    }
    else
    {
      result->NotImplemented();
    }
  }

} // namespace

void WindowToSecondDisplayPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar)
{
  WindowToSecondDisplayPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
