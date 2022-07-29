#pragma once
#include <iterator>

template<typename data_t, size_t max_size>
class SingleCopyStack
{
public:
	SingleCopyStack() = default;

	void Push(const data_t& elem)
	{
		if (m_Container[m_Top] == elem)
			return;
		// Check if the element exists in the stack
		bool found = false;
		for (size_t i = m_Bottom; i != m_Top;) {
			if (m_Container[i] == elem)
				found = true;
			
			size_t next = (i+1) % m_Size;
			if (found)
				m_Container[i] = m_Container[next];
			
			i = next;
		}
		if (found) {
			m_Container[m_Top] = elem;
			return;
		}
		
		// If overflow remove bottom element
		// Push on top
		m_Top = (++m_Top) % m_Size;
		if (m_Top == m_Bottom)
			m_Bottom = (++m_Bottom) % m_Size;
		m_Container[m_Top] = elem;
	}

	data_t Pop()
	{
		APEX_ASSERT(!Empty(), "Underflow in SingleCopyStack!");

		data_t popped = m_Container[m_Top];
		m_Top = (m_Top == 0) ? m_Size-1 : (m_Top-1) % m_Size;
		return popped;
	}

	void Clear()
	{
		m_Top = m_Bottom = 0;
	}
	
	bool Empty()
	{
		return m_Top == m_Bottom;
	}
	
	data_t Top() {
		APEX_ASSERT(!Empty(), "SingleCopyStack is Empty!");
		return m_Container[m_Top];
	}
	
	size_t GetTopIdx() { return m_Top; }
	size_t GetBottomIdx() { return m_Bottom; }
	
public:
	struct Iterator
	{
		using iterator_category    = std::random_access_iterator_tag;
		using difference_type      = size_t;
		
		using value_type           = data_t;
		using pointer_type         = value_type*;
		using reference_type       = value_type&;
		using const_pointer_type   = value_type const*;
		using const_reference_type = value_type const&;
		
		Iterator(SingleCopyStack* ptr, difference_type idx) : m_ContainerPtr(ptr), m_Idx(idx) {}
		
		reference_type operator * () { return m_ContainerPtr->m_Container[m_Idx]; }
		const_reference_type operator * () const { return m_ContainerPtr->m_Container[m_Idx]; }
		
// 		pointer_type operator -> () { return &m_ContainerPtr->m_Container[m_Idx]; }
// 		const pointer_type operator -> () const { return &m_ContainerPtr->m_Container[m_Idx]; }
		
		Iterator& operator ++ ()
		{
			m_Idx = (m_Idx == 0) ? m_ContainerPtr->m_Size-1 : (m_Idx-1) % m_ContainerPtr->m_Size;
			return *this;
		}
		
		Iterator operator ++ (int)
		{
			Iterator temp = *this;
			++(*this);
			return temp;
		}
		
		friend bool operator == (const Iterator& lhs, const Iterator& rhs)
		{
			return lhs.m_ContainerPtr == rhs.m_ContainerPtr && lhs.m_Idx == rhs.m_Idx;
		}
		
		friend bool operator != (const Iterator& lhs, const Iterator& rhs)
		{
			return !(lhs == rhs);
		}
		
	private:
		SingleCopyStack* m_ContainerPtr;
		difference_type m_Idx;
		
	};
	
	Iterator begin() { return Iterator(this, m_Top); }
	Iterator end() { return Iterator(this, m_Bottom); }
	
private:
	const size_t m_Size = max_size+1;
	data_t m_Container[max_size+1];
	size_t m_Top = 0;
	size_t m_Bottom = 0;
}; 