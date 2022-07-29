#include "Utils.h"
#pragma once

template<typename T>
template<typename Ty>
inline typename ArrayRange<T>::ArrayRangeIterator<Ty>& ArrayRange<T>::ArrayRangeIterator<Ty>::operator ++ ()
{
	++_Ptr;
	return *this;
}

template<typename T>
template<typename Ty>
inline typename ArrayRange<T>::ArrayRangeIterator<Ty>& ArrayRange<T>::ArrayRangeIterator<Ty>::operator -- ()
{
	--_Ptr;
	return *this;
}

template<typename T>
template<typename Ty>
inline typename ArrayRange<T>::ArrayRangeIterator<Ty>& ArrayRange<T>::ArrayRangeIterator<Ty>::operator ++ (int)
{
	auto& _this = *this;
	++_Ptr;
	return _this;
}

template<typename T>
template<typename Ty>
inline typename ArrayRange<T>::ArrayRangeIterator<Ty>& ArrayRange<T>::ArrayRangeIterator<Ty>::operator -- (int)
{
	auto& _this = *this;
	--_Ptr;
	return _this;
}

template<typename T>
template<typename Ty>
inline bool ArrayRange<T>::ArrayRangeIterator<Ty>::operator == (const ArrayRange<T>::ArrayRangeIterator<Ty>& other) const
{
	return _Ptr == other._Ptr;
}

template<typename T>
template<typename Ty>
inline bool ArrayRange<T>::ArrayRangeIterator<Ty>::operator != (const ArrayRange<T>::ArrayRangeIterator<Ty>& other) const
{
	return _Ptr != other._Ptr;
}

template<typename T>
template<typename Ty>
inline Ty& ArrayRange<T>::ArrayRangeIterator<Ty>::operator*()
{
	return *_Ptr;
}

template<typename T>
template<typename Ty>
inline Ty* ArrayRange<T>::ArrayRangeIterator<Ty>::operator->()
{
	return _Ptr;
}

template<typename T>
template<typename Ty>
inline ArrayRange<T>::ArrayRangeIterator<Ty>::ArrayRangeIterator(Ty* ptr)
	: _Ptr(ptr)
{
}

template<typename T>
inline ArrayRange<T>::ArrayRange()
	: m_Begin(nullptr), m_End(nullptr)
{
}

template<typename T>
inline ArrayRange<T>::ArrayRange(const T* begin, const T* end)
	: m_Begin(begin), m_End(end)
{
}

template<typename T>
inline ArrayRange<T>::ArrayRange(const T* begin, size_t size)
	: m_Begin(begin), m_End(begin + size)
{
}

template<typename T>
inline typename ArrayRange<T>::const_iterator ArrayRange<T>::begin()
{
	return { m_Begin };
}

template<typename T>
inline typename ArrayRange<T>::const_iterator ArrayRange<T>::end()
{
	return { m_End };
}

template<typename T>
inline typename ArrayRange<T>::const_reference_type ArrayRange<T>::operator[](size_t index) const
{
	return m_Begin[index];
}

template<typename T>
inline size_t ArrayRange<T>::size()
{
	return static_cast<size_t>(m_End - m_Begin);
}

template<typename T>
inline bool ArrayRange<T>::empty()
{
	return m_Begin == m_End;
}
