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
public:
	~defer_t()
	{
		for (auto it = m_lifo.crbegin(); it != m_lifo.crend(); it++)
		{
			if (*it) (*it)();
		}
	}

	void operator += (std::function<void()> &&f)
	{
		m_lifo.emplace_back(std::move(f));
	}

private:
	std::vector<std::function<void()>> m_lifo;
};
