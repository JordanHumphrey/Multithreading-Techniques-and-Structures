#include <chrono>
#include <list>
#include <thread>
#include <deque>
#include <future>
#include <memory>
#include <vector>
#include <mutex>
#include <utility>
#include <iostream>
#include "ThreadSafe_Queue.h"
#include "ThreadSafe_Stack.h"


#pragma region Condition Variables

namespace Condition_Variables
{

	void push(Threadsafe_Queue<int>* q)
	{
		for (int i = 0; i < 10; ++i) {
			//std::cout << "pushing " << i << std::endl;
			printf("pushing %d\n", i);
			q->Push(i);
		}
	}

	void push_s(Threadsafe_Stack<int>* s)
	{
		for (int i = 0; i < 10; ++i) {
			//std::cout << "pushing " << i << std::endl;
			printf("pushing %d\n", i);
			s->Push(i);
		}
	}

	void pop(Threadsafe_Queue<int>* q)
	{
		for (int i = 0; i < 10; ++i) {
			printf("poping %d\n", *q->Wait_And_Pop());
		}
	}

	void pop_s(Threadsafe_Stack<int>* s)
	{
		for (int i = 0; i < 10; ++i)
		{
			printf("poping %d\n", *s->Wait_And_Pop());
		}
	}

	void Launch_Condition_Variables()
	{
		std::cout << "Threadsafe_Queue Results:\n";
		Threadsafe_Queue<int> q;
		std::thread t1(push, &q);
		std::thread t2(pop, &q);
		t1.join();
		t2.join();

		std::cout << "\nThreadsafe_Queue Results:\n";
		Threadsafe_Stack<int> s;
		std::thread t3(push_s, &s);
		std::thread t4(pop_s, &s);
		t3.join();
		t4.join();

	}

}
#pragma endregion

#pragma region Future / Async Example

// Using futures and async
int find_the_answer_to_ltuae() { return 5; }
void Example1()
{
	// std:async allows you to pass additional arguments to the function by adding extra
	// arguments to the call, in the same way that std::thread does.
	std::future<int> the_answer = std::async(find_the_answer_to_ltuae);
	std::cout << "The answer is " << the_answer.get() << std::endl;

}

#pragma endregion

#pragma region Example Packaged_Task

namespace Simple_Packaged_Task_Example
{
	// Associating a task with a future using std::package_task<>
	// Simple example
	int countdown(int from, int to)
	{
		for (size_t i = from; i != to; --i)
		{
			std::cout << i << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		std::cout << "Lift off!\n";
		return from - to;
	}

	void Launch()
	{
		std::packaged_task<int(int, int)> task(countdown);
		// Another way to do it, bind the parameters, and remove them from 
		// the template arguments since they have been satisfied/supplied
		// std::packaged_task<int()> task(std::bind(countdown, 10, 0));
		std::future<int> res = task.get_future();

		// Pass the task, and the task's functional arguments
		std::thread th(std::move(task), 10, 0);

		// Wait for the task to finish and get result
		int value = res.get();

		std::cout << "The countdown for " << value << " seconds.\n";
		th.join();
	}
}

#pragma endregion

#pragma region PASSING ARGUMENTS BETWEEN THREADS
/*
namespace Passing_Between_Threads
{
	std::mutex m;
	std::deque<std::packaged_task<void()>> tasks;
	
	bool gui_shutdown_message_received();
	void get_and_process_gui_message();

	void gui_thread()
	{
		while (!gui_shutdown_message_received())
		{
			get_and_process_gui_message();
			std::packaged_task<void()> task;
			{
				std::lock_guard<std::mutex> lk(m);
				if (tasks.empty())
					continue;
				task = std::move(tasks.front());
				tasks.pop_front();
			}
			task();
		}
	}

	std::thread gui_bg_thread(gui_thread);

	template<typename Func>
	std::future<void> post_task_for_gui_thread(Func f)
	{
		std::packaged_task<void()> task(f);
		std::future<void> res = task.get_future();
		std::lock_guard<std::mutex> lk(m);
		tasks.push_back(std::move(task));
		return res;
	}
}
*/
#pragma endregion

#pragma region Promises

namespace Promises
{
#pragma region Simple Example
	void print_int(std::future<int>& fut)
	{
		int x = fut.get();
		std::cout << "value: " << x << std::endl;
	}

