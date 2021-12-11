
Some environments provide specific support for blocking a thread of execution
without blocking the remainder of the process execution. Python, for example,
has async tasks/co-routines. SystemVerilog has tasks that can block a thread
of execution on a simulation event.

The `IEndpoint::process_one_event` method is defined to suspend execution
until a message is received and processed or an exception occurs. This method
is typically defined such that it suspends all threads of execution in the 
process.

Environments that support a blocking mechanism that does not block all
threads should supply an 'IEndpoint::process_one_event_b' method. For example,
in Python, this would be defined as an `async` method. This would be 
defined as a `task` in SystemVerilog. 

Depending on the characteristics of the endpoint connection, calling a
special blocking method may still block all threads. For example, 
if the connection is socket-based, blocking execution for the entire
environment is the appropriate behavior. On the other hand, if the
endpoint connection is a 'loopback' connection, blocking just the
communicating processes is the appropriate behavior.

Why define two different methods? Environments that make a distinction
between blocking and non-blocking calls typically require special
syntax for these two constructs -- both in declaration and in 
invocation.



