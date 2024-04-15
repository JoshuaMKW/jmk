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
  class queue {
  public:
    queue() = default;

    queue(const JMK::queue<_T, _N>& other) noexcept(std::is_trivially_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) {
      for (size_t i = 0; i < std::min(_N, other.size()); ++i) {
        m_data[i] = other.m_data[i];
      }
      m_size = other.size();
    }

    queue(JMK::queue<_T, _N>&& other) noexcept(std::is_trivially_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) {
      for (size_t i = 0; i < std::min(_N, other.size()); ++i) {
        m_data[i] = std::move(other.m_data[i]);
      }
      m_size = other.size();
    }

    queue(std::initializer_list<_T> list) noexcept(std::is_trivially_copy_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) : m_data(list), m_size(list.size()) {}

    queue(const JMK::array<_T, _N>& other) noexcept(std::is_trivially_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) : m_data(other), m_size(other.size()) {}

    queue(JMK::array<_T, _N>&& other) noexcept(std::is_trivially_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) : m_data(std::move(other)), m_size(other.size()) {}

    [[nodiscard]] size_t size() const noexcept { return m_size; }
    [[nodiscard]] size_t max_size() const noexcept { return m_data.max_size(); }

    [[nodiscard]] bool empty() const noexcept { return m_data.empty(); }

    [[nodiscard]] _T& front() {
      assert(m_size != 0);
      return m_data.at(m_index);
    }

    [[nodiscard]] const _T& front() const {
      assert(m_size != 0);
      return m_data.at(m_index);
    }

    void enqueue(const _T& item) {
      assert(m_size < m_data.max_size());
      _cycle_storage(item);
    }

    void enqueue(_T&& item) {
      assert(m_size < m_data.max_size());
      _cycle_storage(std::move(item));
    }

    _T dequeue() {
      assert(m_size != 0);
      _T res = m_data.at(m_index);
      _shift_entry();
      return res;
    }

  private:
    void _cycle_storage(_T&& data) {
      size_t cyclic_capacity = m_data.capacity();
      assert(m_size < m_data.size());
      m_data[(m_index + m_size) % cyclic_capacity] = data;
      m_size += 1;
    }

    inline void _shift_entry() {
      m_index = (m_index + 1) % m_data.size();
      m_size -= 1;
    }

    JMK::array<_T, _N> m_data;
    size_t m_size = 0;
    size_t m_index = 0;
  };


  template <typename _T>
  class queue<_T, 0> {
  public:
    queue() = default;

    queue(const JMK::queue<_T, 0>& other) noexcept(
      std::is_trivially_constructible_v<_T>) {
      m_data.resize(other.size());
      for (size_t i = 0; i < other.size(); ++i) {
        m_data[i] = other.m_data[i];
      }
    }

    queue(JMK::queue<_T, 0>&& other) noexcept(
      std::is_trivially_constructible_v<_T>) {
      m_data.resize(other.size());
      for (size_t i = 0; i < other.size(); ++i) {
        m_data[i] = std::move(other.at(i));
      }
    }

    queue(std::initializer_list<_T> list) noexcept(std::is_trivially_copy_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) : m_data(list) {}

    queue(const JMK::array<_T, 0>& other) noexcept(std::is_trivially_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) : m_data(other) {}

    queue(JMK::array<_T, 0>&& other) noexcept(std::is_trivially_assignable_v<_T>&&
      std::is_trivially_constructible_v<_T>) : m_data(std::move(other)) {}

    [[nodiscard]] size_t size() const noexcept { return m_size; }
    [[nodiscard]] size_t max_size() const noexcept { return m_data.max_size(); }

    [[nodiscard]] bool empty() const noexcept { return m_size == 0; }

    [[nodiscard]] _T& front() noexcept {
      assert(size() != 0);
      return m_data[m_index];
    }

    [[nodiscard]] const _T& front() const noexcept {
      assert(size() != 0);
      return m_data[m_index];
    }

    void enqueue(const _T& item) noexcept(std::is_trivially_copy_assignable_v<_T>&& std::is_trivially_constructible_v<_T>) {
      assert(m_size < max_size());
      _cycle_storage(item);
    }

    void enqueue(_T&& item) noexcept(std::is_trivially_copy_assignable_v<_T>&& std::is_trivially_constructible_v<_T>) {
      m_data.push_back(item);
      _cycle_storage(std::move(item));
    }

    _T dequeue() {
      assert(m_size != 0);
      _T res = m_data.at(m_index);
      _shift_entry();
      return res;
    }

    void clear() noexcept(std::is_trivially_copy_assignable_v<_T>&& std::is_trivially_constructible_v<_T>) {
      m_data.clear();
      m_size = 0;
    }

    // Friend declaration for the stream operator
    template <typename _Ty, size_t _S>
    friend std::ostream& operator<<(std::ostream& os, const JMK::queue<_Ty, _S>& obj);

  private:
    void _cycle_storage(const _T& data) {
      size_t cyclic_capacity = m_data.size();
      if (m_size >= cyclic_capacity) {
        _realloc_storage();
        cyclic_capacity = m_data.size();
      }
      m_data[(m_index + m_size) % cyclic_capacity] = data;
      m_size += 1;
    }

    void _cycle_storage(_T&& data) {
      size_t cyclic_capacity = m_data.size();
      if (m_size >= cyclic_capacity) {
        _realloc_storage();
        cyclic_capacity = m_data.size();
      }
      m_data[(m_index + m_size) % cyclic_capacity] = data;
      m_size += 1;
    }

    void _realloc_storage() {
      if (m_index > 0) {
        JMK::vector<_T> new_storage;
        new_storage.resize(static_cast<size_t>(m_data.capacity() * 1.5));

        // Copy the shifted beginning to true beginning
        std::memcpy(&new_storage[0], &m_data[m_index], m_data.size() - m_index);

        // Copy the shifted end to true end
        std::memcpy(&new_storage[m_data.size() - m_index], &m_data[0], m_index);

        m_data = std::move(new_storage);
        return;
      }

      m_data.resize(static_cast<size_t>(m_data.capacity() * 1.5));
    }

    inline void _shift_entry() {
      m_index = (m_index + 1) % m_data.size();
      m_size -= 1;
    }

    JMK::vector<_T> m_data;
    size_t m_index = 0;
    size_t m_size = 0;
  };

}

namespace JMK {

  template <typename _T, size_t _N>
  inline std::ostream& operator<<(std::ostream& os, const JMK::queue<_T, _N>& obj) {
    JMK::queue<_T, _N> copy(obj);

    os << "+" << std::string(13, ' ') << "+" << std::endl;
    while (!copy.empty()) {
      std::string fmt_item = std::format("{:^11}", copy.dequeue());
      os << "| " << fmt_item << " |" << std::endl;
      os << "+" << std::string(13, '-') << "+" << std::endl;
    }
    return os;
  }

}