#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <wchar.h>
#include <unistd.h>
#include <locale.h>

void affiche(int *route, int n)
{
    wint_t v[] = { ' ', (wint_t)128663, (wint_t)128665,
                   (wint_t)128661, (wint_t)127949 };

    setlocale(LC_ALL, "");

    // Effacer la ligne précédente
    printf("\r");               
    printf("\033[K");            // Efface la ligne entière

    // Affichage de la route
    for (int i = 0; i < n; i++)
    {
        printf("%lc", v[route[i]]);
        if (route[i] == 0)
            printf("%lc", v[route[i]]);
    }

    fflush(stdout);   // Force l’affichage
    usleep(150000);   // 0.15 sec (plus fluide que 1 sec)
}



void echange(int *a, int *b){
    int tmp = *a;
    *a = *b;
    *b = tmp;
}
 
void afficher_entiers(int *tab,int n)
{
    for (int i=0;i<n;i++)
    {
        printf("%d ",tab[i]);
    }
    printf("\n");
}

void afficher_floats(float *tab,int n)
{
    for (int i=0;i<n;i++)
    {
        printf("%.2f ",tab[i]);
    }
    printf("\n");
}

int *copie(int *tab, int n)
{
    int *new_tab = malloc(n * sizeof(int));
    for (int i=0;i<n;i++)
    {
    
        new_tab[i] = tab[i];
    }
    return new_tab;
}   

float *premiers_termes(float u0, float a, float b, int n)
{
    float *tab = malloc(n * sizeof(float));
    tab[0] = u0;
    for (int i=1;i<n;i++)
    {
        tab[i] = a * tab[i-1] + b;
    }
    return tab;
}

bool *eratosthene(int n)
{
    bool *tab = malloc((n + 1) * sizeof(bool));
    if (tab == NULL) return NULL;

    // Initialisation
    tab[0] = false;
    tab[1] = false;
    for (int i = 2; i <= n; i++)
        tab[i] = true;

    // Crible
    for (int i = 2; i <= n; i++) {
        if (tab[i]) {
            for (int j = 2 * i; j <= n; j += i)
                tab[j] = false;
        }
    }

    return tab;
}

int *prochaine_ligne(int *t, int n)
{
    int *res = malloc((n + 2) * sizeof(int));
    if (res == NULL) return NULL;
    res[0] = 1;
    res[n+1]=1;
    for (int i = 1; i<= n; i++)
        res[i] = t[i-1] + t[i];
    return res;
}

int **triangle_de_pascal(int n)
{
    int **T = malloc(n * sizeof(int *));
    if (T == NULL) return NULL;

    T[0] = malloc(1 * sizeof(int));
    T[0][0] = 1;

    for (int i = 1; i < n; i++) {
        T[i] = prochaine_ligne(T[i - 1], i - 1);
    }

    return T;
}

int vehicule(int f)
{
    if (rand() % f == 0)
        return 0;
    else
        return (rand() % 4) + 1;
}

int *init(int f, int n)
{
    int *t = malloc(n * sizeof(int));
    if (t == NULL) return NULL;

    for (int i = 0; i < n; i++)
        t[i] = vehicule(f);

    return t;
}

int *next(int *route, int n, int f)
{
    int *new = malloc(n * sizeof(int));
    if (new == NULL) return NULL;

    new[0] = 0;

    for (int i = 1; i < n; i++) {
        if (route[i] != 0 && new[i-1] == 0)
            new[i-1] = route[i];
        else
            new[i] = route[i];
    }

    if (route[n-1] == 0)
        new[n-1] = vehicule(f);
    else
        new[n-1] = route[n-1];

    return new;
}


int main()
{
    int x =50;
    int y =40;
    int a=100;
    echange(&x,&y);
    printf("x=%d et y=%d\n",x,y);
    afficher_entiers((int[]){1,3,5,7,9},5);
    int tab1[] = {2,4,6,8,10};
    int *tab2 = copie(tab1, 5);
    afficher_entiers(tab2,5);
    afficher_floats(premiers_termes(1.0,2.0,1.0,5),5);
    bool *t = eratosthene(a);
    for (int i=2; i<= a; i++)
        if (t[i])printf("%d ",i);
    
    free(t);

    int N = 10;
    int **T = triangle_de_pascal(N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j <= i; j++)
            printf("%d ", T[i][j]);
        printf("\n");
    }
    for (int i = 0; i < N; i++)
        free(T[i]);
    free(T);
    srand(time(0));
    int n = 60;
    int f = 2;
    int *route = init(f, n);
    for (int t = 0; t <= 30; t++) {
        affiche(route, n);
        int *next_route = next(route, n, f);
        free(route);
        route = next_route;
    }
    free(route);
    return 0;
}   