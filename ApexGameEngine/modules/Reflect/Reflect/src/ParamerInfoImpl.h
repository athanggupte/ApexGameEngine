#pragma once
#include "ParameterInfoBase.h"
#include "FunctionArgs.h"

struct ParameterInfo;

namespace detail {

	template <typename Param_Type, size_t Param_Index, typename Default_Type>
	struct ParameterInfoImpl : public ParameterInfoBase
	{
		ParameterInfoImpl(std::string_view name)
			: m_Name(name)
		{
		}

		ParameterInfoImpl(std::string_view name, const Default_Type* default_value)
			: m_Name(name), m_DefaultValue(default_value)
		{
		}

		std::string_view Name() const override { return m_Name; }
		::Type* Type() const override { return GetType<Param_Type>(); }
		size_t Index() const override { return Param_Index; }

		bool HasDefaultValue() const override { return true; }
		Any GetDefaultValue() const override { return *m_DefaultValue; }
		void SetDefaultValue(const Default_Type* defaul_value) const override { m_DefaultValue = defaul_value; }

	private:
		std::string_view m_Name;
		const Default_Type* m_DefaultValue = nullptr;
	};

	template <typename Param_Type, size_t Param_Index>
	struct ParameterInfoImpl<Param_Type, Param_Index, void> : public ParameterInfoBase
	{
		ParameterInfoImpl(std::string_view name)
			: m_Name(name)
		{
		}

		std::string_view Name() const override { return m_Name; }
		::Type* Type() const override { return GetType<Param_Type>(); }
		size_t Index() const override { return Param_Index; }

		bool HasDefaultValue() const override { return true; }
		Any GetDefaultValue() const override { return {}; }

	private:
		std::string_view m_Name;
	};


	// Helper functions
	template <typename T>
	static inline ParameterInfo CreateParamInfo(const T& data)
	{
		return ParameterInfo(&data);
	}

	template <size_t... Indices, typename... T>
	static inline std::array<ParameterInfo, sizeof...(T)>
	CreateParamInfoArray(std::index_sequence<Indices...>, const ParamInfos<T...>& param_infos)
	{
		return { { CreateParamInfo(std::get<Indices>(param_infos.Infos))... } };
	}

	template <typename... T, size_t Size = sizeof...(T)>
	static inline std::array<ParameterInfo, Size>
	CreateParamInfoArray(const ParamInfos<T...>& param_infos)
	{
		return CreateParamInfoArray(std::make_index_sequence<Size>(), param_infos);
	}

	template <typename... T>
	static inline ParamInfos<T...>
	CreateParamInfos()
	{
	}

}