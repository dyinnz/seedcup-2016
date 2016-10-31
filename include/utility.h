/*******************************************************************************
 * Author: Dyinnz.HUST.UniqueStudio
 * Email:  ml_143@sina.com
 * Github: https://github.com/dyinnz
 * Date:   2016-05-27
 ******************************************************************************/

#pragma once

#include <utility>

/**
 * Scope Guarder
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

class ScopeGuarderEmptyType {
};

template<class T>
inline ScopeGuarder<T> operator+(ScopeGuarderEmptyType, T &&guard) {
  return ScopeGuarder<T>(std::move(guard));
}

#define ScopeGuard auto __FILE__##__LINE__##scope_guarder \
  = ScopeGuarderEmptyType() +
