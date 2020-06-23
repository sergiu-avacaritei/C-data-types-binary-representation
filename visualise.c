/* Visualise the bit structure of some C data types entered in any base form */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <limits.h>
#include <float.h>
#include <math.h>

// Integer constants defining the maximum bits that mantissa and exponent can
// represent.
#define MANTISSA_DOUBLE_BITS 52
#define MANTISSA_FLOAT_BITS 23
#define EXPONENT_DOUBLE_BITS 11
#define EXPONENT_FLOAT_BITS 8

// Integer constants representing C data types.
enum { Char, Short, Int, Long, Float, Double, Impossible };
enum { Integer, FloatingPoint };

// Array of strings representing C data types (It works like a "dictionary").
const char *dataTypes[] = {"char", "short", "int", "long",
                           "float", "double",
                           "impossible"};

const int MAX_DIGITS_LONG     = 21;  // [log10(2 ^ 64 - 1)] + 1
const int MAX_DIGITS_DOUBLE   = 317; // [log10(1.8 * 10 ^ 308)] + 1 + 6
const int BYTE                = 8;   //1 byte = 8 bits

// Return the absolute value of a double.
double myAbs(double data) {
    if (data < 0) return -data;
    return data;
}

// Check if the type given is an integer type or a floating-point type.
// Return -1 if is not valid.
int isType(int type) {
    if (Char <= type && type <= Long) return Integer;
    else if (Float <= type && type <= Double) return FloatingPoint;
    return -1;
}

// Iterate through dataTypes array and check if the given type is correctly
// typed as input. Return -1 if it is not valid. (for intance, "Long" != "long")
int convertDataType(const char type[]) {
    for (int i = Char; i <= Double; i++)
        if (strcmp(type, dataTypes[i]) == 0) return i;
    return -1;
}

// Check if there is a match between the given type (of the data) and the given
// data and return the actual type. Return "Impossible" if there is no match,
//for instance, ./visualise char 2147483647 ==> Impossible.
int convertIntegerDataType(int type, long data, bool foundData) {
    if (foundData) {
        if (type==Char && SCHAR_MIN<=data && data<=SCHAR_MAX) return Char;
        else if (type==Short && SHRT_MIN<=data && data<=SHRT_MAX) return Short;
        else if (type==Int && INT_MIN<=data && data<=INT_MAX) return Int;
        else if (type==Long && LONG_MIN<=data && data<=LONG_MAX) return Long;
    }
    return Impossible;
}

//Check if there is a match between the given type (of the data) and the given
// data and return the actual type. Return "Impossible" if there is no match.
int convertFloatingPointDataType(int type, double data, bool foundData) {
    if (foundData) {
        if (type == Float && myAbs(data) <= FLT_MAX) return Float;
        else if (type == Double && myAbs(data) <= DBL_MAX) return Double;
    }
    return Impossible;
}

// Convert a string into an integer (long). Return -1 if it is not valid.
long convertIntegerData(const char data[], bool *foundData) {
    long n = atol(data);
    char auxData[MAX_DIGITS_LONG];
    sprintf(auxData, "%ld", n);
    if (strcmp(auxData, data) == 0) {
        *foundData = true;
        return n;
    }
    return -1;
}

// Convert a string into a floating-point (double). Return -1 if it is not valid.
// "If a decimal string with at most 15 significant digits is converted to
// IEEE 754 double-precision representation, and then converted back to a
// decimal string with the same number of digits, the final result should match
// the original string."
// (https://en.wikipedia.org/wiki/Double-precision_floating-point_format)
double convertFloatingPointData(const char data[], bool *foundData) {
    double n = strtod(data, NULL);
    char auxData[MAX_DIGITS_DOUBLE];
    sprintf(auxData, "%lf", n);
    if(strncmp(auxData, data, strlen(data)) == 0) {
        *foundData = true;
        return n;
    }
    return -1;
}

// Return decimal value of a char such that ['A'...'F'] maps to [10...15] and
// ['0'...'9'] to [0...9].
int value(char c) {
    if (isdigit(c)) return c - '0';
    else return toupper(c) - 'A' + 10;
}

