# Board
These are classes of Go board. User should only use `board::Board` class,
 and any other class, if not specified by the parameter/return value type
 of `Board`, will not be guaranteed consistency. 
 
By default, `board::Board` is a class template. In order to reduce template 
bloat, some template class has been explicitly instantiated in `board.cpp`, thus
 you also need compiling `board.cpp` and link it with your program which 
 should include `board.hpp`. Using class that are not explicitly instantiated 
 should just work.
  
To change which classes are instantiated, 
 edit `board_class_templ_header.hpp` and `board_class_templ_inst.cpp`, and
 add newlines as follows:
```cpp
// in board_class_templ_header.hpp
    extern template class Board<9, 9>;
// in board_class_templ_inst.cpp
    template class Board<9, 9>;
```