#include <iostream>
#include <experimental/coroutine>

struct Promise {
	using CoroHandle = std::experimental::coroutine_handle<Promise>;
	CoroHandle get_return_object() { return CoroHandle::from_promise(*this); }
	auto initial_suspend() { return std::experimental::suspend_never(); }
	auto final_suspend() { return std::experimental::suspend_always(); }

//	void return_value(int val)
//	{
//		msg = "Returned int";
//		value = val;
//	}

//	void return_value(double val)
//	{
//		msg = "Returned doble";
//		value = int(val);
//	}

	void return_void()
	{
		msg = "Returned void";
		value = 0;
	}

	std::string msg;
	int value;
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
	co_await std::experimental::suspend_always();
	std::cout << "Calculating the meaning of life" << std::endl;
	co_return ;
}

int main(int argc, char *argv[])
{
	auto coro = myFirstCoroutine();
	while (!coro.done()) {
		std::cout << "Main: (doing useful work)" << std::endl;
		coro.resume();
	}
	std::cout << "The meaning of life is: " << coro.promise().value << std::endl;
	std::cout << "Additional msg: " << coro.promise().msg.c_str() << std::endl;
}