	void Launch()
	{
		std::promise<int> prom;
		std::future<int> fut = prom.get_future();
		std::thread th(print_int, std::ref(fut));

		// Fulfill promise (synchronizes with getting future)
		prom.set_value(10);
		th.join();
	}
#pragma endregion

#pragma region Handling Multiple Connections from a Single Thread
	/*

	(1) function processes connections until done is satisfied
	(2) it checks every connection in turn, 
	(3) retrieving incoming data if there is any
	(5) or sending any outgoing data.
		This assumes that an incoming packet has some ID and a
		payload with the actual data in it. The ID is mapped to 
		a std::promise (perhaps by look-up in an associative 
		container (4), and the value is set to the packet's
		payload. 

	void process_connections(connection_set& connections)
	{
		while (!done(connections)) (1)
		{
			for (connection_set::iterator connection = connections.begin(), (2)
				end = connections.end(); connection != end; ++connection)
			{
				if (connection->has_incoming_data()) (3)
				{
					data_packet data = connection->incoming();
					std::promise<payload_type>& p = 
						connection->get_promise(data.id); (4)
					p.set_value(data.payload);
				}
				if(connection->has_outgoing_data()) (5)
				{
				outgoing_packet data = 
					connection->top_of_outgoing_queue();
				connection->send(data.payload);
				data.promise.set_value(true); (6)
			}
		}
	}
	*/
#pragma endregion

}

#pragma endregion

#pragma region Saving an Exception for the Future

namespace Saving_Exceptions
{
	/*
	extern std::promise<double> some_promise;
	void func()
	{
		try {
			some_promise.set_value(calculate_value());
		}
		catch(...)
		{
			some_promise.set_exception(std::current_exception());
		}
	}

	Alternatively, if you know the exception ur expecting to throw
	some_promise.set_exception(std::copy_exception(std::llogic_error("foo ")));
	*/
}

#pragma endregion

#pragma region Waiting for a Condition Variable with a Timeout

namespace Timeout
{
	std::condition_variable cv;
	bool done;
	std::mutex m;

	bool wait_loop()
	{
		auto const timeout = std::chrono::steady_clock::now() +
			std::chrono::milliseconds(500);
		std::unique_lock<std::mutex> lk(m);

		while (!done)
		{
			if (cv.wait_until(lk, timeout) == std::cv_status::timeout)
				break;
		}
		return done;
	}
}

#pragma endregion

#pragma region Using Synchronization of Ops to Simplify Code & Function Programming

namespace Functional
{
	template<typename T>
	std::list<T> sequential_quick_sort(std::list<T> input)
	{
		if (input.empty())
		{
			return input;
		}
		std::list<T> result;
		result.splice(result.begin(), input, input.begin());
		T const& pivot = *result.begin();

		auto divide_point = std::partition(input.begin(), input.end(),
			[&](T const& t) {return t < pivot; });

		std::list<T> lower_part;
		lower_part.splice(lower_part.end(), input, input.begin(),
			divide_point);

		auto new_lower(
			sequential_quick_sort(std::move(lower_part));
		auto new_higher(
			sequential_quick_sort(std::move(input)));

		result.splice(result.end(), new_higher);
		result.splice(result.begin(), new_lower);
		return result;
	}

	template<typename T>
	std::list<T> parallel_quick_sort(std::list<T> input)
	{
		if (input.empty())
		{
			return input;
		}
		std::list<T> result;
		result.splice(result.begin(), input, input.begin());
		T const& pivot = *result.begin();

		auto divide_point = std::partition(input.begin(), input.end(),
			[&](T const& t) {return t < pivot; });

		std::list<T> lower_part;
		lower_part.splice(lower_part.end(), input, input.begin(),
			divide_point);

		std::future<std::list<T>> new_lower(
			std::async(parallel_quick_sort(std::move(lower_part))));
		auto new_higher(
			sequential_quick_sort(std::move(input)));

		result.splice(result.end(), new_higher);
		result.splice(result.begin(), new_lower.get());
		return result;
	}
}

#pragma endregion


int main()
{
	return 0;
}