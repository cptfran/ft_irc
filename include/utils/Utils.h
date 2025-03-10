#pragma once
#include <sstream>
#include <string>
#include <vector>

class Utils
{
public:
	static int cStringToPositiveInt(const char *str);
	static std::string intToString(int value);

	static std::string stringToUpper(const std::string& str);
	static std::vector<std::string> splitStringByComma(const std::string& str);

	template <typename T>
	static std::vector<T> anyToVec(const T& arg1);
	template <typename T>
	static std::vector<T> anyToVec(const T& arg1, const T& arg2);
	template <typename T>
	static std::vector<T> anyToVec(const T& arg1, const T& arg2, const T& arg3);
	template <typename T>
	static std::vector<T> anyToVec(const T& arg1, const T& arg2, const T& arg3, const T& arg4);
	template <typename T>
	static std::vector<T> anyToVec(const T& arg1, const T& arg2, const T& arg3, const T& arg4, const T& arg5);

private:
	Utils();
	~Utils();
	Utils(const Utils& toCopy);
	Utils& operator=(const Utils& toAssign);
};

#include "../../src/utils/Utils.tpp"