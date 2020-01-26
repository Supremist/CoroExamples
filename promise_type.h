#pragma once

struct promise_type
{
  resumable get_return_object() {/**/}
  static resumable get_return_object_on_allocation_failure() {/**/}

  awaitable initial_suspend() {/**/}
  awaitable final_suspend() {/**/}

  void return_void() {/**/}
  void return_value(value_type val) {/**/}

  awaitable yield_value(value_type val) {/**/}

  void unhandled_exception() {/**/}

};
