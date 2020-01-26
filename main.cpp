#include "generator.h"

#include <iostream>

Generator<int> fibonacci()
{
	int current = 1, next = 1;
	while (true) {
		co_yield current;
		auto new_value = current + next;
		current = next;
		next = new_value;
	}
}

int main(int argc, char *argv[])
{
	int count = 0;
	auto fibGenerator = fibonacci();
	while(fibGenerator.hasNextValue()) {
		if (count++ >= 10) {
			break;
		}
		std::cout << fibGenerator.nextValue() << std::endl;
	}
}
