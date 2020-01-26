#include <iostream>
#include <experimental/coroutine>

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
