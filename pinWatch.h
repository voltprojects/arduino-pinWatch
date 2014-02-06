/**
* @author Rhys Bryant <rhys@voltprojects.com>
* basic command processing support for Arduino
* I make no clam to be a c++ programmer. if you can see a better way please let me know or report a bug
* @copyright Rhys Bryant <rhys@voltprojects.com>
*
*/
#include <stdio.h>
#define OPT_RANGE '~'
#define OPT_GT '>'
#define OPT_LT '<'
#define OPT_EQ '='
#define CONDITION_COUNT 10
typedef float (*pw_type_cb)(int pinNumber);
#include "pinWatch.cpp"
//ini static
char PinWatch::Condition::tmp[40];