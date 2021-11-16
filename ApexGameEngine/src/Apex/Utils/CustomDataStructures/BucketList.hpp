#pragma once

template<typename Type, uint64_t BucketSize = 64>
class BucketList
{
    static_assert(BucketSize >= 2, "Minimum bucket size is 2!");

    struct Bucket
    {
        std::array<Type, BucketSize> data;
        uint64_t backIndex = 0;

        [[nodiscard]] size_t capacity() const
        {
            return data.size();
        }

        [[nodiscard]] size_t size() const
        {
            return backIndex;
        }

        Type& operator [] (size_t index)
        {
            return data[index];
        }

        const Type& operator [] (size_t index) const
        {
            return data[index];
        }

        void insert(const Type& x)
        {
            data[backIndex] = x;
            ++backIndex;
        }

        void insert(Type&& x)
        {
            data[backIndex] = std::move(x);
            ++backIndex;
        }

    };

public:

    class Iterator
    {
    public:
        Iterator(const BucketList& container, uint64_t dataIndex, const typename std::list<Bucket>::iterator& bucketIt)
            : container(container), dataIndex(dataIndex), bucketIt(bucketIt)
        {
        }

        Iterator(const BucketList& container, uint64_t index = 0)
            : container(container), dataIndex(index % BucketSize), bucketIt(container.m_BucketList.begin())
        {
            auto bucketIndex = index / BucketSize;
            for (int i = 0; i < bucketIndex; ++i, ++bucketIt);
        }

        Type& operator * ()
        {
            return (*bucketIt)[dataIndex];
        }

        Iterator& operator ++ ()
        {
            ++dataIndex;
            if (dataIndex == BucketSize) {
                dataIndex = 0;
                ++bucketIt;
            }
            return *this;
        }

        Iterator operator ++ (int)
		{
			Iterator temp = *this;
			++(*this);
			return temp;
		}

        Iterator& operator -- ()
        {
            if (dataIndex == 0) {
                dataIndex = BucketSize - 1;
                --bucketIt;
            } else {
                --dataIndex;
            }
            return *this;
        }

		Iterator operator -- (int)
		{
			Iterator temp = *this;
			--(*this);
			return temp;
		}

        bool operator != (const Iterator& other) const
        {
            return (&container != &other.container) || (dataIndex != other.dataIndex) || (bucketIt != other.bucketIt);
        }

        bool operator < (const Iterator& other) const
        {
            return (bucketIt < other.bucketIt) && (dataIndex < other.dataIndex);
        }

    private:
        const BucketList& container;
        uint64_t dataIndex;
        typename std::list<Bucket>::iterator bucketIt;

        friend class BucketList;
    };

    using iterator = Iterator;
    using const_iterator = Iterator;

    BucketList()
    {
        m_BucketList.emplace_back();
    }

    void push_back(const Type& x)
    {
        auto& back = m_BucketList.back();
        if (back.size() < back.capacity()) {
            back.insert(x);
        } else {
            m_BucketList.emplace_back();
            auto& newBack = m_BucketList.back();
            newBack.insert(x);
        }
        ++m_Size;
    }

    Type& operator [] (size_t index)
    {
        APEX_CORE_ASSERT(index < m_Size, "Index out of bounds!");
        auto bucketIndex = index / BucketSize;
        auto dataIndex = index % BucketSize;

        auto it = m_BucketList.begin();
        for (size_t i = 0; i < bucketIndex; ++i, ++it) {}

        return (*it)[dataIndex];
    }

    const Type& operator [] (size_t index) const
    {
        APEX_CORE_ASSERT(index < m_Size, "Index out of bounds!");
        auto bucketIndex = index / BucketSize;
        auto dataIndex = index % BucketSize;

        auto it = m_BucketList.begin();
        for (size_t i = 0; i < bucketIndex; ++i, ++it) {}

        return (*it)[dataIndex];
    }

    Type& back()
    {
        return *(--this->end());
    }

    [[nodiscard]] const Type& back() const
    {
        return *(--this->end());
    }

    Type& front()
    {
        return *this->begin();
    }

    [[nodiscard]] const Type& front() const
    {
        return *this->begin();
    }

    Iterator begin()
    {
        return Iterator(*this, 0, m_BucketList.begin());
    }

    Iterator end()
    {
        auto lastIndex = m_Size % BucketSize;
        return Iterator(*this, lastIndex, (lastIndex == 0 ? m_BucketList.end() : --m_BucketList.end()));
    }

    size_t size() const
    {
	    return m_Size;
    }

private:
    size_t m_Size = 0;
    std::list<Bucket> m_BucketList;
};
