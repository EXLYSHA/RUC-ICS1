/* WARNING: Do not include any other libraries here,
 * otherwise you will get an error while running test.py
 * You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.
 *
 * Using printf will interfere with our script capturing the execution results.
 * At this point, you can only test correctness with ./btest.
 * After confirming everything is correct in ./btest, remove the printf
 * and run the complete tests with test.py.
 */

/*
 * bitXor - x ^ y using only ~ and &
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 7
 *   Difficulty: 1
 */
int bitXor(int x, int y) {
    return ~(x & y) & ~ (~x & ~y);
}

/*
 * samesign - Determines if two integers have the same sign.
 *   0 is not positive, nor negative
 *   Example: samesign(0, 1) = 0, samesign(0, 0) = 1
 *            samesign(-4, -5) = 1, samesign(-4, 5) = 0
 *   Legal ops: >> << ! ^ && if else &
 *   Max ops: 12
 *   Difficulty: 2
 *
 * Parameters:
 *   x - The first integer.
 *   y - The second integer.
 *
 * Returns:
 *   1 if x and y have the same sign , 0 otherwise.
 */
int samesign(int x, int y) {
    return !( (x ^ y) >> 31 ) & !( !!x ^ !!y );
}

/*
 * logtwo - Calculate the base-2 logarithm of a positive integer using bit
 *   shifting. (Think about bitCount)
 *   Note: You may assume that v > 0
 *   Example: logtwo(32) = 5
 *   Legal ops: > < >> << |
 *   Max ops: 25
 *   Difficulty: 4
 */
int logtwo(int v) {
    int x=0,t=0;
    t = ((v >> 16) > 0) << 4; 
    x |= t;
    v >>= t;
    t = ((v >> 8) > 0) << 3;
    x |= t;
    v >>= t;
    t = ((v >> 4) > 0) << 2;
    x |= t;
    v >>= t;
    t = ((v >> 2) > 0) << 1;
    x |= t;
    v >>= t;
    t = ((v >> 1) > 0);
    x |= t;
    v >>= t;
    return x;
}

/*
 *  byteSwap - swaps the nth byte and the mth byte
 *    Examples: byteSwap(0x12345678, 1, 3) = 0x56341278
 *              byteSwap(0xDEADBEEF, 0, 2) = 0xDEEFBEAD
 *    Note: You may assume that 0 <= n <= 3, 0 <= m <= 3
 *    Legal ops: ! ~ & ^ | + << >>
 *    Max ops: 17
 *    Difficulty: 2
 */
int byteSwap(int x, int n, int m) {
    n <<= 3;
    m <<= 3;
    int mask1 = (x >> n) & 0xff;
    int mask2 = (x >> m) & 0xff;
    x &= ~((0xff << m) | (0xff << n));
    x = (mask1 << m) | x | (mask2 << n);
    return x;
}

/*
 * reverse - Reverse the bit order of a 32-bit unsigned integer.
 *   Example: reverse(0xFFFF0000) = 0x0000FFFF reverse(0x80000000)=0x1 reverse(0xA0000000)=0x5
 *   Note: You may assume that an unsigned integer is 32 bits long.
 *   Legal ops: << | & - + >> for while ! ~ (You can define unsigned in this function)
 *   Max ops: 30
 *   Difficulty: 3
 */
unsigned reverse(unsigned v) {
    unsigned t = 0;
    int i = 0;
    while (i != 32)
    {
        t += (v & 1) << (31 - i);
        i += 1;
        v >>= 1;
    }
    return t;
}

/*
 * logicalShift - shift x to the right by n, using a logical shift
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Note: You can assume that 0 <= n <= 31
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Difficulty: 3
 */
int logicalShift(int x, int n) {
    return (x >> n) & ~(((x >> 31) << 31) >> n << 1);
}

/*
 * leftBitCount - returns count of number of consective 1's in left-hand (most) end of word.
 *   Examples: leftBitCount(-1) = 32, leftBitCount(0xFFF0F0F0) = 12,
 *             leftBitCount(0xFE00FF0F) = 7
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 50
 *   Difficulty: 4
 */
