#pragma once
#include <string>

// ヘルパー関数群
namespace Helper {

	// ログの出力
	void Log(const std::string& str);
	void Log(const std::wstring& str);

	// stringとwstringの変換
	std::wstring ConvertString(const std::string& str);
	std::string ConvertString(const std::wstring& str);

	// パスの操作
	std::string GetParentPath(const std::string& path);
	std::string GetFilename(const std::string& path);
	std::string GetStem(const std::string& path);
	std::string GetExtension(const std::string& path);

}
