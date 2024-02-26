#ifndef INCLUDE_BASE_FN_TRAITS_H_
#define INCLUDE_BASE_FN_TRAITS_H_

#include <tuple>

namespace vkexperiment::base {

template <typename T>
struct fn_traits : public fn_traits<decltype(&T::operator())> {};

template <typename Ret, typename... Args>
struct fn_traits<Ret(*)(Args...)> : public fn_traits<Ret(Args...)> {};

template <typename Ret, typename... Args>
struct fn_traits<Ret(&)(Args...)> : public fn_traits<Ret(Args...)> {};

template <typename Ret, typename... Args>
struct fn_traits<Ret(Args...)>
{
	template<std::size_t N>
	struct arg {
		using type = std::tuple_element_t<N, std::tuple<Args...>>;
	};

	using return_type = Ret;
};

}

#endif  // INCLUDE_BASE_FN_TRAITS_H_