int leftBitCount(int x) {
    int p = 0, t = 0;
    p += !(x ^ 0xffffffff);
    t = !(((x >> 16) & 0xffff) ^ 0xffff);
    p += t << 4;
    x >>= (!t) << 4;
    t = !(((x >> 8) & 0xff) ^ 0xff);
    p += t << 3;
    x >>= (!t) << 3;
    t = !(((x >> 4) & 0xf) ^ 0xf);
    p += t << 2;
    x >>= (!t) << 2;
    t = !(((x >> 2) & 0x3) ^ 0x3);
    p += t << 1;
    x >>= (!t) << 1;
    t = !(((x >> 1) & 0x1) ^ 0x1);
    p += t;
    x >>= !t;
    return p;
}

/*
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but it is to be interpreted as
 *   the bit-level representation of a single-precision floating point values.
 *   Legal ops: if else while for & | ~ + - >> << < > ! ==
 *   Max ops: 30
 *   Difficulty: 4
 */
unsigned float_i2f(int x) {
    unsigned s = x & 0x80000000, m = 0, all = 0;
    if (x == -2147483648)
    {
        return 0xcf000000;
    }

    if (s != 0)
    {
        x = -x;
    }
    m = x << 1;
    int e = 0;
    while (!(m & 0x80000000))
    {
        if (e == 157)
        {
            break;
        }
        e += 1;
        m <<= 1;
    }
    m <<= 1;

    int temp = 0;
    if ((m & 0x1FF) > 0x100)
    {
        if (m + 0x100 < m)
        {
            temp = 1;
        }
        m += 0x100;
    }
    else if ((m & 0x1FF) == 0x100)
    {
        if ((m >> 9) & 1)
        {
            if (m + 0x100 < m)
            {
                temp = 1;
            }
            m += 0x100;
        }
    }
    m >>= 9;
    unsigned exp = 157 - e + temp;
    all = s + (exp << 23) + m;
    return all;
}

/*
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: & >> << | if > < >= <= ! ~ else + ==
 *   Max ops: 30
 *   Difficulty: 4
 */
unsigned floatScale2(unsigned uf) {
    unsigned s = uf & 0x80000000, exp = uf & 0x7F800000, m = uf & 0x007FFFFF;
    if (exp == 0x7F800000)
    {
        return uf;
    }
    else if (exp == 0)
    {
        m = m << 1;
        return s | m;
    }
    else
    {
        exp += 0x00800000;
        return s | exp | m;
    }
}

/*
 * float64_f2i - Convert a 64-bit IEEE 754 floating-point number to a 32-bit signed integer.
 *   The conversion rounds towards zero.
 *   Note: Assumes IEEE 754 representation and standard two's complement integer format.
 *   Parameters:
 *     uf1 - The lower 32 bits of the 64-bit floating-point number.
 *     uf2 - The higher 32 bits of the 64-bit floating-point number.
 *   Returns:
 *     The converted integer value, or 0x80000000 on overflow, or 0 on underflow.
 *   Legal ops: >> << | & ~ ! + - > < >= <= if else
 *   Max ops: 60
 *   Difficulty: 3
 */
int float64_f2i(unsigned uf1, unsigned uf2) {
    int s = (uf2 >> 31) & 1,exp = (uf2 >> 20) & 0x7FF;
    int m_hi = uf2 & 0xFFFFF;
    unsigned m_lo = uf1;
    int E = exp - 1023;
    if (!(exp - 0x7FF)) {
        return 0x80000000;
    }

    if (E < 0) {
        return 0;
    }
    if (E > 31) {
        return 0x80000000;
    }

    int m1_hi = m_hi | 0x100000;
    int in;
    if (E >= 20) {
        in = (m1_hi << (E - 20)) | (m_lo >> (32 - (E - 20)));
    }
    else {
        in = m1_hi >> (20 - E);
    }
    if (s) {
        in = -in;
    }
    return in;
}

/*
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 *
 *   Legal ops: < > <= >= << >> + - & | ~ ! if else &&
 *   Max ops: 30
 *   Difficulty: 4
 */
unsigned floatPower2(int x) {
    int exp = x + 127;
    unsigned result;
    if (exp >= 255) {
        return 0x7F800000;
    }
    if (exp <= 0) {
        return 0;
    }
    result = exp << 23;
    return result;
}
