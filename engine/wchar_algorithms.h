#pragma once
#include <cwchar>
#include <type_traits>

struct pwchar_hash
{
	size_t operator()(const wchar_t* const string) const
	{
		{
			uint32_t ind = 0, res = std::_FNV_offset_basis;
			wchar_t c = string[ind];
			const unsigned char* temp = reinterpret_cast<const unsigned char*>(string);
			while (c != L'\0')
			{
				res ^= static_cast<uint32_t>(temp[ind * 2]);
				res *= std::_FNV_prime;
				res ^= static_cast<uint32_t>(temp[ind * 2 + 1]);
				res *= std::_FNV_prime;

				c = string[++ind];
			}
			return res;
		}
	}
};

struct pwchar_comparator
{
	bool operator ()(const wchar_t* const lhs, const wchar_t* const rhs) const
	{
		return !(bool)wcscmp(lhs, rhs);
	}
};