// Return the signed value of the data (n) passed by reference (within the
// convertDecimal() function).
void getSignedData(long *n, int type, const char data[], int b, int len) {
    if (b > 2 ) {
        if (data[0] == '-') *n = -(*n);
        else *n += value(data[0]) * pow(b, len - 1);
    }
    else if (b == 2) {
        if (data[0] == '1' && len == (1 << type) * BYTE) *n -= pow(b, len - 1);
        else *n += value(data[0]) * pow(b, len - 1);
    }
}

// Convert integer data from any base to decimal (2 <= base <= 16).
void convertDecimal(int type, const char data[], const char base[], char decimalData[]) {
    long n = 0;
    int b = atoi(base);
    int len = strlen(data);
    for (int i = len - 1, k = 0; i > 0; i--) {
        if (value(data[i]) < b && data[i] != '.') {
            n += value(data[i]) * pow(b, k++);
        }
        else return;
    }
    getSignedData(&n, type, data, b, len);
    sprintf(decimalData, "%ld", n);
}

// -----------------------------------------------------------------------------
// User interface and testing.

// Print the sign bit of a floating-point data.
void printFloatingPointSignBit(double data) {
    if (data < 0) printf("1 sign bit: %d (negative)\n", 1);
    else printf("1 sign bit: %d (positive)\n", 0);
}

// Print the exponent part represented in binary.
// (1 << (bits - 1)) - 1 = 127 , if bits == EXPONENT_FLOAT_BITS  ( = 8 )
//                       = 1023, if bits == EXPONENT_DOUBLE_BITS ( = 11)
// n is the exponent of (2 ^ n), which is kind of a "scientific
// notation", like decimals (x * 10 ^ y), but in binary: x * (2 ^ y).
void printExponent(double data, const int bits) {
    double absData = myAbs(data);
    int n = absData? (1 << (bits - 1)) - 1 + log2(absData) : (1 << (bits - 1));
    for (int i = bits - 1; i >= 0; i--) {
        printf("%d ", (n >> i) & 1);
    }
    printf("\n");
}

// Print the mantissa part represented in binary.
void printMantissa(double data, const int bits) {
    int exponentBits;
    int bitsCounter = 0;
    double mantissa = myAbs(frexp(data, &exponentBits));
    mantissa *= 2; //Get rid of the 1st bit of mantissa. (sign)
    if (mantissa >= 1) mantissa -= floor(mantissa);
    while (bitsCounter < bits) {
        mantissa *= 2;
        if (mantissa >= 1) {
            printf("%d ", 1);
            mantissa -= floor(mantissa);
        }
        else printf("%d ", 0);
        bitsCounter++;
    }
    printf("\n");
}

// Print the components of a floating point number represented in binary.
void printComponents(double data, const int expBits, const int mantissaBits) {
    printFloatingPointSignBit(data);
    printf("%d exponent bits: ", expBits);
    printExponent(data, expBits);
    printf("%d mantissa bits: ", mantissaBits);
    printMantissa(data, mantissaBits);
}

// Print sign bit of an integer data.
void printIntegerSignBit(long data, int bits) {
    if ((data >> (bits - 1)) & 1) printf("1 sign bit: %d (negative)\n", 1);
    else printf("1 sign bit: %d (positive)\n", 0);
}

// Print binary digits of the given (integer) data.
void printBinaryDigits(long data, int bits) {
    printf("%d binary digits: ", bits - 1);
    for(int i = bits - 2; i >= 0; i--) {
        printf("%ld ", (data >> i) & 1);
    }
    printf("\n");
}

// Print (visualise) an integer data.
void printInteger(int type, long data) {
    if (type != Impossible) {
        int bits = (1 << type) * BYTE;
        printIntegerSignBit(data, bits);
        printBinaryDigits(data, bits);
    }
    else printf("Impossible! Data is too large or invalid.\n");
}

