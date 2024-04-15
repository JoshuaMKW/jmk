#pragma once

#include <cassert>
#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <iterator>

namespace JMK {

  template <typename _T>
  class vector {
  public:
    class iterator {
    public:
      using iterator_category = std::contiguous_iterator_tag;
      using value_type = std::remove_reference_t<_T>;
      using difference_type = std::ptrdiff_t;
      using pointer = value_type*;
      using reference = value_type&;

      constexpr iterator() = delete;
      constexpr iterator(const iterator&) noexcept = default;
      constexpr iterator(iterator&&) noexcept = default;
      constexpr iterator(pointer value) noexcept : m_value(value) {}

      [[nodiscard]] constexpr iterator operator+(difference_type i) const noexcept {
        iterator copy = *this;
        copy.m_value += i;
        return *copy;
      }

      constexpr iterator& operator++() noexcept {
        ++m_value;
        return *this;
      }

      constexpr iterator& operator++(int) noexcept {
        iterator copy = *this;
        ++m_value;
        return copy;
      }

      [[nodiscard]] constexpr iterator operator-(difference_type i) const noexcept {
        iterator copy = *this;
        copy.m_value -= i;
        return *copy;
      }

      [[nodiscard]] constexpr difference_type operator-(const iterator& other) const noexcept {
        return static_cast<difference_type>(m_value - other.m_value);
      }

      constexpr iterator& operator--() noexcept {
        --m_value;
        return *this;
      }

      constexpr iterator& operator--(int) noexcept {
        iterator copy = *this;
        --m_value;
        return copy;
      }

      [[nodiscard]] constexpr reference operator*() noexcept {
        return *m_value;
      }

      [[nodiscard]] constexpr pointer operator->() noexcept {
        return m_value;
      }

      constexpr iterator& operator=(const iterator&) = default;

      [[nodiscard]] constexpr bool operator==(const iterator& other) const& = default;
      [[nodiscard]] constexpr std::strong_ordering operator<=>(const iterator& other) const& = default;

    private:
      pointer m_value;
    };

    class const_iterator {
    public:
      using iterator_category = std::contiguous_iterator_tag;
      using value_type = std::remove_reference_t<const _T>;
      using difference_type = std::ptrdiff_t;
      using pointer = value_type*;
      using reference = value_type&;

      constexpr const_iterator() = delete;
      constexpr const_iterator(const const_iterator&) noexcept = default;
      constexpr const_iterator(const_iterator&&) noexcept = default;
      constexpr const_iterator(pointer value) noexcept : m_value(value) {}

      [[nodiscard]] constexpr const_iterator operator+(difference_type i) const noexcept {
        const_iterator copy = *this;
        copy.m_value += i;
        return copy;
      }

      constexpr const_iterator& operator++() noexcept {
        ++m_value;
        return *this;
      }

      constexpr const_iterator& operator++(int) noexcept {
        const_iterator copy = *this;
        ++m_value;
        return copy;
      }

      [[nodiscard]] constexpr const_iterator operator-(difference_type i) const noexcept {
        const_iterator copy = *this;
        copy.m_value -= i;
        return copy;
      }

      [[nodiscard]] constexpr difference_type operator-(const const_iterator& other) const noexcept {
        return static_cast<difference_type>(m_value - other.m_value);
      }

      constexpr const_iterator& operator--() noexcept {
        --m_value;
        return *this;
      }

      constexpr const_iterator& operator--(int) noexcept {
        const_iterator copy = *this;
        --m_value;
        return copy;
      }

      [[nodiscard]] constexpr reference operator*() noexcept {
        return *m_value;
      }

      [[nodiscard]] constexpr pointer operator->() noexcept {
        return m_value;
      }

      constexpr const_iterator& operator=(const const_iterator&) = default;

      [[nodiscard]] constexpr bool operator==(const const_iterator& other) const& = default;
      [[nodiscard]] constexpr std::strong_ordering operator<=>(const const_iterator& other) const& = default;

    private:
      pointer m_value;
    };

    using reverse_iterator = std::reverse_iterator<iterator>;
    using reverse_const_iterator = std::reverse_iterator<const_iterator>;

    constexpr vector() noexcept(std::is_trivially_constructible_v<_T>) : vector(_T(), 0) {}

    template <typename _U>
    constexpr vector(const vector<_U>& other) noexcept(std::is_trivially_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>&&
      std::is_trivially_copy_assignable_v<_U>&&
      std::is_trivially_constructible_v<_U>) : m_growth_factor(1.5f) {
      resize(other.size());
      for (size_t i = 0; i < other.size(); ++i) {
        m_data[i] = other.m_data[i];
      }
    }

