#pragma once

#include <string>

bool isCFunc(const std::string& func);

std::string trimPrefix(const std::string& fname);

bool isCMath(const std::string& func);

/**
 * @brief isStdLibFunc Checks whether the function is part of stdlib.h
 * @param func Function name
 * @return True if function is part of stdlib.h, false otherwise
 */
bool isStdLibFunc(const std::string& func);

/**
 * @brief isStringFunc Checks whether the function is part of string.h
 * @param func Function name
 * @return True if function is part of string.h, false otherwise
 */
bool isStringFunc(const std::string& func);

/**
 * @brief isStdioFunc Checks whether the function is part of stdio.h
 * @param func Function name
 * @return True if function is part of stdio.h, false otherwise
 */
bool isStdioFunc(const std::string& func);

/**
 * @brief isPthreadFunc Checks whether the function is part of pthread.h
 * @param func Function name
 * @return True if function is part of pthread.h, false otherwise
 */
bool isPthreadFunc(const std::string& func);

