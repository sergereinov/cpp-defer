#pragma once

/*
Author: Serge Reinov
Git source: https://github.com/sergereinov/cpp-defer
License: MIT
*/

#include <vector>
#include <functional>

class defer_t
{
	std::vector<std::function<void()>> m_lifo;

public:
	defer_t() = default;
	defer_t(defer_t&&) = default;

	defer_t(const defer_t&) = delete;
	defer_t& operator=(const defer_t&) = delete;

	void operator += (std::function<void()>&& f)
	{
		m_lifo.emplace_back(std::forward<std::function<void()>>(f));
	}

	~defer_t()
	{
		for (auto it = m_lifo.crbegin(); it != m_lifo.crend(); it++)
		{
			if (*it) (*it)();
		}
	}
};
