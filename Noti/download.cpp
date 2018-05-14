#include "pch.h"
#include <filesystem>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Web;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;

std::wstring PathFileName(const wchar_t *begin, const wchar_t *end) {
  if (begin == end) {
    return std::wstring(L"index.html");
  }
  std::wstring filename(begin, end - begin);
  while (!filename.empty() && filename.back() == '/') {
    filename.pop_back();
  }
  if (filename.empty()) {
    return std::wstring(L"index.html");
  }
  auto pos = filename.rfind('/');
  if (pos != std::wstring::npos) {
    return filename.substr(pos + 1);
  }
  return filename;
}

// https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Content-Disposition
/*
HttpBaseProtocolFilter support HTTP2 by default.
*/
IAsyncAction NotiSignledownload(const wchar_t *url, const wchar_t *savefile,
                                const Dcontext &dctx) {
  try {
    std::wstring fulldir, filename;
    if (savefile != nullptr) {
      auto p = std::filesystem::path(savefile);
      auto pdir = std::filesystem::path(savefile).parent_path();
      if (!std::filesystem::exists(pdir)) {
        std::error_code ec;
        if (!std::filesystem::create_directories(pdir, ec)) {
          throw std::filesystem::filesystem_error("create dir failed", ec);
        }
      }
      fulldir = pdir.wstring();
      if (p.has_filename()) {
        filename = p.filename().wstring();
      }
    } else {
      fulldir = std::filesystem::current_path().wstring();
    }
    printf("download url: %ls %ls\n", url, fulldir.c_str());
    HttpBaseProtocolFilter baseFilter;
    baseFilter.AllowAutoRedirect(false);
    baseFilter.AllowUI(true);
    // printf("HTTP version: %d\n",baseFilter.MaxVersion()); default enable
    // HTTP2
    HttpClient client(baseFilter);
    client.DefaultRequestHeaders().Append(
        L"User-Agent", L"Mozilla/5.0 (Windows NT 10.0; Win64; x64)");
    Uri uri(url);

    auto resp = co_await client.GetAsync(
        uri, HttpCompletionOption::ResponseHeadersRead); /// Only read header.
    int times = 0;
    while ((resp.StatusCode() == HttpStatusCode::Found ||
            resp.StatusCode() == HttpStatusCode::MovedPermanently ||
            resp.StatusCode() == HttpStatusCode::TemporaryRedirect) &&
           (times <= dctx.tries)) {
      auto location = resp.Headers().Location();
      printf("Redirect to %ls\n", location.AbsoluteUri().c_str());
      resp = co_await client.GetAsync(
          location, HttpCompletionOption::ResponseHeadersRead);
      times++;
    }

    if (!resp.IsSuccessStatusCode()) {
      printf("StatusCode: %d\n", resp.StatusCode());
      return;
    }

    if (resp.Version() == HttpVersion::Http20) {
      printf("HTTP 2.0\n");
    }

    if (filename.empty() && resp.Content().Headers().ContentDisposition()) {
      if (!resp.Content().Headers().ContentDisposition().FileName().empty()) {
        filename.assign(
            resp.Content().Headers().ContentDisposition().FileName().c_str());
        printf("use filename %ls\n", filename.c_str());
      }
    }

    if (filename.empty()) {
      filename = PathFileName(uri.Path().c_str(),
                              uri.Path().c_str() + uri.Path().size());
      printf("filename: %ls\n", filename.c_str());
    }

    uint64_t blen = 0;
    if (resp.Content().TryComputeLength(blen)) {
    }
    /// If cannot download some dir,
    auto folder = co_await StorageFolder::GetFolderFromPathAsync(
        fulldir); /// Must fullpath
    auto file = co_await folder.CreateFileAsync(
        filename,
        CreationCollisionOption::ReplaceExisting); /// replace existing
    auto stream = co_await file.OpenAsync(FileAccessMode::ReadWrite);
    // HttpProgress

    auto task = resp.Content().WriteToStreamAsync(stream);
    task.Progress([](const IAsyncOperationWithProgress<uint64_t, uint64_t> &,
                     const uint64_t &pb) {
      //
      wprintf(L"download %llu\n", pb);
    });
    // await stream.FlushAsync();

    auto result = co_await task;
    wprintf(L"total %llu\n", result);
  } catch (const hresult_error &e) {
    printf("download error: 0x%08x %ls\n", e.code(), e.message().c_str());
  } catch (const std::exception &e) {
    printf("download url: %s\n", e.what());
  }
}

IAsyncAction Notidownload(const Dcontext &dctx) {
  try {
    co_await NotiSignledownload(dctx.urls[0].c_str(), nullptr, dctx);
  } catch (const hresult_error &e) {
    printf("open dir: %ls\n", e.message().c_str());
  } catch (const std::exception &e) {
    printf("open dir: %s\n", e.what());
  }
}