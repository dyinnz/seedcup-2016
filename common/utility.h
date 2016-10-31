/*******************************************************************************
 * Author: Dyinnz.HUST.UniqueStudio
 * Email:  ml_143@sina.com
 * Github: https://github.com/dyinnz
 * Date:   2016-05-27
 ******************************************************************************/

/**
 * This is a simple Scope Guard library.
 *
 * To ensure that resources are always released in face of an exception
 * but not while returning normally
 */

#pragma once

#include <utility>

/**
 * @brief A class using RAII
 */
template<class T>
class ScopeGuarder {
 public:
  ScopeGuarder(T &&guard) : _guard(std::move(guard)) {}

  ~ScopeGuarder() { _guard(); }

  ScopeGuarder(ScopeGuarder &&) = default;

 private:
  ScopeGuarder &operator=(ScopeGuarder &&) = delete;

  ScopeGuarder &operator=(const ScopeGuarder &) = delete;

  ScopeGuarder(const ScopeGuarder &) = delete;

  T _guard;
};

/**
 * @brief A empty helper class
 */
class ScopeGuarderEmptyType {
};

/**
 * @brief Used by macro
 */
template<class T>
inline ScopeGuarder<T> operator+(ScopeGuarderEmptyType, T &&guard) {
  return ScopeGuarder<T>(std::move(guard));
}

/**
 * @brief For easier usage
 */
#define ScopeGuard auto __FILE__##__LINE__##scope_guarder \
  = ScopeGuarderEmptyType() +
