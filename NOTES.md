# There are multiple ways to start a thread in C++

Tutorials Point https://www.tutorialspoint.com/cplusplus/cpp_multithreading.htm
Geeks https://www.geeksforgeeks.org/multithreading-in-cpp/

### 1) Pass a function to execute

```c++
#include <iostream>
#include <thread>

void threadFunction()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // simulate work
    std::cout << "Finished work in thread\n";
}

int main()
{
    // create thread
    std::thread t(threadFunction);
    // at this point, the thread has started, but it may run longer than the main thread, so we will "join" this "t" thread to the main thread.

    // do something in main()
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // simulate work
    std::cout << "Finished work in main\n";

    // wait for thread to finish
    // To prevent the main thread from exiting before thread "t" has a chance to finish, we need to call join() on the thread object. This call will only return when the thread reaches the end of the thread function and block the main thread until then.
    t.join();

    return 0;
}

```

### 2) Starting a Thread with a Function Object

```c++
#include <iostream>
#include <thread>

class Vehicle
{
public:
    // Make sure to overload the class instatiator in order to use "copy initialization"
    void operator()()
    {
        std::cout << "Vehicle object has been created \n" << std::endl;
    }
};


int main()
{
    // create thread
    //std::thread t0(Vehicle()); // C++'s most vexing parse
    // We don't know if we are calling a function or instantiating a Vehicle object

    std::thread t1( (Vehicle()) ); // Add an extra pair of parantheses

    std::thread t2 = std::thread( Vehicle() ); // Use copy initialization

    std::thread t3{ Vehicle() }; // Use uniform initialization with braces

    std::cout << "Finished work in main \n";

    // wait for thread to finish
    t1.join();
    t2.join();
    t3.join();

    return 0;
}

```

### 3) Lambdas

A Lambda consists of three parts.

- A Capture List are variables that can be used by the lambda and be passed by reference or by copy. These variables normally could not be used within the main {} but now they can be captured. By default, variables in the capture block can not be modified within the Lambda. Using the keyword "mutable" allows to modify the parameters captured by copy, and to call their non-const member functions within the body of the Lambda. The following code examples show several ways of making the external variable "id" accessible within a Lambda.

- A Paramter List are variables that are passed into the function.
  The parameter list () : The way parameters are passed to a Lambda is basically identical to calling a regular function. If the Lambda takes no arguments, these parentheses can be omitted (except when "mutable" is used).

- The Main Part is where the function is defined.

```java
#include <iostream>

int main()
{
    // create lambdas
    int id = 0; // Define an integer variable

    //auto f0 = []() { std::cout << "ID = " << id << std::endl; }; // Error: 'id' cannot be accessed

    id++;
    auto f1 = [id]() { std::cout << "ID = " << id << std::endl; }; // OK, 'id' is captured by value

    id++;
    auto f2 = [&id]() { std::cout << "ID = " << id << std::endl; }; // OK, 'id' is captured by reference

    //auto f3 = [id]() { std::cout << "ID = " << ++id << std::endl; }; // Error, 'id' may not be modified

    auto f4 = [id]() mutable { std::cout << "ID = " << ++id << std::endl; }; // OK, 'id' may be modified

    // execute lambdas
    f1();
    f2();
    f4();

    return 0;
}
```

# There are several ways to pass data to a thread function.
