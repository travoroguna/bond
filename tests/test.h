//
// Created by Travor Oguna Oneya on 28/03/2023.
//

#ifndef BOND_TESTS_H
#define BOND_TESTS_H

#include "../src/gc.h"
#include "../src/object.h"
#include "../src/engine.h"
#include <fmt/core.h>


#ifdef _MSC_VER
#define FUNC_P_ __FUNCSIG__
#else
#define FUNC_P_ __PRETTY_FUNCTION__
#endif

#define ASSERT(condition)                             \
{                                                                   \
  if( !( condition ) )                                              \
  {                                                                 \
    throw std::runtime_error(   std::string( __FILE__ )             \
                              + std::string( ":" )                  \
                              + std::to_string( __LINE__ )          \
                              + std::string( " in " )               \
                              + std::string( FUNC_P_ )  \
    );                                                              \
  }                                                                 \
}

#define ASSERT_EQUAL(x, y)                                  \
{                                                                   \
  if( ( x ) != ( y ) )                                              \
  {                                                                 \
    throw std::runtime_error(   std::string( __FILE__ )             \
                              + std::string( ":" )                  \
                              + std::to_string( __LINE__ )          \
                              + std::string( " in " )               \
                              + std::string( FUNC_P_ )  \
                              + std::string( ": " )                 \
                              + std::to_string( ( x ) )             \
                              + std::string( " != " )               \
                              + std::to_string( ( y ) )             \
    );                                                              \
  }                                                                 \
}



#endif //BOND_TESTS_H