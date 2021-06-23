# OBSERVER

## Progress

- For now, only a class able to launch and track accurately sub-processes on POSIX systems is available. It will be cleaned and optimized (if required) before a class with the same public interface was created for MS-DOS systems (Windows).
- The next step will consist in doing the basis of classes (certainly with the "composite" pattern) to create the objects of the cascade.

## Sub Processes Launcher (class Subprocess)
- This class is able to instanciate objects that represent sub-processes as state machines. Many properties of these processes are being tracked like the I/Os streams or their state (not launched yet, running, stopped, finished, ...).
- You can register a callback function to the sub process. It will be called with the new state as argument whenever a state switch occurs.

## Global todo's
- [ ] Make a detailed UML prediction of what the system should look like
- [ ] Verify that this pattern is compatible with scripting (without sacrifying perfs)
- [ ] Start integrating Doxygen in the project

## Subprocess todo's
- [ ] Check for possible optimisation of Subprocess class
- [ ] Start a Windows version of this class
- [ ] See if wee use an abstract factory or a preprocessor check to determine which one to use
- [ ] Triggering callbacks could be done on another thread to prevent the entire sub process to stay frozen
- [ ] Make a testing session on the class to mesure bottlenecks