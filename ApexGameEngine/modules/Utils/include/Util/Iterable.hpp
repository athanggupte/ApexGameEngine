#pragma once

template<typename T>
class Iterable
{
	using Iter = typename T::iterator;
	using ConstIter = typename T::const_iterator;
public:
	Iterable(T& container)
		: _begin(container.begin()), _end(container.end())
	{
	}

	Iterable(Iter& begin, Iter& end)
		: _begin(begin), _end(end)
	{
	}

	Iterable(const Iter& begin, const Iter& end)
		: _begin(begin), _end(end)
	{
	}

	Iterable(const ConstIter& begin, const ConstIter& end)
            : _begin(begin), _end(end)
	{
	}

	Iter begin()
	{
		return _begin;
	}

	Iter end()
	{
		return _end;
	}

	ConstIter begin() const
	{
		return _begin;
	}

	ConstIter end() const
	{
		return _end;
	}

private:
	Iter _begin, _end;
};
