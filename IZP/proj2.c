#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include <errno.h>

// Deklarace funkcí
double taylor_log(double x, unsigned int n);
double cfrac_log(double x, unsigned int n);

// Matematická funkce pro výpoèet libocolné mocniny
double math_pow(double value, int exp)
{
    double origin_value = value;

    // Cyklus opakující násobení základu, stejným èíslem n-krát
    for( int i = 1; i < exp; ++i )
    {
        value *= origin_value;
    }

    return value;
}
// Matematická funkce absolutní hodnoty
double math_abs(double x)
{
    if( x > 0)
    // Vìtev pro kladná èísla
    {
        return x;
    }
    else
    // Vìtev pro záporná èísla
    {
        return -x;
    }
}
// Funkce pro výpoèet pøirozeného logaritmu pomocí Taylorova polynomu
double taylor_log(double x, unsigned int n)
{
    // Zkontrolování mezních hodnot
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

    // Zaèátek výpoètù
    double result = 0;
    // Cyklus sèítající jednotlivé èleny Taylorova polynomu od prvního po n-tý
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

// Funkce pro výpoèet pøirozeného logaritmu pomocí zøetìzených zlomkù
double cfrac_log(double x, unsigned int n)
{
    // Zkontrolování mezních hodnot
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
    // Nejzanoøenìj¹í jmenovatel zlomku, který je¹tì bereme v potaz
    double cf = 2*n - 1;

    // Pomocná promìnná pro zrychlení a zpøehlednìní výpoètù
    double q = (x-1)/(x+1);

    // Cyklus poèítající n-té èleny zøetìzených zlomkù
    // Pøed výpoètem je n zmen¹eno o 1, kvùli poèáteènímu zjednodu¹ení cf
    while( --n > 0 )
    {
        cf = 2*n - 1 - q*q*n*n/cf;
    }

    // Finální vydìlení èitatele vypoèítaným jmenovatelem
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
// Funkce pro výpoèet nejmen¹ího poètu iterací nutných pro danou pøesnost funkce taylor_log oproti funkci log
unsigned int iter_taylor_log(double a, double b, double eps)
{
    unsigned int m = min_iter_taylor_log(a, eps);
    unsigned int n = min_iter_taylor_log(b, eps);

    // Vrátí vy¹¹í ze dvou èísel
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
// Funkce pro výpoèet nejmen¹ího poètu iterací nutných pro danou pøesnost funkce cfrac_log oproti funkci log
unsigned int iter_cfrac_log(double a, double b, double eps)
{
    unsigned int m = min_iter_cfrac_log(a, eps);;
    unsigned int n = min_iter_cfrac_log(b, eps);;

    // Vrátí vy¹¹í ze dvou èísel
    return (  m > n ? m : n );
}

// Funke tisknoucí hodnoty pro pøíznak --log
bool print_log(double x, unsigned int n)
{
    printf("       log(%g) = %.12g\n", x, log(x));
    printf("    cf_log(%g) = %.12g\n", x, cfrac_log(x, n));
    printf("taylor_log(%g) = %.12g\n", x, taylor_log(x, n));
    return true;
}

// Funkce tisknoucí hodnoty pro pøíznak --iter
bool print_iter(double a, double b, double eps)
{

    // Pøedvýpoèet nejmen¹ího mo¾ného poètu iterací pro danou pøesnost
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
    // Nastavení errno na 0
    errno = 0;

    if(argc == 1)
        return 0;

    //Vìtìv pro pøíznak --log
    if( strcmp(argv[1], "--log") == 0  && argc == 4)
    {
        // Validace hodnot
        double x = strtod(argv[2], NULL);
        double n = strtod(argv[3], NULL);

        // Zkontrolování pøeteèení
        if(errno == ERANGE || n > UINT_MAX || n <= 0 )
        {
            // Vypsání chybové hlá¹ky a ukonèení
            printf("Zadané hodnoty jsou mimo povolený interval.\n");
            return 1;
        }

        // Zkontrolování NAN a INF hodnot
        if( isnan(n) || isinf(n) )
        {
            // Vypsání chybové hlá¹ky a ukonèení
            printf("Hodnoty INF a NAN nejsou pro poèet iterací povoleny.\n");
            return 1;
        }
        // Vyti¹tìní hodnot log
        unsigned int i = n;
        print_log(x, i);
    }
    // Vìtev pro pøíznak --iter
    else if( strcmp(argv[1], "--iter") == 0 && argc == 5)
    {
        // Validace hodnot
        double a = strtod(argv[2], NULL);
        double b = strtod(argv[3], NULL);
        double eps = strtod(argv[4], NULL);

        // Zkontrolování pøeteèení
        if(errno == ERANGE || eps <= 0)
        {
            // Vypsání chybové hlá¹ky a ukonèení
            printf("Zadané hodnoty jsou mimo povolený interval.\n");
            return 1;
        }

        // Zkontrolování NAN a INF hodnot
        if( isnan(eps) || isinf(eps) )
        {
            // Vypsání chybové hlá¹ky a ukonèení
            printf("Hodnoty INF a NAN nejsou pro hodnotu pøesnoti povoleny.\n");
            return 1;
        }
        // Vyti¹tìní minimálního poètu iterací
        print_iter(a, b, eps);
    }

    return 0;

}
