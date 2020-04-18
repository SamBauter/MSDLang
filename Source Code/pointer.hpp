//
//  pointer.hpp
//  MSDLang
//
//  Created by Samuel Bauter on 3/1/20.
//  Copyright © 2020 U of U. All rights reserved.
//

#ifndef pointer_hpp
#define pointer_hpp

#include <stdio.h>


#if 0

# define NEW(T)  new T
# define PTR(T)  T*
# define CAST(T) dynamic_cast<T*>
# define THIS    this

#else

# define NEW(T)  std::make_shared<T>
# define PTR(T)  std::shared_ptr<T>
# define CAST(T) std::dynamic_pointer_cast<T>
# define THIS    shared_from_this()
# define ENABLE_THIS(T) : public std::enable_shared_from_this<T>


#endif

#endif /* pointer_hpp */
