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

/**
 * class filter_wrapper
 */
template<class C, class F>
class FilterWrapper {
 public:
  FilterWrapper(C container, F filter) : _container(container),
                                         _filter(filter) {}

  class FilterIterator {
   public:
    typedef typename C::const_iterator inner_iterator;

    FilterIterator(inner_iterator curr, inner_iterator end, F filter) :
        _curr(curr), _end(end), _filter(filter) {}

    FilterIterator &operator++() {
      if (_curr != _end) {
        for (++_curr; _curr != _end; ++_curr) {
          if (_filter(*_curr)) {
            return *this;
          }
        }
      }
      return *this;
    }

    typename C::const_reference operator*() {
      return *_curr;
    }

    bool operator!=(const FilterIterator &rhs) {
      return _curr != rhs._curr;
    }

   private:
    inner_iterator _curr;
    inner_iterator _end;
    F _filter;
  };

  typedef const FilterIterator const_iterator;
  typedef const typename C::const_reference const_reference;

  const_iterator begin() {
    for (auto beg = _container.begin(); beg != _container.end(); ++beg) {
      if (_filter(*beg)) {
        return const_iterator(beg, _container.end(), _filter);
      }
    }
    return end();
  }

  const_iterator end() {
    static const_iterator end(_container.end(), _container.end(), _filter);
    return end;
  }

 private:
  C _container;
  F _filter;
};

template<class C, class F>
FilterWrapper<C, F> FilterContainer(C container, F filter) {
  return FilterWrapper<C, F>(container, filter);
};

