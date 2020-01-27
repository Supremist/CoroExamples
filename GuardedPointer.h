#pragma once

#include <mutex>
#include <memory>
#include <utility>

namespace ptr_guard {
template<typename Mutex, typename Pointer>
class PointerGuard {
	std::unique_lock<Mutex> lock;
	Pointer ptr;

public:
	PointerGuard() = default;
	PointerGuard(Mutex &mutex, const Pointer &pointer)
	    : lock(mutex)
	    , ptr(pointer)
	{}

	PointerGuard(std::unique_lock<Mutex> ulock, const Pointer &pointer)
	    : lock(std::move(ulock))
	    , ptr(pointer)
	{
	}

	std::unique_lock<Mutex> acquireLock()
	{
		ptr = {};
		return std::move(lock);
	}

	Pointer operator->() { return ptr; }
	auto operator*() -> decltype(*ptr) { return *ptr; }
	auto get() const -> decltype(&(*ptr)) { return &(*ptr); }
	explicit operator bool() const { return bool(ptr); }
};

template<typename Mutex, typename Pointer>
PointerGuard<Mutex, Pointer> make_guarded_ptr(Mutex &mutex, const Pointer &pointer)
{
	return PointerGuard<Mutex, Pointer>(mutex, pointer);
}

template<typename Mutex, typename Pointer>
PointerGuard<Mutex, Pointer> make_guarded_ptr(std::unique_lock<Mutex> ulock, const Pointer &pointer)
{
	return PointerGuard<Mutex, Pointer>(std::move(ulock), pointer);
}

template<typename T, typename Mutex = std::mutex>
class Guarded
{
public:
	using Ptr = std::shared_ptr<Guarded<T, Mutex>>;

	template<typename... Args>
	Guarded(Args&&... args)
	    : m_data(std::forward<Args>(args)...)
	{
	}

	PointerGuard<Mutex, T*> get()
	{
		return make_guarded_ptr(m_mutex, &m_data);
	}

	PointerGuard<Mutex, const T*> get() const
	{
		return make_guarded_ptr(m_mutex, &m_data);
	}

private:
	Guarded(const Guarded<Mutex, T> &) = delete;

	mutable Mutex m_mutex;
	T m_data;
};
}
