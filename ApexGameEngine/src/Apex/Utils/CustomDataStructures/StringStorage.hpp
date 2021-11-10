#pragma once

class StringStorage
{
public:
	StringStorage(size_t size = 100)
	{
		storage.reserve(size * 30);
		sizes.reserve(size);
		table.reserve(size);
	}

	inline uint64_t Add(uint64_t hash, const std::string& str)
	{
		APEX_CORE_ASSERT(!Has(hash), "String '" + str + "' already exists!");

		table.emplace(hash, std::pair{ storage.size(), sizes.size() });
		storage.insert(storage.end(), str.begin(), str.end());
		sizes.push_back(str.size());
		return hash;
	}

	inline std::string_view Get(uint64_t hash)
	{
		APEX_CORE_ASSERT(Has(hash), "String does not exist!");
		auto indexes = table.find(hash)->second;
		return std::string_view(&storage[indexes.first], sizes[indexes.second]);
	}

	inline bool Has(uint64_t hash)
	{
		return table.find(hash) != table.end();
	}

private:
	std::vector<char> storage;
	std::vector<size_t> sizes;
	std::unordered_map<uint64_t, std::pair<size_t, size_t>> table;
};