// Print (visualise) a floating-point data.
void printFloatingPoint(int type, double data) {
    if (type == Float) {
        printComponents(data, EXPONENT_FLOAT_BITS, MANTISSA_FLOAT_BITS);
    }
    else if (type == Double) {
      printComponents(data, EXPONENT_DOUBLE_BITS, MANTISSA_DOUBLE_BITS);
    }
    else printf("Impossible! Data is too large or invalid.\n");
}

// Convert the data, given an integer data type, and visualise the result.
void visualiseInteger(int type, const char args[]) {
    bool foundData = false;
    long data  = convertIntegerData(args, &foundData);
    int result = convertIntegerDataType(type, data, foundData);
    printInteger(result, data);
}

// Convert the data, given a floating-point data type and visualise the result.
void visualiseFloatingPoint(int type, const char args[]) {
    bool foundData = false;
    double data = convertFloatingPointData(args, &foundData);
    int result  = convertFloatingPointDataType(type, data, foundData);
    printFloatingPoint(result, data);
}

// A replacement for the library assert function.
void assert(int line, bool b) {
    if (b) return;
    printf("The test on line %d fails.\n", line);
    exit(1);
}

// Check if I haven't changed the data type constants.
void checkConstants() {
    assert(__LINE__, Char==0 && Short==1 && Int==2 && Long==3);
    assert(__LINE__, Float==4 && Double==5);
    assert(__LINE__, Impossible==6);
}

// Check conversion of data.
void testConvertIntegerData() {
    bool any = false;
    assert(__LINE__, convertIntegerData("0", &any) == 0);
    assert(__LINE__, convertIntegerData("12345678", &any) == 12345678);
    assert(__LINE__, convertIntegerData("2147483647", &any) == 2147483647);
    assert(__LINE__, convertIntegerData("2147483648", &any) == 2147483648);
    assert(__LINE__, convertIntegerData("-2147483648", &any) == -2147483648);
    assert(__LINE__, convertIntegerData("9223372036854775807", &any) == 9223372036854775807);
    assert(__LINE__, convertIntegerData("92233720368547758074234324234", &any) == -1);
    assert(__LINE__, convertIntegerData("9223372036854775808", &any) == -1);
    assert(__LINE__, convertIntegerData("04", &any) == -1);
    assert(__LINE__, convertIntegerData(" 3", &any) == -1);
    assert(__LINE__, convertIntegerData("x", &any) == -1);
    assert(__LINE__, convertIntegerData("4x", &any) == -1);
    assert(__LINE__, convertIntegerData("3.1415", &any) == -1);
}

// Check conversion of data.
void testConvertFloatingPointData() {
    bool any = false;
    assert(__LINE__, convertFloatingPointData("0", &any) ==0.000000);
    assert(__LINE__, convertFloatingPointData("-3.1415", &any) == -3.141500);
    assert(__LINE__, convertFloatingPointData("12345678", &any) == 12345678.000000);
    assert(__LINE__, convertFloatingPointData("2147483648", &any) == 2147483648.0000000);
    assert(__LINE__, convertFloatingPointData("340282346638528859811704183484516925440.000000", &any) == FLT_MAX);
    assert(__LINE__, convertFloatingPointData("179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000", &any) == DBL_MAX);
    assert(__LINE__, convertFloatingPointData("123456789123456.125", &any) == 123456789123456.125);
    assert(__LINE__, convertFloatingPointData("1234567891234567.125", &any) == -1);
    assert(__LINE__, convertFloatingPointData("1234567891234567891233223456789.5", &any) == -1);
    assert(__LINE__, convertFloatingPointData("x", &any) == -1.000000);
    assert(__LINE__, convertFloatingPointData(" 3.2", &any) == -1.000000);
    assert(__LINE__, convertFloatingPointData("4.x", &any) == -1.000000);
}

// Check conversion of types.
void testConvertType() {
    assert(__LINE__, convertDataType("char") == Char);
    assert(__LINE__, convertDataType("short") == Short);
    assert(__LINE__, convertDataType("int") == Int);
    assert(__LINE__, convertDataType("long") == Long);
    assert(__LINE__, convertDataType("Char") == -1);
    assert(__LINE__, convertDataType("INT") == -1);
}

