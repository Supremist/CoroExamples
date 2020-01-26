#pragma once

template <>
struct coroutine_handle<void>
{
	// 21.11.2.1 construct/reset
	constexpr coroutine_handle() noexcept;
	constexpr coroutine_handle(nullptr_t) noexcept;
	coroutine_handle& operator=(nullptr_t) noexcept;

	// 21.11.2.2 export/import
	constexpr void* address() const noexcept;
	constexpr static coroutine_handle from_address(void* addr);

	// 21.11.2.3 observers
	constexpr explicit operator bool() const noexcept;
	bool done() const;

	// 21.11.2.4 resumption
	void operator()() const;
	void resume() const;
	void destroy() const;

private:
	void* ptr;// exposition only
};

template <class Promise>
struct coroutine_handle : coroutine_handle<>
{
	// 21.11.2.1 construct/reset
	using coroutine_handle<>::coroutine_handle;
	static coroutine_handle from_promise(Promise&);
	coroutine_handle& operator=(nullptr_t) noexcept;

	// 21.11.2.2 export/import
	constexpr static coroutine_handle from_address(void* addr);

	// 21.11.2.5 promise access
	Promise& promise() const;
};


// 21.11.2.6 comparison operators:
constexpr bool operator==(coroutine_handle<> x, coroutine_handle<> y) noexcept;
constexpr bool operator!=(coroutine_handle<> x, coroutine_handle<> y) noexcept;
constexpr bool operator<(coroutine_handle<> x, coroutine_handle<> y) noexcept;
constexpr bool operator>(coroutine_handle<> x, coroutine_handle<> y) noexcept;
constexpr bool operator<=(coroutine_handle<> x, coroutine_handle<> y) noexcept;
constexpr bool operator>=(coroutine_handle<> x, coroutine_handle<> y) noexcept;

// 21.11.2.7 hash support:
template <class T> struct hash;
template <class P> struct hash<experimental::coroutine_handle<P>>;
