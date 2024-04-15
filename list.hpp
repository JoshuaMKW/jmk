#pragma once

#include <cassert>
#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <iterator>

namespace JMK {

  template <typename _T>
  class list {
    struct _node {
      _node* m_next;
      _node* m_prev;
      _T m_item;
    };

  public:

    class iterator {
      friend class JMK::list<_T>;

    public:
      using iterator_category = std::bidirectional_iterator_tag;
      using value_type = std::remove_reference_t<_T>;
      using difference_type = std::ptrdiff_t;
      using pointer = value_type*;
      using reference = value_type&;

      iterator() = delete;
      iterator(const iterator&) = default;
      iterator(_node* value) noexcept : m_value(value) {}

      [[nodiscard]] iterator operator+(difference_type i) const noexcept {
        if (i < 0) {
          return operator-(-i);
        }
        iterator copy = *this;
        while (i > 0) {
          copy.m_value = copy.m_value->m_next;
          i -= 1;
        }
        return copy;
      }

      iterator& operator++() noexcept {
        m_value = m_value->m_next;
        return *this;
      }

      iterator& operator++(int) noexcept {
        iterator copy = *this;
        m_value = m_value->m_next;
        return copy;
      }

      [[nodiscard]] iterator operator-(difference_type i) const noexcept {
        if (i < 0) {
          return operator+(-i);
        }
        iterator copy = *this;
        while (i > 0) {
          copy.m_value = copy.m_value->m_prev;
          i -= 1;
        }
        return copy;
      }

      iterator& operator--() noexcept {
        m_value = m_value->m_prev;
        return *this;
      }

      iterator& operator--(int) noexcept {
        iterator copy = *this;
        m_value = m_value->m_prev;
        return copy;
      }

      [[nodiscard]] reference operator*() noexcept {
        return m_value->m_item;
      }

      [[nodiscard]] pointer operator->() noexcept {
        return &m_value->m_item;
      }

      iterator& operator=(const iterator&) = default;

      [[nodiscard]] bool operator==(const iterator& other) const& = default;
      [[nodiscard]] std::strong_ordering operator<=>(const iterator& other) const& = default;

    private:
      _node* m_value;
    };

    class const_iterator {
      friend class JMK::list<_T>;

    public:
      using iterator_category = std::contiguous_iterator_tag;
      using value_type = std::remove_reference_t<const _T>;
      using difference_type = std::ptrdiff_t;
      using pointer = value_type*;
      using reference = value_type&;

      const_iterator() = delete;
      const_iterator(const const_iterator&) = default;
      const_iterator(_node* value) noexcept : m_value(value) {}

      [[nodiscard]] const_iterator operator+(difference_type i) const noexcept {
        if (i < 0) {
          return operator-(-i);
        }
        const_iterator copy = *this;
        while (i > 0) {
          copy.m_value = copy.m_value->m_next;
          i -= 1;
        }
        return *copy;
      }

      const_iterator& operator++() noexcept {
        m_value = m_value->m_next;
        return *this;
      }

      const_iterator& operator++(int) noexcept {
        const_iterator copy = *this;
        m_value = m_value->m_next;
        return copy;
      }

      [[nodiscard]] const_iterator operator-(difference_type i) const noexcept {
        if (i < 0) {
          return operator+(-i);
        }
        const_iterator copy = *this;
        while (i > 0) {
          copy.m_value = copy.m_value->m_prev;
          i -= 1;
        }
        return *copy;
      }

      const_iterator& operator--() noexcept {
        m_value = m_value->m_prev;
        return *this;
      }

      const_iterator& operator--(int) noexcept {
        const_iterator copy = *this;
        m_value = m_value->m_prev;
        return copy;
      }

      [[nodiscard]] reference operator*() noexcept {
        return m_value->m_item;
      }

      [[nodiscard]] pointer operator->() noexcept {
        return &m_value->m_item;
      }

      const_iterator& operator=(const const_iterator&) = default;

      [[nodiscard]] bool operator==(const const_iterator& other) const& = default;
      [[nodiscard]] std::strong_ordering operator<=>(const const_iterator& other) const& = default;

    private:
      _node* m_value;
    };

    using reverse_iterator = std::reverse_iterator<iterator>;
    using reverse_const_iterator = std::reverse_iterator<const_iterator>;

