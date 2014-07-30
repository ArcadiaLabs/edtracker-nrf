#include <stdbool.h>
#include <compiler_mcs51.h>

#include "mymath.h"

// These are specialized versions of math functions from the SDCC library,
// used in a desperate attempt to save some valuable flash space.
// This saves us about 400 bytes of flash

// The only thing I've changed is that the functions are not declared reentrant,
// and a making asincos() into asin().

#define PI          3.1415926536
#define TWO_PI      6.2831853071
#define HALF_PI     1.5707963268
#define QUART_PI    0.7853981634

#define P0atan -0.4708325141E+0
#define P1atan -0.5090958253E-1
#define Q0atan  0.1412500740E+1
#define Q1atan  0.1000000000E+1

#define Patan(g,f)		((P1atan*g+P0atan)*g*f)
#define Qatan(g)		(Q1atan*g+Q0atan)

#define K1  0.2679491924		// 2-sqrt(3)
#define K2  0.7320508076		// sqrt(3)-1
#define K3  1.7320508076		// sqrt(3)

#define EPS		244.14062E-6

#define P1asin		0.933935835E+0
#define P2asin	   -0.504400557E+0
#define Q0asin		0.560363004E+1
#define Q1asin	   -0.554846723E+1
#define Q2asin		0.100000000E+1

#define Pasin(g)	(P2asin * g + P1asin)
#define Qasin(g)	((Q2asin * g + Q1asin) * g + Q0asin)

union float_long
{
    float f;
    long l;
};

float fabs(float x)
{
    union float_long fl;

    fl.f = x;
    fl.l &= 0x7fffffff;
    return fl.f;
}

float ldexp(float x, int pw2)
{
    union float_long fl;
    long e;

    fl.f = x;

    e=(fl.l >> 23) & 0x000000ff;
    e+=pw2;
    fl.l= ((e & 0xff) << 23) | (fl.l & 0x807fffff);

    return(fl.f);
}

float atanf(float x)
{
    float f, r, g;
    int n=0;
	static __code float a[]={  0.0, 0.5235987756, 1.5707963268, 1.0471975512 };

    f=fabs(x);
    if(f>1.0)
    {
        f=1.0/f;
        n=2;
    }
    if(f>K1)
    {
        f=((K2*f-1.0)+f)/(K3+f);
        // What it is actually wanted is this more accurate formula,
        // but SDCC optimizes it and then it does not work:
        // f=(((K2*f-0.5)-0.5)+f)/(K3+f);
        n++;
    }
    if(fabs(f)<EPS) r=f;
    else
    {
        g=f*f;
        r=f+Patan(g,f)/Qatan(g);
    }
    if(n>1) r=-r;
    r+=a[n];
    if(x<0.0) r=-r;
    return r;
}

float atan2(float x, float y)
{
    float r;

    if ((x==0.0) && (y==0.0))
    {
        //errno = EDOM;
        return 0.0;
    }

    if (fabs(y) >= fabs(x))
    {
        r=atanf(x/y);
        if(y<0.0) r+=(x>=0?PI:-PI);
    } else {
        r=-atanf(y/x);
        r+=(x<0.0?-HALF_PI:HALF_PI);
    }

    return r;
}

float frexp(float x, int *pw2)
{
    union float_long fl;
    long int i;

    fl.f=x;
    /* Find the exponent (power of 2) */
    i  = ( fl.l >> 23) & 0x000000ff;
    i -= 0x7e;
    *pw2 = i;
    fl.l &= 0x807fffff; /* strip all exponent bits */
    fl.l |= 0x3f000000; /* mantissa between 0.5 and 1 */
    return(fl.f);
}

float sqrt(float x)
{
    float f, y;
    int n;

    if (x==0.0) return x;
    else if (x==1.0) return 1.0;
    else if (x<0.0)
    {
        //errno=EDOM;
        return 0.0;
    }
    f=frexp(x, &n);
    y=0.41731+0.59016*f; /*Educated guess*/
    /*For a 24 bit mantisa (float), two iterations are sufficient*/
    y+=f/y;
    y=ldexp(y, -2) + f/y; /*Faster version of 0.25 * y + f/y*/

    if (n&1)
    {
        y*=0.7071067812;
        ++n;
    }
    return ldexp(y, n/2);
}

float asin(float x)
{
    float y, g, r;
    unsigned char i;
    bool quartPI = false;

    static const float a[2] = { 0.0, QUART_PI };
    static const float b[2] = { HALF_PI, QUART_PI };

         if (x == 1.0) return  HALF_PI;
    else if (x ==-1.0) return -HALF_PI;
    else if (x == 0.0) return 0.0;
	
    y = fabs(x);
    if (y < EPS)
    {
        r = y;
    }
    else
    {
        if (y > 0.5)
        {
            quartPI = true;
            if (y > 1.0)
                return 0.0;
            g = (0.5 - y) + 0.5;
            g = ldexp(g, -1);
            y = sqrt(g);
            y = -(y + y);
        }
        else
        {
            g = y * y;
        }
        r = y + y * ((Pasin(g) * g) / Qasin(g));
    }
    i = quartPI;
    //if (isacos)
    //{
    //    if (x < 0.0)
    //        r = (b[i] + r) + b[i];
    //    else
    //        r = (a[i] - r) + a[i];
    //}
    //else
    {
        r = (a[i] + r) + a[i];
        if (x < 0.0)
            r = -r;
    }
    return r;
}
