#pragma once
#include <tuple>

template <typename... T>
struct ParamInfos
{
    ParamInfos() {}
    ParamInfos(T&&... args) : Infos(std::forward<T>(args)...) {}

    std::tuple<T...> Infos;

    static constexpr size_t Count = sizeof...(T);
};

template <>
struct ParamInfos<>
{
    ParamInfos() {}

    std::tuple<> Infos;

    static constexpr size_t Count = 0;
};

template <typename... T>
struct DefaultArgs
{
    DefaultArgs() {}
    DefaultArgs(T&&... args) : Args(std::forward<T>(args)...) {}

    std::tuple<T...> Infos;

    static constexpr size_t Count = sizeof...(T);
};

template <>
struct DefaultArgs<>
{
    DefaultArgs() {}

    std::tuple<> Infos;

    static constexpr size_t Count = 0;
};
