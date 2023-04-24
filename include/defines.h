#ifndef DEFINES_H
#define DEFINES_H

/* Inside header files, there should only be:
* Constants (uppercase variables)
* Macros (like functions but with less syntax)
* System wide global variables (like normal variables but outside functions)
* Function prototypes (only the first line of a function)
* Structs (custom types like 'Item')
* Header dependent includes (if a function prototype needs <stdlib.h>, then include it here)
*
* This means:
* - no '#include' other than those required by the header (if, for example, 'item.c' needs <stdlib>
* but 'item.h' doesn't, then include it in 'item.c')
* - no function definitions
*
* Unit header files (top level headers like 'item.h') should always be on top
* of other #include directives (for the compiler to verify the header)
*/

#define SECTION_TEXT_SIZE 20
#define TEXT_SIZE 200

#endif // !DEFINES_H
