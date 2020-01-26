#include <iostream>
#include <experimental/coroutine>

namespace std::experimental {
template<>
struct coroutine_traits<void>{
	struct promise_type {
		void get_return_object() {}
		auto initial_suspend() { return std::experimental::suspend_never(); }
		auto final_suspend() { return std::experimental::suspend_never(); }
	};
};
}

void myFirstCoroutine()
{
	std::cout << "Hello" << std::endl;
	co_await std::experimental::suspend_never();
	std::cout << "Coroutine" << std::endl;
}

int main(int argc, char *argv[])
{
	myFirstCoroutine();
}
