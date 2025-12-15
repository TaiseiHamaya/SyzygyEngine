#pragma once

#include <type_traits>

template <typename T>
class SingletonInterface {
protected:
	SingletonInterface() = default;
	~SingletonInterface() = default;

public:
	SingletonInterface(const SingletonInterface&) = delete;
	SingletonInterface& operator=(const SingletonInterface&&) = delete;

public:
	static T& GetInstance() noexcept(std::is_nothrow_default_constructible_v<T>) {
		static T instance{};
		return instance;
	}
};

/// SingletonInterfaceを使用するクラスでのコンストラクタマクロ
#define SZG_CLASS_SINGLETON(CLASS_NAME)\
	friend class SingletonInterface<CLASS_NAME>;\
private:\
	CLASS_NAME() noexcept = default;\
	~CLASS_NAME() noexcept = default;\
\
public:\
	CLASS_NAME(const CLASS_NAME&) = delete;\
	CLASS_NAME& operator=(const CLASS_NAME&) = delete;\
	CLASS_NAME(CLASS_NAME&&) = delete;\
	CLASS_NAME& operator=(CLASS_NAME&&) = delete;