// Check conversion of data from any base to decimal.
void testConvertDecimal() {
    char any[MAX_DIGITS_LONG];
    convertDecimal(Long, "DEADBEEF", "16", any); assert(__LINE__, strcmp(any, "3735928559") == 0);
    convertDecimal(Int, "7FFFFFFF", "16", any); assert(__LINE__, strcmp(any, "2147483647") == 0);
    convertDecimal(Int, "-7FFFFFFF", "16", any); assert(__LINE__, strcmp(any, "-2147483647") == 0);
    convertDecimal(Long, "7FFFFFFFFFFFEFFF", "16", any); assert(__LINE__, strcmp(any, "9223372036854771712") == 0);
    convertDecimal(Int, "17777777777", "8", any); assert(__LINE__, strcmp(any, "2147483647") == 0);
    convertDecimal(Int, "-17777777777", "8", any); assert(__LINE__, strcmp(any, "-2147483647") == 0);
    convertDecimal(Int, "01111111111111111111111111111111", "2", any); assert(__LINE__, strcmp(any, "2147483647") == 0);
    convertDecimal(Int, "11111111111111111111111111111111", "2", any); assert(__LINE__, strcmp(any, "-1") == 0);
    convertDecimal(Char, "2020", "3", any); assert(__LINE__, strcmp(any, "60") == 0);
    convertDecimal(Char, "1333", "4", any); assert(__LINE__, strcmp(any, "127") == 0);
    convertDecimal(Char, "1002", "5", any); assert(__LINE__, strcmp(any, "127") == 0);
    convertDecimal(Short, "411411", "6", any); assert(__LINE__, strcmp(any, "32767") == 0);
    convertDecimal(Short, "164350", "7", any); assert(__LINE__, strcmp(any, "32767") == 0);
    convertDecimal(Short, "-48847", "9", any); assert(__LINE__, strcmp(any, "-32767") == 0);
    convertDecimal(Int, "2019", "10", any); assert(__LINE__, strcmp(any, "2019") == 0);
    convertDecimal(Int, "A02220281", "11", any); assert(__LINE__, strcmp(any, "2147483647") == 0);
    convertDecimal(Int, "-4BB2308A7", "12", any); assert(__LINE__, strcmp(any, "-2147483647") == 0);
    convertDecimal(Long, "10B269549075432107", "13", any); assert(__LINE__, strcmp(any, "9223372036854771712") == 0);
    convertDecimal(Long, "4340724C6C71DB0C0", "14", any); assert(__LINE__, strcmp(any, "9223372036854771712") == 0);
    convertDecimal(Long, "-160E2AD32463654C7", "15", any); assert(__LINE__, strcmp(any, "-9223372036854771712") == 0);
}

// Check char
void testChar() {
    assert(__LINE__, convertIntegerDataType(Char, -1, 1) == Char);
    assert(__LINE__, convertIntegerDataType(Char, SCHAR_MAX, 1) == Char);
    assert(__LINE__, convertIntegerDataType(Char, SCHAR_MIN, 1) == Char);
    assert(__LINE__, convertIntegerDataType(Char, -1, 0) == Impossible);
    assert(__LINE__, convertIntegerDataType(Char, 128, 1) == Impossible);
    assert(__LINE__, convertIntegerDataType(Char, -129, 1) == Impossible);
}

// Check short
void testShort() {
    assert(__LINE__, convertIntegerDataType(Short, -1, 1) == Short);
    assert(__LINE__, convertIntegerDataType(Short, SHRT_MAX, 1) == Short);
    assert(__LINE__, convertIntegerDataType(Short, SHRT_MIN, 1) == Short);
    assert(__LINE__, convertIntegerDataType(Short, -1, 0) == Impossible);
    assert(__LINE__, convertIntegerDataType(Short, 32768, 1) == Impossible);
    assert(__LINE__, convertIntegerDataType(Short, -32769, 1) == Impossible);
}

