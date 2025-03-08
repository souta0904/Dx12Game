#include "Helper.h"
#include <Windows.h>

namespace Helper {

	// ログの出力
	void Log(const std::string& str) {
#ifdef _DEBUG// デバッグ時のみ
		OutputDebugStringA(std::string("[Log] " + str).c_str());
#else
		(void)str;
#endif
	}

	// wstring用
	void Log(const std::wstring& str) {
		Log(ConvertString(str));
	}

	// stringをwstringへ
	std::wstring ConvertString(const std::string& str) {
		if (str.empty()) {
			return std::wstring();
		}
		int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), nullptr, 0);
		if (sizeNeeded < 1) {
			return std::wstring();
		}
		std::wstring result(sizeNeeded, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded);
		return result;
	}

	// wstringをstringへ
	std::string ConvertString(const std::wstring& str) {
		if (str.empty()) {
			return std::string();
		}
		int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), nullptr, 0, nullptr, nullptr);
		if (sizeNeeded < 1) {
			return std::string();
		}
		std::string result(sizeNeeded, 0);
		WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, nullptr, nullptr);
		return result;
	}

	// 親パス
	std::string GetParentPath(const std::string& path) {
		size_t pos = path.find_last_of("/\\");
		if (pos != std::string::npos) {
			return path.substr(0, pos);
		}
		return std::string();
	}

	// ファイル名
	std::string GetFilename(const std::string& path) {
		size_t pos = path.find_last_of("/\\");
		if (pos != std::string::npos) {
			return path.substr(pos + 1);
		}
		return path;
	}

	// 拡張子を除いたファイル名
	std::string GetStem(const std::string& path) {
		std::string filename = GetFilename(path);
		size_t pos = filename.find_last_of(".");
		if (pos != std::string::npos) {
			return filename.substr(0, pos);
		}
		return filename;
	}

	// 拡張子
	std::string GetExtension(const std::string& path) {
		std::string filename = GetFilename(path);
		size_t pos = filename.find_last_of(".");
		if (pos != std::string::npos) {
			return filename.substr(pos);
		}
		return std::string();
	}

}
