#include <iostream>
#include <experimental/coroutine>

struct Promise {
	using CoroHandle = std::experimental::coroutine_handle<Promise>;
	CoroHandle get_return_object() { return CoroHandle::from_promise(*this); }
	auto initial_suspend() { return std::experimental::suspend_never(); }
	auto final_suspend() { return std::experimental::suspend_never(); }
};

namespace std::experimental {
template<>
struct coroutine_traits<Promise::CoroHandle> {
	using promise_type = Promise;
};
}


Promise::CoroHandle myFirstCoroutine()
{
	std::cout << "Hello" << std::endl;
	co_await std::experimental::suspend_always();
	std::cout << "Coroutine" << std::endl;
}

int main(int argc, char *argv[])
{
	auto coro = myFirstCoroutine();
	std::cout << "Main: (doing useful work)" << std::endl;
	coro.resume();
}