    list() noexcept(std::is_trivially_constructible_v<_T>) : list(_T(), 0) {}

    template <typename _U>
    list(const list<_U>& other) noexcept(std::is_trivially_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>&&
      std::is_trivially_copy_assignable_v<_U>&&
      std::is_trivially_constructible_v<_U>) : m_end(nullptr), m_size(0) {
      m_begin = _sentinel_node();
      _copy_from_list(other);
    }

    template <typename _U>
    list(list<_U>&& other) noexcept(std::is_trivially_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>&&
      std::is_trivially_move_assignable_v<_U>&&
      std::is_trivially_constructible_v<_U>) : m_end(nullptr), m_size(0) {
      m_begin = _sentinel_node();
      _move_from_list(std::move(other));
    }

    list(_T _fill, size_t size) noexcept(std::is_trivially_copy_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) : m_end(nullptr), m_size(0) {
      m_begin = _sentinel_node();
      resize(size);
      fill(_fill);
    }

    list(std::initializer_list<_T> list) noexcept(std::is_trivially_copy_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) : m_end(nullptr), m_size(0) {
      m_begin = _sentinel_node();
      _init_from_list(list);
    }

    [[nodiscard]] size_t size() const noexcept { return m_size; }
    [[nodiscard]] size_t max_size() const noexcept { return std::numeric_limits<size_t>::max(); }

    [[nodiscard]] bool empty() const noexcept { return m_size == 0; }

    [[nodiscard]] _T& at(size_t index) {
      assert(index < size());
      return *(begin() + index);
    }

    [[nodiscard]] const _T& at(size_t index) const {
      assert(index < size());
      return *(begin() + index);
    }

    [[nodiscard]] _T& operator[](size_t index) noexcept {
      return *(begin() + index);
    }

    [[nodiscard]] const _T& operator[](size_t index) const noexcept {
      return *(begin() + index);
    }

    [[nodiscard]] _T& front() noexcept { return *begin(); }
    [[nodiscard]] const _T& front() const noexcept { return *begin(); }

    [[nodiscard]] _T& back() noexcept { return *(end() - 1); }
    [[nodiscard]] const _T& back() const noexcept { return *(end() - 1); }

    void resize(size_t new_size) {
      _resize_impl(new_size);
    }

    [[nodiscard]] iterator begin() noexcept { return m_begin; }
    [[nodiscard]] iterator end() noexcept { return _sentinel_node(); }
    [[nodiscard]] const_iterator begin() const noexcept { return m_begin; }
    [[nodiscard]] const_iterator end() const noexcept { return _sentinel_node(); }

    [[nodiscard]] const_iterator cbegin() const noexcept { return m_begin; }
    [[nodiscard]] const_iterator cend() const noexcept { return _sentinel_node(); }

    [[nodiscard]] reverse_iterator rbegin() noexcept { return _sentinel_node(); }
    [[nodiscard]] reverse_iterator rend() noexcept { return m_begin; }

    [[nodiscard]] reverse_iterator crbegin() const noexcept { return _sentinel_node(); }
    [[nodiscard]] reverse_iterator crend() const noexcept { return m_begin; }

    template <typename ... _Args>
    void emplace_back(_Args&& ... args) noexcept(std::is_trivially_constructible_v<_T>) {
      emplace(end(), std::forward<_Args>(args)...);
      m_size += 1;
    }

    void push_back(const _T& value) noexcept(std::is_trivially_copy_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) {
      insert(end(), _T(value));
      m_size += 1;
    }

    _T pop_back() noexcept(std::is_trivially_copy_assignable_v<_T>&& std::is_trivially_constructible_v<_T>) {
      assert(m_size > 0);
      return erase(end());
    }

    template <typename ... _Args>
    iterator emplace(iterator pos, _Args&& ... args) noexcept(std::is_trivially_copy_assignable_v<_T>&&
      std::is_constructible_v<_T>) {
      insert(pos, _T(std::forward<_Args>(args)...));
      m_size += 1;
      return pos;
    }

    iterator insert(iterator pos, const _T& value) noexcept(std::is_trivially_copy_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) {
      _node* new_node = new _node;
      new_node->m_item = value;
      _insert_node(new_node, pos.m_value);
      return iterator(new_node);
    }

