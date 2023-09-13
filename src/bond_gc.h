#pragma once
#undef DEBUG

//#define GC_BUILD
//#define GC_NOT_DLL
#define GC_DLL


#ifdef DEBUG
#define GC_DEBUG
#endif

#define ALL_INTERIOR_POINTERS

#ifndef EMSCRIPTEN
    #define GC_THREADS
#endif

#include <gc.h>
#include <gc_cpp.h>
#include <gc/gc_allocator.h>
#include <thread>
#include <mutex>