#pragma once
#include <string>
#include <vector>

class Utils
{
public:
	static int cStringToPositiveInt(const char *str);
	static std::string intToString(int value);

	template <typename T>
	static std::vector<T> anyToVec(const T& arg1);
	template <typename T>
	static std::vector<T> anyToVec(const T& arg1, const T& arg2);
	template <typename T>
	static std::vector<T> anyToVec(const T& arg1, const T& arg2, const T& arg3);
	template <typename T>
	static std::vector<T> anyToVec(const T& arg1, const T& arg2, const T& arg3, const T& arg4);
};

#include "../src/Utils.tpp"