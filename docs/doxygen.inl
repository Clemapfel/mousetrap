#ifdef DOXYGEN

    // doxygen is unable to parse preprocessor macros in the inheritance part of a class declaration,
    // unless they are defined in the same file. This file defines macros from include/signal_component.hpp
    // it has no impact on actual compilation using a traditional compiler

    #define SIGNAL_CLASS_NAME(signal_name) has_signal_##signal_name
    #define HAS_SIGNAL(T, name) public SIGNAL_CLASS_NAME(name)<T>
#endif