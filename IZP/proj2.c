#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include <errno.h>

// Deklarace funkc�
double taylor_log(double x, unsigned int n);
double cfrac_log(double x, unsigned int n);

// Matematick� funkce pro v�po�et libocoln� mocniny
double math_pow(double value, int exp)
{
    double origin_value = value;

    // Cyklus opakuj�c� n�soben� z�kladu, stejn�m ��slem n-kr�t
    for( int i = 1; i < exp; ++i )
    {
        value *= origin_value;
    }

    return value;
}
// Matematick� funkce absolutn� hodnoty
double math_abs(double x)
{
    if( x > 0)
    // V�tev pro kladn� ��sla
    {
        return x;
    }
    else
    // V�tev pro z�porn� ��sla
    {
        return -x;
    }
}
// Funkce pro v�po�et p�irozen�ho logaritmu pomoc� Taylorova polynomu
double taylor_log(double x, unsigned int n)
{
    // Zkontrolov�n� mezn�ch hodnot
    if(x == 0)
    {
        return -INFINITY;
    }
    else if ( x==1 )
    {
        return 0;
    }
    else if( isnan(x) || x < 0 || x==-INFINITY)
    {
        return NAN;

    }
    else if( isinf(x) )
    {
        return INFINITY;
    }

    // Za��tek v�po�t�
    double result = 0;
    // Cyklus s��taj�c� jednotliv� �leny Taylorova polynomu od prvn�ho po n-t�
    if( x > 0 && x <= 1)
    {
        for( unsigned int i = 1 ; i <= n ; ++i )
            result += -math_pow(1-x, i)/i;
    }
    else if(x > 1)
    {
        for( unsigned int i = 1 ; i <= n ; ++i )
            result += math_pow(( x-1 ) / x, i) / i;
    }
    return result;
}

// Funkce pro v�po�et p�irozen�ho logaritmu pomoc� z�et�zen�ch zlomk�
double cfrac_log(double x, unsigned int n)
{
    // Zkontrolov�n� mezn�ch hodnot
    if(x == 0)
    {
        return -INFINITY;
    }
    else if ( x==1 )
    {
        return 0;
    }
    else if( isnan(x) || x < 0 || x==-INFINITY)
    {
        return NAN;
    }
    else if( isinf(x) )
    {
        return INFINITY;
    }
    // Nejzano�en�j�� jmenovatel zlomku, kter� je�t� bereme v potaz
    double cf = 2*n - 1;

    // Pomocn� prom�nn� pro zrychlen� a zp�ehledn�n� v�po�t�
    double q = (x-1)/(x+1);

    // Cyklus po��taj�c� n-t� �leny z�et�zen�ch zlomk�
    // P�ed v�po�tem je n zmen�eno o 1, kv�li po��te�n�mu zjednodu�en� cf
    while( --n > 0 )
    {
        cf = 2*n - 1 - q*q*n*n/cf;
    }

    // Fin�ln� vyd�len� �itatele vypo��tan�m jmenovatelem
    return 2*q/cf;

}
unsigned int min_iter_taylor_log(double x, double eps)
{
    unsigned int n = 0;

    // Validace hodnot
    if( isinf(x) || isnan(x) || x == 0 )
    {
        return 0;
    }

    while( math_abs(math_abs(taylor_log(x, ++n)) - math_abs(log(x))) > eps );

    return n;
}
// Funkce pro v�po�et nejmen��ho po�tu iterac� nutn�ch pro danou p�esnost funkce taylor_log oproti funkci log
unsigned int iter_taylor_log(double a, double b, double eps)
{
    unsigned int m = min_iter_taylor_log(a, eps);
    unsigned int n = min_iter_taylor_log(b, eps);

    // Vr�t� vy��� ze dvou ��sel
    return ( m > n ? m : n );
}