    iterator erase(iterator pos) noexcept(std::is_trivially_destructible_v<_T>) {
      _node* next = pos.m_value->m_next;
      _remove_node(pos.m_value);
      delete pos.m_value;
      return iterator(next);
    }

    iterator erase(iterator _beg, iterator _end) noexcept(std::is_trivially_destructible_v<_T>) {
      while (_beg != _end) {
        _beg = erase(_beg);
      }
      return _beg;
    }

    void clear() noexcept(std::is_trivially_copy_assignable_v<_T>&& std::is_trivially_constructible_v<_T>) {
      erase(begin(), end());
    }

    void fill(_T v) noexcept(std::is_trivially_copy_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) {
      for (auto it = begin(); it != end(); ++it) {
        *it = v;
      }
    }

    void reset() noexcept(std::is_trivially_copy_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) {
      fill(_T());
    }

  private:
    template <typename _U>
    void _copy_from_list(const list<_U>& other) {
      clear();

      _node* other_node = other.m_begin;

      for (_U& item : other) {
        _node* new_node = new _node;
        new_node->m_item = _T(item);
        _append_node(new_node);
      }
    }

    template <typename _U>
    void _move_from_list(list<_U>&& other) {
      m_begin = other.m_begin;
      m_end = other.m_end;
      m_size = other.m_size;
      other.m_begin = other._sentinel_node();
      other.m_end = nullptr;
      other.m_size = 0;
    }

    void _init_from_list(std::initializer_list<_T> list) {
      clear();

      for (_T item : list) {
        _node* new_node = new _node;
        new_node->m_item = item;
        _append_node(new_node);
      }
    }

    void _resize_impl(size_t new_size) {
      if (new_size == 0) {
        m_begin = _sentinel_node();
        m_end = nullptr;
        m_size = 0;
        return;
      }

      _node* node = m_begin;
      if (m_begin == _sentinel_node()) {
        m_begin = new _node;
        m_begin->m_prev = nullptr;
        m_begin->m_next = (_node*)&m_begin;
        m_begin->m_item = _T();
        m_end = m_begin;
        node = m_begin;
        new_size -= 1;
      }

      size_t i = 0;
      while (i < new_size) {
        if (node->m_next) {
          node = node->m_next;
          i += 1;
          continue;
        }
        _node* new_node = new _node;
        new_node->m_prev = node;
        node->m_next = new_node;
        new_node->m_item = _T();
        node = new_node;
        m_end = new_node;
      }

      if (node->m_next) {
        _node* rem = m_end;
        while (rem != node) {
          _node* prev = rem->m_prev;
          delete rem;
          rem = prev;
        }
      }

      m_size = new_size;
    }

    void _append_node(_node* n) {
      if (m_begin == _sentinel_node()) {
        m_begin = m_end = n;
        m_begin->m_next = _sentinel_node();
        m_begin->m_prev = nullptr;
        m_size = 1;
        return;
      }
      m_end->m_next = n;
      n->m_prev = m_end;
      n->m_next = _sentinel_node();
      m_end = n;
      m_size += 1;
    }

    void _prepend_node(_node* n) {
      if (m_begin == _sentinel_node()) {
        m_begin = m_end = n;
        m_begin->m_next = _sentinel_node();
        m_begin->m_prev = nullptr;
        m_size = 1;
        return;
      }
      m_begin->m_prev = n;
      n->m_next = m_begin;
      n->m_prev = nullptr;
      m_begin = n;
      m_size += 1;
    }

    void _insert_node(_node* n, _node* at) {
      if (at == _sentinel_node()) {
        _append_node(n);
        return;
      }

      if (at == m_begin) {
        _prepend_node(n);
        return;
      }

      _node* next = at;
      _node* prev = at->m_prev;

      n->m_prev = prev;
      n->m_next = next;
      prev->m_next = n;
      next->m_prev = n;

      m_size += 1;
    }

    void _remove_node(_node* at) {
      if (at == _sentinel_node()) {
        return;
      }
      _node* prev = at->m_prev;
      _node* next = at->m_next;
      prev->m_next = next;
      next->m_prev = prev;
      m_size -= 1;
    }

    _node* _sentinel_node() const noexcept { return (_node*)&m_begin; }

    _node* m_begin;
    _node* m_end;
    size_t m_size;
  };

}