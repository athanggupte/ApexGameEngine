#pragma once


/////////////////////////////////////////////////////////////////////////
static inline bool AllTrue() { return true; }

template <typename... BoolArgs>
static inline bool AllTrue(bool arg1, BoolArgs... otherArgs) { return arg1 & AllTrue(otherArgs...); }

template <size_t A, size_t B>
struct assert_equality
{
	static_assert(A == B, "NOT equal");
	static constexpr bool value = (A == B);
};

template <size_t A, size_t B>
inline constexpr bool assert_equality_v = assert_equality<A, B>::value;

/////////////////////////////////////////////////////////////////////////
template <typename T>
class ArrayRange
{
public:
	using value_type = T;
	using pointer_type = T*;
	using reference_type = T&;
	using const_pointer_type = const T*;
	using const_reference_type = const T&;

	ArrayRange();
	ArrayRange(const T* begin, const T* end);
	ArrayRange(const T* begin, size_t size);

	template <typename Ty>
	class ArrayRangeIterator
	{
	public:
		using self_type = ArrayRangeIterator<Ty>;
		using value_type = Ty;
		using pointer = Ty*;
		using reference = Ty&;
		//using iterator_category = std::forward_iterator_tag;
		//using difference_type = std::ptrdiff_t;

		ArrayRangeIterator& operator ++ ();
		ArrayRangeIterator& operator -- ();

		ArrayRangeIterator& operator ++ (int);
		ArrayRangeIterator& operator -- (int);

		bool operator == (const ArrayRangeIterator& other) const;
		bool operator != (const ArrayRangeIterator& other) const;

		Ty& operator * ();
		Ty* operator -> ();

	protected:
		ArrayRangeIterator(Ty* ptr);

	private:
		Ty* _Ptr;

		friend class ArrayRange;
	};

	using iterator = ArrayRangeIterator<T>;
	using const_iterator = ArrayRangeIterator<const T>;

	const_iterator begin();
	const_iterator end();

	const_reference_type operator [] (size_t index) const;

	size_t size();
	bool empty();

private:
	const T* const m_Begin;
	const T* const m_End;
};

#include "ArrayRangeImpl.hpp"