unsigned int min_iter_cfrac_log(double x, double eps)
{
    unsigned int n = 0;

    // Validace hodnot
    if( isinf(x) || isnan(x) || x == 0 )
    {
        return 0;
    }

    while( math_abs(math_abs(cfrac_log(x, ++n)) - math_abs(log(x))) > eps );

    return n;
}
// Funkce pro v�po�et nejmen��ho po�tu iterac� nutn�ch pro danou p�esnost funkce cfrac_log oproti funkci log
unsigned int iter_cfrac_log(double a, double b, double eps)
{
    unsigned int m = min_iter_cfrac_log(a, eps);;
    unsigned int n = min_iter_cfrac_log(b, eps);;

    // Vr�t� vy��� ze dvou ��sel
    return (  m > n ? m : n );
}

// Funke tisknouc� hodnoty pro p��znak --log
bool print_log(double x, unsigned int n)
{
    printf("       log(%g) = %.12g\n", x, log(x));
    printf("    cf_log(%g) = %.12g\n", x, cfrac_log(x, n));
    printf("taylor_log(%g) = %.12g\n", x, taylor_log(x, n));
    return true;
}

// Funkce tisknouc� hodnoty pro p��znak --iter
bool print_iter(double a, double b, double eps)
{

    // P�edv�po�et nejmen��ho mo�n�ho po�tu iterac� pro danou p�esnost
    unsigned int cf_min = iter_cfrac_log(a, b, eps);
    unsigned int taylor_min = iter_taylor_log(a, b, eps);

    printf("       log(%g) = %.12g\n", a, log(a));
    printf("       log(%g) = %.12g\n", b, log(b));
    printf("continued fraction iterations = %i\n", cf_min);
    printf("    cf_log(%g) = %.12g\n", a, cfrac_log(a, cf_min));
    printf("    cf_log(%g) = %.12g\n", b, cfrac_log(b, cf_min));
    printf("taylor polynomial iterations = %i\n", taylor_min);
    printf("taylor_log(%g) = %.12g\n", a, taylor_log(a, taylor_min));
    printf("taylor_log(%g) = %.12g\n", b, taylor_log(b, taylor_min));

    return true;
}

int main(int argc, char * argv[])
{
    // Nastaven� errno na 0
    errno = 0;

    if(argc == 1)
        return 0;

    //V�t�v pro p��znak --log
    if( strcmp(argv[1], "--log") == 0  && argc == 4)
    {
        // Validace hodnot
        double x = strtod(argv[2], NULL);
        double n = strtod(argv[3], NULL);

        // Zkontrolov�n� p�ete�en�
        if(errno == ERANGE || n > UINT_MAX || n <= 0 )
        {
            // Vyps�n� chybov� hl�ky a ukon�en�
            printf("Zadan� hodnoty jsou mimo povolen� interval.\n");
            return 1;
        }

        // Zkontrolov�n� NAN a INF hodnot
        if( isnan(n) || isinf(n) )
        {
            // Vyps�n� chybov� hl�ky a ukon�en�
            printf("Hodnoty INF a NAN nejsou pro po�et iterac� povoleny.\n");
            return 1;
        }
        // Vyti�t�n� hodnot log
        unsigned int i = n;
        print_log(x, i);
    }
    // V�tev pro p��znak --iter
    else if( strcmp(argv[1], "--iter") == 0 && argc == 5)
    {
        // Validace hodnot
        double a = strtod(argv[2], NULL);
        double b = strtod(argv[3], NULL);
        double eps = strtod(argv[4], NULL);

        // Zkontrolov�n� p�ete�en�
        if(errno == ERANGE || eps <= 0)
        {
            // Vyps�n� chybov� hl�ky a ukon�en�
            printf("Zadan� hodnoty jsou mimo povolen� interval.\n");
            return 1;
        }

        // Zkontrolov�n� NAN a INF hodnot
        if( isnan(eps) || isinf(eps) )
        {
            // Vyps�n� chybov� hl�ky a ukon�en�
            printf("Hodnoty INF a NAN nejsou pro hodnotu p�esnoti povoleny.\n");
            return 1;
        }
        // Vyti�t�n� minim�ln�ho po�tu iterac�
        print_iter(a, b, eps);
    }

    return 0;

}
