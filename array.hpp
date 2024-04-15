#pragma once

#include <cassert>
#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <iterator>

namespace JMK {

  template <typename _T, size_t _N>
  class array {
  public:
    static_assert(_N > 0, "JMK::array type cannot be empty");

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

      constexpr iterator& operator =(const iterator& other) noexcept = default;
      constexpr iterator& operator =(iterator&& other) noexcept = default;

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
      constexpr const_iterator(const iterator& other) noexcept : m_data(std::addressof(*other)) {}
      constexpr const_iterator(iterator&& other) noexcept : m_data(std::addressof(*other)) {}
      constexpr const_iterator(pointer value) noexcept : m_value(value) {}

      [[nodiscard]] constexpr const_iterator operator+(difference_type i) const noexcept {
        const_iterator copy = *this;
        copy.m_value += i;
        return *copy;
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
        return *copy;
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

      constexpr const_iterator& operator=(const const_iterator& other) noexcept = default;
      constexpr const_iterator& operator=(const_iterator&& other) noexcept = default;
      constexpr const_iterator& operator=(const iterator& other) noexcept { return const_iterator(other); }
      constexpr const_iterator& operator=(iterator&& other) noexcept { return const_iterator(std::move(other)); }

      [[nodiscard]] constexpr bool operator==(const const_iterator& other) const& = default;
      [[nodiscard]] constexpr std::strong_ordering operator<=>(const const_iterator& other) const& = default;

    private:
      pointer m_value;
    };

    using reverse_iterator = std::reverse_iterator<iterator>;
    using reverse_const_iterator = std::reverse_iterator<const_iterator>;

    constexpr array() noexcept(std::is_trivially_constructible_v<_T>) : array(_T()) {}

    template <typename _U, size_t _S>
    constexpr array(const array<_U, _S>& other) noexcept(std::is_trivially_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>&&
      std::is_trivially_copy_assignable_v<_U>&&
      std::is_trivially_constructible_v<_U>) {
      for (size_t i = 0; i < std::min(_N, _S); ++i) {
        m_data[i] = other.m_data[i];
      }
    }

    template <typename _U, size_t _S>
    constexpr array(array<_U, _S>&& other) noexcept(std::is_trivially_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>&&
      std::is_trivially_move_assignable_v<_U>&&
      std::is_trivially_constructible_v<_U>) {
      for (size_t i = 0; i < std::min(_N, _S); ++i) {
        m_data[i] = std::move(other.m_data[i]);
      }
    }

    constexpr array(_T _fill) noexcept(std::is_trivially_copy_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) {
      fill(_fill);
    }

    constexpr array(std::initializer_list<_T> list) noexcept(std::is_trivially_copy_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) : m_data() {
      for (size_t i = 0; i < list.size(); ++i) {
        m_data[i] = *(list.begin() + i);
      }
    }

    [[nodiscard]] constexpr size_t size() const noexcept { return _N; }
    [[nodiscard]] constexpr size_t max_size() const noexcept { return _N; }

    [[nodiscard]] constexpr bool empty() const noexcept { return _N == 0; }

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

    [[nodiscard]] constexpr iterator begin() noexcept { return &m_data[0]; }
    [[nodiscard]] constexpr iterator end() noexcept { return &m_data[_N - 1]; }
    [[nodiscard]] constexpr const_iterator begin() const noexcept { return &m_data[0]; }
    [[nodiscard]] constexpr const_iterator end() const noexcept { return &m_data[_N - 1]; }

    [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return &m_data[0]; }
    [[nodiscard]] constexpr const_iterator cend() const noexcept { return &m_data[_N - 1]; }

    [[nodiscard]] constexpr reverse_iterator rbegin() noexcept { return &m_data[_N - 1]; }
    [[nodiscard]] constexpr reverse_iterator rend() noexcept { return &m_data[0]; }

    [[nodiscard]] constexpr reverse_iterator crbegin() const noexcept { return &m_data[_N - 1]; }
    [[nodiscard]] constexpr reverse_iterator crend() const noexcept { return &m_data[0]; }

    [[nodiscard]] constexpr _T& front() { return at(0); }
    [[nodiscard]] constexpr const _T& front() const { return at(0); }

    [[nodiscard]] constexpr _T& back() { return at(_N - 1); }
    [[nodiscard]] constexpr const _T& back() const { return at(_N - 1); }

    constexpr void fill(_T v) noexcept(std::is_trivially_copy_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) {
      for (size_t i = 0; i < _N; ++i) {
        m_data[i] = v;
      }
    }

    constexpr void reset() noexcept(std::is_trivially_copy_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) {
      fill(_T());
    }

    template<typename _U, size_t _S>
    JMK::array<_T, _N>& operator =(const JMK::array<_U, _S>& other) noexcept(std::is_trivially_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>&&
      std::is_trivially_copy_assignable_v<_U>&&
      std::is_trivially_constructible_v<_U>) {
      for (size_t i = 0; i < std::min(_N, _S); ++i) {
        m_data[i] = other.m_data[i];
      }
    }

    template<typename _U, size_t _S>
    JMK::array<_T, _N>& operator =(JMK::array<_U, _S>&& other) noexcept(std::is_trivially_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>&&
      std::is_trivially_move_assignable_v<_U>&&
      std::is_trivially_constructible_v<_U>) {
      for (size_t i = 0; i < std::min(_N, _S); ++i) {
        m_data[i] = std::move(other.m_data[i]);
      }
    }

    [[nodiscard]] explicit operator std::string() const noexcept {
      std::string out;
      for (size_t i = 0; i < _N; ++i) {
        if (m_data[i] == '\0') {
          break;
        }
        out += m_data[i];
      }
      return out;
    }

    // Friend declaration for the stream operator
    template <typename _U, size_t _S>
    friend std::ostream& operator<<(std::ostream& os, const array<_U, _S>& obj);

  private:
    _T m_data[_N];
  };

}

namespace JMK {

  template <typename _T, size_t _N>
  inline std::ostream& operator<<(std::ostream& os, const JMK::array<_T, _N>& arr) {
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