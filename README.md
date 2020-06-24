## GENERAL INFO
  The main purpose of the program is to visualise the bit structure of some C data types entered in any base form.

## RUN
  To run this program use one of the following:
  #### ./visualise data_type data
  #### ./visualise integer_data_type data base
  where:
  > **data_type**         = **{char, short, int, long, float, double}**
  
  > **integer_data_type** = **{char, short, int, long}**
  
  > (-1) * 1.8 * 10 ^ 308 <= **data** <= 1.8 * 10 ^ 308 (**DBL_MIN**, **DBL_MAX**)
  
  > 2 <= **base** <= 16

  Example:
  ```sh
  $ ./visualise char 7
  $ ./visualise long DEADBEEF 16
  ```

## ABOUT THE PROGRAM
### 1. When you run it using ./visualise data_type data:
- Convert the string *data_type* into an int, enumerated in enum, such that (2 ^ data_type) gives the size of that data type and 8 * (2 ^ data_type) gives the numbers of bits required for binary representation of a decimal number of that data type. For example, for "int" data type, 2 ^ Int = 2 ^ 2 = 4 and 8 * (2 ^ Int) = 8 * 4 = 32.
**Note**: Exception *float* and *double* as I used another constants in order to represent their component parts: sign, exponent and mantissa in binary.
- Convert the string *data* into an integer (long) or a floating-point (double), depending on the *data_type* given.
**Note**: "If a decimal string with at most 15 significant digits is converted to IEEE 754 double-precision representation,
and then converted back to a decimal string with the same number of digits, the final result should match the original string." (https://en.wikipedia.org/wiki/Double-precision_floating-point_format). Therefore, if you introduce a big number with an "ugly" fractional part, it is a highly chance that the program will print nothing due to the fact that conversion is inaccurate.

### 2. When you run it using ./visualise <integer_data_type data base:
- Convert the string *integer_data_type* the same as above **(1)**.
- Convert the string *base* into an int.
- Convert the string *data* into an integer, but only after the data has been converted from that base form to a decimal string.