// Check int
void testInt() {
    assert(__LINE__, convertIntegerDataType(Int, -1, 1) == Int);
    assert(__LINE__, convertIntegerDataType(Int, INT_MAX, 1) == Int);
    assert(__LINE__, convertIntegerDataType(Int, INT_MIN, 1) == Int);
    assert(__LINE__, convertIntegerDataType(Int, -1, 0) == Impossible);
    assert(__LINE__, convertIntegerDataType(Int, 2147483648, 1) ==Impossible);
    assert(__LINE__, convertIntegerDataType(Int, -2147483649, 1) == Impossible);
}

// Check long
void testLong() {
    assert(__LINE__, convertIntegerDataType(Long, -1, 1) == Long);
    assert(__LINE__, convertIntegerDataType(Long, LONG_MIN, 1) == Long);
    assert(__LINE__, convertIntegerDataType(Long, LONG_MAX, 1) == Long);
}

// Check float
void testFloat() {
    assert(__LINE__, convertFloatingPointDataType(Float, FLT_MAX, 1) == Float);
    assert(__LINE__, convertFloatingPointDataType(Float, -FLT_MAX, 1) == Float);
    assert(__LINE__, convertFloatingPointDataType(Float, DBL_MAX, 1) == Impossible);
    assert(__LINE__, convertFloatingPointDataType(Float, -DBL_MAX, 1) == Impossible);
}

// Check double
void testDouble() {
    assert(__LINE__, convertFloatingPointDataType(Double, -1, 1) == Double);
    assert(__LINE__, convertFloatingPointDataType(Double, DBL_MAX, 1) == Double);
    assert(__LINE__, convertFloatingPointDataType(Double, -DBL_MAX, 1) == Double);
}

// Check for correct handling of data overflow.
void checkImpossible() {
    assert(__LINE__, convertIntegerDataType(Char, 128, 0) == Impossible);
    assert(__LINE__, convertIntegerDataType(Char, 128, 1) == Impossible);
    assert(__LINE__, convertIntegerDataType(Char, 32767, 1) == Impossible);
    assert(__LINE__, convertIntegerDataType(Short, 32768, 1) == Impossible);
    assert(__LINE__, convertIntegerDataType(Short, 2147483647, 1) == Impossible);
    assert(__LINE__, convertIntegerDataType(Int, 2147483648, 1) == Impossible);
    assert(__LINE__, convertIntegerDataType(Int, -2147483649, 1) == Impossible);
    assert(__LINE__, convertIntegerDataType(Int, 9223372036854775807, 1) == Impossible);
    assert(__LINE__, convertIntegerDataType(Int, 9223372036854775807, 0) == Impossible);
    assert(__LINE__, convertFloatingPointDataType(Float, DBL_MAX, 1) == Impossible);
    assert(__LINE__, convertFloatingPointDataType(Float, DBL_MAX, 0) == Impossible);
}

// Run tests on the visualise program.
void test() {
    checkConstants();
    testConvertType();
    testConvertIntegerData();
    testConvertFloatingPointData();
    testConvertDecimal();
    testChar();
    testShort();
    testInt();
    testLong();
    testFloat();
    testDouble();
    checkImpossible();
    printf("All tests passed.\n");
}

// Run the program or, if there are no arguments, test it.
int main(int n, char *args[n]) {
    setbuf(stdout, NULL);
    if (n == 1) {
        test();
    }
    else if (n == 3) {
        int type = convertDataType(args[1]);
        if (isType(type) == Integer) {
            visualiseInteger(type, args[2]);
        }
        else if (isType(type) == FloatingPoint) {
            visualiseFloatingPoint(type, args[2]);
        }
    }
    else if (n == 4) {
        int type = convertDataType(args[1]);
        if (isType(type) == Integer) {
            char decimalData[MAX_DIGITS_LONG];
            convertDecimal(type, args[2], args[3], decimalData);
            visualiseInteger(type, decimalData);
        }
    }
    else {
        fprintf(stderr, "Use e.g.: ./visualise char 7\n");
        fprintf(stderr, "Use e.g.: ./visualise long DEADBEEF 16\n");
        exit(1);
    }
    return 0;
}
