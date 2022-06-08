///////////////////////////////////////////////////////////////////////////////
// disks.hpp
//
// Definitions for two algorithms that each solve the alternating disks
// problem.
//
// As provided, this header has four functions marked with TODO comments.
// You need to write in your own implementation of these functions.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

// TODO
#include <functional>
#include <iostream>

enum disk_color
{
  DISK_DARK,
  DISK_LIGHT
};

class disk_state
{
private:
  std::vector<disk_color> _colors;

public:
  disk_state(size_t light_count)
      : _colors(light_count * 2, DISK_DARK)
  {

    assert(light_count > 0);

    for (size_t i = 0; i < _colors.size(); i += 2)
    {
      _colors[i] = DISK_LIGHT;
    }
  }

  bool operator==(const disk_state &rhs) const
  {
    return std::equal(_colors.begin(), _colors.end(), rhs._colors.begin());
  }

  size_t total_count() const
  {
    return _colors.size();
  }

  size_t light_count() const
  {
    return total_count() / 2;
  }

  size_t dark_count() const
  {
    return light_count();
  }

  bool is_index(size_t i) const
  {
    return (i < total_count());
  }

  disk_color get(size_t index) const
  {
    assert(is_index(index));
    return _colors[index];
  }

  void swap(size_t left_index)
  {
    assert(is_index(left_index));
    auto right_index = left_index + 1;
    assert(is_index(right_index));
    std::swap(_colors[left_index], _colors[right_index]);
  }

  std::string to_string() const
  {
    std::stringstream ss;
    bool first = true;
    for (auto color : _colors)
    {
      if (!first)
      {
        ss << " ";
      }

      if (color == DISK_LIGHT)
      {
        ss << "L";
      }
      else
      {
        ss << "D";
      }

      first = false;
    }
    return ss.str();
  }

  // Implementation of initialization check
  bool is_initialized() const
  {
    for (size_t i = 0; i < _colors.size(); i += 2)
    {
      if (_colors[i] != DISK_LIGHT)
      {
        return false;
      }
    }
    return true;
  }

  // Implementation of check for dark on lhs of vector _colors and light on rhs
  // as divided by variable halfPoint
  bool is_sorted() const
  {
    size_t halfPoint = total_count() / 2;

    for (size_t i = 0; i < halfPoint; i++)
    {
      if (_colors[i] != DISK_DARK)
      {
        return false;
      }
    }

    for (size_t j = total_count() - 1; j > halfPoint; j--)
    {
      if (_colors[j] != DISK_LIGHT)
      {
        return false;
      }
    }
    return true;
  }
};

// Data structure for the output of the alternating disks problem. That
// includes both the final disk_state, as well as a count of the number
// of swaps performed.
class sorted_disks
{
private:
  disk_state _after;
  unsigned _swap_count;

public:
  sorted_disks(const disk_state &after, unsigned swap_count)
      : _after(after), _swap_count(swap_count) {}

  sorted_disks(disk_state &&after, unsigned swap_count)
      : _after(after), _swap_count(swap_count) {}

  const disk_state &after() const
  {
    return _after;
  }

  unsigned swap_count() const
  {
    return _swap_count;
  }
};

// Implementation of the alternate sort algorithm which sorts with a nested
// for loop with two if statements checking if a swap is necessary. The amount
// of passes necessary is determined by the amount of paris or n.
sorted_disks sort_alternate(const disk_state &before)
{
  auto after(before);

  unsigned int swapCount = 0;
  size_t halfCount = after.total_count() / 2;
  size_t loopCounter = 0;

  for (size_t j = 0; j < halfCount; j++)
  {

    for (size_t i = loopCounter % 2; i < after.total_count() - 1; i += 2)
    {
      if (after.get(i) == DISK_DARK && after.get(i + 1) != DISK_DARK)
      {
        after.swap(i);
        swapCount++;
      }

      if (after.get(i) != DISK_DARK && after.get(i + 1) == DISK_DARK)
      {
        after.swap(i);
        swapCount++;
      }
    }
    loopCounter++;
  }
  return sorted_disks(after, swapCount);
}

// Implementation of the lawnmower sort algorithm which uses a for loop with
// two separate for loops to ensure a "back and forth" swap check occurs
// with each pass. The amount of passes is determined by the amount of pairs/2
// or n/2.
sorted_disks sort_lawnmower(const disk_state &before)
{
  auto after(before);
  unsigned int swapCount = 0;
  size_t halfCount = after.total_count() / 2;
  size_t loopCounter = 0;

  if (halfCount % 2 == 0)
  {
    loopCounter = halfCount / 2;
  }
  else
  {
    loopCounter = halfCount / 2 + 1;
  }

  for (size_t i = 0; i < loopCounter; i++)
  {
    for (size_t j = 0; j < after.total_count() - 1; j += 2)
    {
      if (after.get(j + 1) == DISK_DARK && after.get(j) != DISK_DARK)
      {
        after.swap(j);
        swapCount++;
      }
    }

    // backward moving for loop starting at index max -1
    for (size_t k = after.total_count() - 2; k > 0; k -= 2)
    {

      if (after.get(k) == DISK_DARK && after.get(k - 1) != DISK_DARK)
      {
        after.swap(k - 1);
        swapCount++;
      }
    }
  }
  return sorted_disks(after, swapCount);
}