    template <typename _U>
    constexpr vector(vector<_U>&& other) noexcept(std::is_trivially_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>&&
      std::is_trivially_move_assignable_v<_U>&&
      std::is_trivially_constructible_v<_U>) : m_growth_factor(1.5f) {
      resize(other.size());
      for (size_t i = 0; i < other.size(); ++i) {
        m_data[i] = std::move(other.m_data[i]);
      }
    }

    constexpr vector(_T _fill, size_t size) noexcept(std::is_trivially_copy_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) : m_growth_factor(1.5f) {
      resize(size);
      fill(_fill);
    }

    constexpr vector(std::initializer_list<_T> list) noexcept(std::is_trivially_copy_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) : m_growth_factor(1.5f) {
      resize(list.size());
      for (size_t i = 0; i < list.size(); ++i) {
        m_data[i] = *(list.begin() + i);
      }
    }

    [[nodiscard]] constexpr size_t size() const noexcept { return m_size; }
    [[nodiscard]] constexpr size_t max_size() const noexcept { return std::numeric_limits<size_t>::max(); }
    [[nodiscard]] constexpr size_t capacity() const noexcept { return m_capacity; }

    [[nodiscard]] constexpr bool empty() const noexcept { return m_size == 0; }

    [[nodiscard]] _T& at(size_t index) {
      assert(index < size());
      return m_data[index];
    }

    [[nodiscard]] const _T& at(size_t index) const {
      assert(index < size());
      return m_data[index];
    }

    [[nodiscard]] constexpr _T& operator[](size_t index) noexcept {
      return m_data[index];
    }

    [[nodiscard]] constexpr const _T& operator[](size_t index) const noexcept {
      return m_data[index];
    }

    [[nodiscard]] constexpr _T& front() noexcept { return m_data[0]; }
    [[nodiscard]] constexpr const _T& front() const noexcept { return m_data[0]; }

    [[nodiscard]] constexpr _T& back() noexcept { return m_data[m_size - 1]; }
    [[nodiscard]] constexpr const _T& back() const noexcept { return m_data[m_size - 1]; }

    [[nodiscard]] constexpr _T* data() noexcept { return m_data; }
    [[nodiscard]] constexpr const _T* data() const noexcept { return m_data; }

    void resize(size_t new_size) {
      _resize_impl(new_size);
    }

    void reserve(size_t new_capacity) {
      _reserve_impl(new_capacity);
    }

    void shrink_to_fit() {
      _strict_resize_impl(m_size);
    }

    void set_growth_factor(float factor) {
      m_growth_factor = std::max(factor, 1.1f);
    }

    [[nodiscard]] constexpr iterator begin() noexcept { return &m_data[0]; }
    [[nodiscard]] constexpr iterator end() noexcept { return &m_data[size()]; }
    [[nodiscard]] constexpr const_iterator begin() const noexcept { return &m_data[0]; }
    [[nodiscard]] constexpr const_iterator end() const noexcept { return &m_data[size()]; }

    [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return &m_data[0]; }
    [[nodiscard]] constexpr const_iterator cend() const noexcept { return &m_data[size()]; }

    [[nodiscard]] constexpr reverse_iterator rbegin() noexcept { return &m_data[size()]; }
    [[nodiscard]] constexpr reverse_iterator rend() noexcept { return &m_data[0]; }

    [[nodiscard]] constexpr reverse_iterator crbegin() const noexcept { return &m_data[size()]; }
    [[nodiscard]] constexpr reverse_iterator crend() const noexcept { return &m_data[0]; }

    template <typename ... _Args>
    constexpr void emplace_back(_Args&& ... args) noexcept(std::is_trivially_constructible_v<_T>) {
      if (m_size >= m_capacity) {
        _reserve_impl(static_cast<size_t>(m_capacity * m_growth_factor));
      }
      _construct_at(&m_data[m_size], std::forward<_Args>(args)...);
      m_size += 1;
    }

    constexpr void push_back(const _T& value) noexcept(std::is_trivially_copy_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) {
      if (m_size >= m_capacity) {
        _reserve_impl(static_cast<size_t>(m_capacity * m_growth_factor));
      }
      _construct_at(&m_data[m_size], value);
      m_size += 1;
    }

    constexpr _T pop_back() noexcept(std::is_trivially_copy_assignable_v<_T>&& std::is_trivially_constructible_v<_T>) {
      assert(m_size > 0);
      return m_data[--m_size];
    }

