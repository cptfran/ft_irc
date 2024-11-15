#pragma once

/**
* @brief Assign argument to vector and return the vector.
*
* It takes any type of argument and assigns it to vector which is returned.
*
* @tparam T Argument of any type, that will be assigned to the vector.
*/
template <typename T>
std::vector<T> Utils::anyToVec(const T& arg1)
{
	std::vector<T> argVec;
	argVec.push_back(arg1);
	return argVec;
}

/**
* @brief Assign arguments to a vector.
*
* It takes any type of argument and assigns it to vector which is returned.
*
* @tparam T Argument of any type, that will be assigned to the vector.
*/
template <typename T>
std::vector<T> Utils::anyToVec(const T& arg1, const T& arg2)
{
	std::vector<T> argVec;
	argVec.push_back(arg1);
	argVec.push_back(arg2);
	return argVec;
}

/**
* @brief Assign arguments to a vector.
*
* It takes any type of argument and assigns it to vector which is returned.
*
* @tparam T Argument of any type, that will be assigned to the vector.
*/
template <typename T>
std::vector<T> Utils::anyToVec(const T& arg1, const T& arg2, const T& arg3)
{
	std::vector<T> argVec;
	argVec.push_back(arg1);
	argVec.push_back(arg2);
	argVec.push_back(arg3);
	return argVec;
}

/**
* @brief Assign arguments to a vector.
*
* It takes any type of argument and assigns it to vector which is returned.
*
* @tparam T Argument of any type, that will be assigned to the vector.
*/
template <typename T>
std::vector<T> Utils::anyToVec(const T& arg1, const T& arg2, const T& arg3, const T& arg4)
{
	std::vector<T> argVec;
	argVec.push_back(arg1);
	argVec.push_back(arg2);
	argVec.push_back(arg3);
	argVec.push_back(arg4);
	return argVec;
}