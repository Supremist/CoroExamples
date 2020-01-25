#include <iostream>
#include <memory>
#include <cppcoro/generator.hpp>

struct CoroutineFrame
{
	int state;
	int current = 1, next = 1;
};

struct Resumable
{
	Resumable();

	int next();

	std::shared_ptr<CoroutineFrame> c;
};

Resumable::Resumable()
    : c(std::make_shared<CoroutineFrame>())
{
	c->state = 0;
}

int Resumable::next()
{
#define co_yield(val)    \
	c->state = __LINE__; \
	return val;          \
	case __LINE__:       \
	;

	switch(c->state) {
	case 0:
		while(true) {
			co_yield(c->current)
			auto newElem = c->current + c->next;
			c->current = c->next;
			c->next = newElem;
		}
	}
	return 0;
#undef co_yield
}

int main(int argc, char *argv[])
{
	Resumable coro;
	const int fibCount = 10;
	std::cout << "Main: First " << fibCount << " Fibonacci numbers" << std::endl;
	for (int i = 0; i < fibCount; ++i) {
		std::cout << coro.next() << " ";
	}
	std::cout << std::endl;

	auto f = []() -> cppcoro::generator<float>
	{
	    co_yield 42.f;
	    co_yield 101.66f;
    };

	for (auto i : f()) {
		std::cout << i << std::endl;
	}
}
