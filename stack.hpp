#pragma once

#include <algorithm>
#include <format>
#include <initializer_list>
#include <iomanip>
#include <iostream>

#include "array.hpp"
#include "vector.hpp"

namespace JMK {

  template <typename _T, size_t _N>
  class stack {
  public:
    constexpr stack() = default;

    template <typename _U, size_t _S>
    constexpr stack(const JMK::stack<_U, _S>& other) noexcept(std::is_trivially_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>&&
      std::is_trivially_copy_assignable_v<_U>&&
      std::is_trivially_constructible_v<_U>) {
      for (size_t i = 0; i < std::min(_N, other.size()); ++i) {
        m_data[i] = other.m_data[i];
      }
      m_size = other.size();
    }

    template <typename _U, size_t _S>
    constexpr stack(JMK::stack<_U, _S>&& other) noexcept(std::is_trivially_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>&&
      std::is_trivially_move_assignable_v<_U>&&
      std::is_trivially_constructible_v<_U>) {
      for (size_t i = 0; i < std::min(_N, other.size()); ++i) {
        m_data[i] = std::move(other.m_data[i]);
      }
      m_size = other.size();
    }

    constexpr stack(std::initializer_list<_T> list) noexcept(std::is_trivially_copy_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) : m_data(list), m_size(list.size()) {}

    template <typename _U, size_t _S>
    constexpr stack(const JMK::array<_U, _S>& other) noexcept(std::is_trivially_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>&&
      std::is_trivially_copy_assignable_v<_U>&&
      std::is_trivially_constructible_v<_U>) : m_data(other), m_size(other.size()) {}

    template <typename _U, size_t _S>
    constexpr stack(JMK::array<_U, _S>&& other) noexcept(std::is_trivially_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>&&
      std::is_trivially_move_assignable_v<_U>&&
      std::is_trivially_constructible_v<_U>) : m_data(std::move(other)), m_size(other.size()) {}

    [[nodiscard]] constexpr size_t size() noexcept { return m_size; }
    [[nodiscard]] constexpr size_t max_size() noexcept { return m_data.max_size(); }

    [[nodiscard]] constexpr bool empty() noexcept { return m_data.empty(); }

    [[nodiscard]] _T& top() {
      assert(m_size != 0);
      return m_data.at(m_size - 1);
    }

    [[nodiscard]] const _T& top() const {
      assert(m_size != 0);
      return m_data.at(m_size - 1);
    }

    void push(const _T &item) {
      assert(m_size < m_data.max_size());
      m_data[m_size++] = item;
    }

    void push(_T&& item) {
      assert(m_size < m_data.max_size());
      m_data[m_size++] = item;
    }

    _T pop() {
      assert(m_size != 0);
      return m_data.at(--m_size);
    }

  private:
    JMK::array<_T, _N> m_data;
    size_t m_size = 0
  };


  template <typename _T>
  class stack<_T, 0> {
  public:
    constexpr stack() = default;

    template <typename _U, size_t _S>
    constexpr stack(const JMK::stack<_U, _S>& other) noexcept(std::is_trivially_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>&&
      std::is_trivially_copy_assignable_v<_U>&&
      std::is_trivially_constructible_v<_U>) {
      m_data.resize(other.size());
      for (size_t i = 0; i < other.size(); ++i) {
        m_data[i] = other.m_data[i];
      }
    }

    template <typename _U, size_t _S>
    constexpr stack(JMK::stack<_U, _S>&& other) noexcept(std::is_trivially_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>&&
      std::is_trivially_move_assignable_v<_U>&&
      std::is_trivially_constructible_v<_U>) {
      m_data.resize(other.size());
      for (size_t i = 0; i < other.size(); ++i) {
        m_data[i] = std::move(other.m_data[i]);
      }
    }

    constexpr stack(std::initializer_list<_T> list) noexcept(std::is_trivially_copy_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) : m_data(list) {}

    template <typename _U, size_t _S>
    constexpr stack(const JMK::array<_U, _S>& other) noexcept(std::is_trivially_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>&&
      std::is_trivially_copy_assignable_v<_U>&&
      std::is_trivially_constructible_v<_U>) : m_data(other) {}

    template <typename _U, size_t _S>
    constexpr stack(JMK::array<_U, _S>&& other) noexcept(std::is_trivially_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>&&
      std::is_trivially_move_assignable_v<_U>&&
      std::is_trivially_constructible_v<_U>) : m_data(std::move(other)) {}

    [[nodiscard]] constexpr size_t size() noexcept { return m_data.size(); }
    [[nodiscard]] constexpr size_t max_size() noexcept { return m_data.max_size(); }

    [[nodiscard]] constexpr bool empty() noexcept { return m_data.empty(); }

    [[nodiscard]] _T& top() noexcept {
      assert(size() != 0);
      return m_data.back();
    }

    [[nodiscard]] const _T& top() const noexcept {
      assert(size() != 0);
      return m_data.back();
    }

    void push(const _T& item) noexcept(std::is_trivially_copy_assignable_v<_T>&& std::is_trivially_constructible_v<_T>) {
      assert(size() < max_size());
      m_data.push_back(item);
    }

    void push(_T&& item) noexcept(std::is_trivially_copy_assignable_v<_T>&& std::is_trivially_constructible_v<_T>) {
      m_data.push_back(item);
    }

    _T pop() {
      assert(size() != 0);
      return m_data.pop_back();
    }

    void clear() noexcept(std::is_trivially_copy_assignable_v<_T>&& std::is_trivially_constructible_v<_T>) {
      m_data.clear();
    }

    // Friend declaration for the stream operator
    template <typename _Ty, size_t _S>
    friend std::ostream& operator<<(std::ostream& os, const JMK::stack<_Ty, _S>& obj);

  private:
    JMK::vector<_T> m_data;
  };

}

namespace JMK {

  template <typename _T, size_t _N>
  inline std::ostream& operator<<(std::ostream& os, const JMK::stack<_T, _N>& obj) {
    JMK::stack<_T, _N> copy(obj);

    os << "+" << std::string(13, ' ') << "+" << std::endl;
    while (!copy.empty()) {
      std::string fmt_item = std::format("{:^11}", copy.pop());
      os << "| " << fmt_item << " |" << std::endl;
      os << "+" << std::string(13, '-') << "+" << std::endl;
    }
    return os;
  }

}