    template <typename ... _Args>
    constexpr iterator emplace(const_iterator pos, _Args&& ... args) noexcept(std::is_trivially_copy_assignable_v<_T>&&
      std::is_constructible_v<_T>) {
      if (m_size == m_capacity) {
        _reserve_impl(static_cast<size_t>(m_capacity * m_growth_factor));
      }
      // Shift all future elements right
      for (auto it = end(); it != pos; --it) {
        *it = *(it - 1);
      }
      // Insert
      _construct_at(std::addressof(*pos), std::forward<_Args>(args)...);
      ++m_size;
      return pos;
    }

    constexpr iterator insert(const_iterator pos, const _T& value) noexcept(std::is_trivially_copy_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) {
      if (m_size == m_capacity) {
        _reserve_impl(static_cast<size_t>(m_capacity * m_growth_factor));
      }
      // Shift all future elements right
      for (auto it = end(); it != pos; --it) {
        *it = *(it - 1);
      }
      // Insert
      _construct_at(std::addressof(*pos), value);
      ++m_size;
      return pos;
    }

    constexpr iterator erase(iterator pos) noexcept(std::is_trivially_destructible_v<_T>) {
      if constexpr (std::is_pointer_v<_T>) {
        delete (*pos);
      }
      else if (std::is_destructible_v<_T>) {
        (*pos).~_T();
      }
      for (auto it = pos + 1; it != end(); ++it) {
        *(it - 1) = *it;
      }
      --m_size;
      return pos;
    }

    constexpr iterator erase(iterator _beg, iterator _end) noexcept(std::is_trivially_destructible_v<_T>) {
      for (auto it = _beg + 1; it != _end; ++it) {
        if constexpr (std::is_pointer_v<_T>) {
          delete (*it);
        }
        else if (std::is_destructible_v<_T>) {
          (*it).~_T();
        }
      }
      for (auto it = _end; it != end(); ++it) {
        *(it - (_end - _beg)) = *it;
      }
      --m_size;
      return _beg;
    }

    constexpr void clear() noexcept(std::is_trivially_copy_assignable_v<_T>&& std::is_trivially_constructible_v<_T>) {
      erase(begin(), end());
    }

    constexpr void fill(_T v) noexcept(std::is_trivially_copy_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) {
      for (size_t i = 0; i < size(); ++i) {
        m_data[i] = v;
      }
    }

    constexpr void reset() noexcept(std::is_trivially_copy_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) {
      fill(_T());
    }

    [[nodiscard]] explicit operator std::string() const noexcept {
      std::string out;
      for (size_t i = 0; i < size(); ++i) {
        if (m_data[i] == '\0') {
          break;
        }
        out += m_data[i];
      }
      return out;
    }

    // Friend declaration for the stream operator
    template <typename _Ty>
    friend std::ostream& operator<<(std::ostream& os, const vector<_Ty>& obj);

  protected:
    constexpr void _strict_resize_impl(size_t new_size) noexcept(std::is_trivially_copy_assignable_v<_T>&& std::is_trivially_constructible_v<_T>) {
      _reserve_impl(new_size);
      for (size_t i = m_size; i < new_size; ++i) {
        m_data[i] = _T();
      }
      m_size = new_size;
    }

    constexpr void _resize_impl(size_t new_size) noexcept(std::is_trivially_copy_assignable_v<_T>&& std::is_trivially_constructible_v<_T>) {
      size_t new_capacity = std::max<size_t>(m_capacity, 4);
      while (new_capacity < new_size) {
        new_capacity = static_cast<size_t>(new_capacity * m_growth_factor);
      }
      _reserve_impl(new_capacity);
      for (size_t i = m_size; i < new_size; ++i) {
        m_data[i] = _T();
      }
      m_size = new_size;
    }

    constexpr void _reserve_impl(size_t new_capacity) {
      new_capacity = std::max<size_t>(new_capacity, 4);
      if (new_capacity <= m_capacity) {
        return;
      }

      _T* new_data = new _T[new_capacity];
      for (size_t i = 0; i < m_size; ++i) {
        new_data[i] = m_data[i];
      }
      delete[] m_data;
      m_data = new_data;
      m_capacity = new_capacity;
    }

    template <typename ... _Args>
    constexpr void _construct_at(void* ptr, _Args&&... value) noexcept(std::is_trivially_constructible_v<_T>) {
      new(ptr) _T(std::forward<_Args>(value)...);
    }

  private:
    _T* m_data;
    size_t m_size;
    size_t m_capacity;
    float m_growth_factor;
  };

}

namespace JMK {

  template <typename _T>
  inline std::ostream& operator<<(std::ostream& os, const JMK::vector<_T>& arr) {
    os << "[";
    for (size_t i = 0; i < arr.size(); ++i) {
      os << arr[i];
      if (i < arr.size() - 1) {
        os << ", ";
      }
    }
    os << "]";
    return os;
  }

}