#ifndef PROGRESS_HPP
#define PROGRESS_HPP
#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Shobjidl.h>
#include <Windows.h>

#pragma comment(lib, "Explorerframe.lib")

namespace ui {
// ITaskbarList3
// https://msdn.microsoft.com/en-us/library/windows/desktop/dd378460(v=vs.85).aspx
class Progressbar {
public:
  Progressbar() {
    //
  }
  ~Progressbar() {
    if (iTaskbarList != nullptr) {
      iTaskbarList->Release();
    }
  }
  Progressbar(const Progressbar &) = delete;
  Progressbar &operator=(const Progressbar &) = delete;
  bool Initialize() {
    hConsole = GetConsoleWindow();
    auto hr = CoCreateInstance(
        CLSID_TaskbarList, // 3. Wait for that message, and create instance.
        NULL, CLSCTX_ALL, __uuidof(ITaskbarList3),
        reinterpret_cast<void **>(&iTaskbarList));
    return hr == S_OK;
  }
  int Update(int n) {
    if (hConsole != nullptr && iTaskbarList != nullptr) {
      iTaskbarList->SetProgressValue(hConsole, n, 100);
    }
    return 0;
  }
  void Finish() {
    if (iTaskbarList != nullptr) {
      iTaskbarList->SetProgressState(hConsole, TBPF_NOPROGRESS);
    }
  }
  void Paused() {
    if (iTaskbarList != nullptr) {
      iTaskbarList->SetProgressState(hConsole, TBPF_PAUSED);
    }
  }
  void Error() {
    if (iTaskbarList != nullptr) {
      iTaskbarList->SetProgressState(hConsole, TBPF_ERROR);
    }
  }

private:
  HWND hConsole{nullptr};
  ITaskbarList3 *iTaskbarList{nullptr};
};
} // namespace ui

#endif