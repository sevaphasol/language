#include "stdio.h"
#include "stdlib.h"

const char* s = "25*10*(3*(25-10*2)+1)$";
int         p = 0;

void SyntaxError();
int GetG();
int GetE();
int GetT();
int GetP();
int GetN();

void SyntaxError()
{
    abort();
}

int GetG()
{
    int val = GetE();
    if (s[p] != '$') {SyntaxError();}
    p++;
    return val;
}

int GetE()
{
    int val1 = GetT();
    while (s[p] == '+' ||
           s[p] == '-')
    {
        int op = s[p];
        p++;
        int val2 = GetT();
        if (op == '+')
            val1 += val2;
        else
            val1 -= val2;
    }
    return val1;
}

int GetT()
{
    int val1 = GetP();
    while (s[p] == '*' ||
           s[p] == '/')
    {
        int op = s[p];
        p++;
        int val2 = GetP();
        if (op == '*')
            val1*=val2;
        else
            val1/=val2;
    }
    return val1;
}

int GetP()
{
    if (s[p] == '(')
    {
        p++;
        int val = GetE();
        if (s[p] != ')')
            SyntaxError();
        p++;
        return val;
    }
    else
    {
        return GetN();
    }
}

int GetN()
{
    int val = 0;
    int old_p = p;
    while ('0' <= s[p] && s[p] <= '9')
    {
        val = val*10 + s[p] - '0';
        p++;
    }
    if (old_p == p)
        SyntaxError();
    return val;
}

int main()
{
    printf("%d\n", GetG());
}
