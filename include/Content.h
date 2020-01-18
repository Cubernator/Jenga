#pragma once

#include <typeindex>
#include <unordered_map>
#include <string>
#include <boost/any.hpp>

class ContentPool
{
private:
	std::unordered_map<std::wstring, boost::any> m_pool;

public:
	ContentPool();

	template<typename T>
	void put(const std::wstring& name, const T& v)
	{
		m_pool[name] = v;
	}

	template<typename T>
	bool get(const std::wstring& name, T& v) const
	{
		if (m_pool.count(name)) {
			try {
				v = boost::any_cast<const T>(m_pool.at(name));
				return true;
			} catch (boost::bad_any_cast&) { }
		}
		return false;
	}
};

extern ContentPool * content;