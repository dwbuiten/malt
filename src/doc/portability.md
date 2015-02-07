Portability
===========


Code structure
--------------

Functions which might be impacted by portability issue are mostly stored into lib/portability subdirectory.
This directory provide a wrapper header for each class of portable groups :

- Compiler.hpp
- OS.hpp
- Mutex.hpp
- Spinlock.hpp
- ProcMapReader.hpp

Each of them are generic and point to specialized implementation :

- Compiler : CompilerGNU
- OS : OSUnix
- Mutex : MutexPthread, LockDummy
- Spinlock : SpinlockPthread, LockDummy

Selection
---------

Implementation are selected throught variables exported by cmake into src/lib/config.h :

- MATT\_PORTABILITY\_SPINLOCK\_\* : Provide spinlock wrapper, by default use pthread implementation but might be other
  for example on windows. Matt also provide a Dummy verison.
- MATT\_PORTABILITY\_MUTEX\_\* : Provide spinlock wrapper, by default use pthread implementation but might be other
  for example on windows. Matt also provide a Dummy version.
- MATT\_PORTABILITY\_OS\_\* : Provide some OS specific routines. Currently only implement Unix version.
- MATT\_PORTABILITY\_COMPILER\_\* : Provide some compiler specific routines. Currently only provide GCC/ICC for C++
  demangling.


