#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define VSTUPNI_SOUBOR "zavodnici.txt"
#define VYSTUPNI_SOUBOR "vyseldkova_listina.txt"
#define DEL " :\n"
#define POCET_RADKU 150
#define POCET_ZNAKU 100

typedef struct {
    int minuty_prvni;
    float sekundy_prvni;
    int minuty_druhe;
    float sekundy_druhe;
}CAS;

typedef struct {
    int startovni_cislo;
    char prijmeni[20];
    char jmeno[20];
    CAS cas;
}ZAVODNIK;

int nacti_soubor(ZAVODNIK *zavodnik){
    int index_zavodnika = 0;
    FILE *soubor = fopen(VSTUPNI_SOUBOR, "r");
    if(soubor == NULL){
        printf("Soubor %s se nepodarilo otevrit\n", VSTUPNI_SOUBOR);
        return 0;
    } else{
        char radek[POCET_ZNAKU];
        char *cast_radku;
        while (fgets(radek, sizeof(radek), soubor) != NULL){
            if(index_zavodnika != 0){
                int sloupec = 0;
                cast_radku = strtok(radek, DEL);
                while (cast_radku != NULL){
                    switch (sloupec) {
                        case 0:
                            zavodnik[index_zavodnika - 1].startovni_cislo = atoi(cast_radku);
                            break;
                        case 1:
                            strcpy(zavodnik[index_zavodnika - 1].prijmeni, cast_radku);
                            break;
                        case 2:
                            strcpy(zavodnik[index_zavodnika - 1].jmeno, cast_radku);
                            break;
                        case 3:
                            zavodnik[index_zavodnika - 1].cas.minuty_prvni = atoi(cast_radku);
                            break;
                        case 4:
                            zavodnik[index_zavodnika - 1].cas.sekundy_prvni = atof(cast_radku);
                            break;
                        case 5:
                            zavodnik[index_zavodnika - 1].cas.minuty_druhe = atoi(cast_radku);
                            break;
                        case 6:
                            zavodnik[index_zavodnika - 1].cas.sekundy_druhe = atof(cast_radku);
                            break;
                    }
                    cast_radku = strtok(NULL, DEL);
                    sloupec++;
                }
            }
            index_zavodnika++;
        }
        if(fclose(soubor) == EOF){
            printf("Soubor %s se nepodarilo zavrit.\n", soubor);
        }
    }
    return index_zavodnika - 1;
}

float prvni_cas_na_sekundy(CAS cas){
    return (float)cas.minuty_prvni * 60 + cas.sekundy_prvni;
}

float druhe_cas_na_sekundy(CAS cas){
    return (float)cas.minuty_druhe * 60 + cas.sekundy_druhe;
}

float celkove_sekundy(CAS cas){
    return prvni_cas_na_sekundy(cas) + druhe_cas_na_sekundy(cas);
};

CAS prevod_cel_sekund_na_cas(float celkove_sekundy){
    CAS vysledny_cas;
    vysledny_cas.minuty_prvni = ((int)celkove_sekundy % 3600) / 60;
    vysledny_cas.sekundy_prvni = celkove_sekundy - ((float)vysledny_cas.minuty_prvni * 60);
    return vysledny_cas;
}

void serad_zavodniky(ZAVODNIK *zavodnik, int pocet){
    for (int i = 0; i < pocet - 1; ++i) {
        for (int j = 0; j < pocet - i - 1; ++j) {
            int cas1 = celkove_sekundy(zavodnik[j].cas);
            int cas2 = celkove_sekundy(zavodnik[j + 1].cas);
            if (cas1>cas2){
                ZAVODNIK temp = zavodnik[j];
                zavodnik[j] = zavodnik[j+1];
                zavodnik[j+1] = temp;
            }
        }
    }
}


void vypis(ZAVODNIK *zavodnik, int pocet){
    printf("STARTOVNI LISTINA - SLALOM ZENY\n");
    printf("---------------------------------------------------------------------------\n");
    printf("startovni cislo       prijmeni        jmeno      cas 1. kola   cas 2. kola \n");
    printf("---------------------------------------------------------------------------\n");
    for (int i = 0; i < pocet; ++i) {
        printf(" %14d %14s %12s          %0d:%05.2f       %0d:%05.2f \n", zavodnik[i].startovni_cislo, zavodnik[i].prijmeni,
               zavodnik[i].jmeno, zavodnik[i].cas.minuty_prvni, zavodnik[i].cas.sekundy_prvni,
               zavodnik[i].cas.minuty_druhe, zavodnik[i].cas.sekundy_druhe);
    }
}

void zapis_do_souboru(ZAVODNIK *zavodnik, int pocet){
    FILE *soubor = fopen(VYSTUPNI_SOUBOR, "w");
    if(soubor == NULL){
        printf("Soubor %s se nepodarilo otevrit.\n", VYSTUPNI_SOUBOR);
        return;
    }
    fprintf(soubor, " poradi   cislo   prijmeni    jmeno    1. kolo    2. kolo    celkem     ztrata\n");

    float cas_vyteze = celkove_sekundy(zavodnik[0].cas);

    for (int i = 0; i < pocet; ++i) {
        float celkovy_cas = celkove_sekundy(zavodnik[i].cas);
        float ztrata = celkovy_cas - cas_vyteze;
        CAS vysledny_cas = prevod_cel_sekund_na_cas(celkovy_cas);
        CAS celkova_ztrata = prevod_cel_sekund_na_cas(ztrata);

        fprintf(soubor, " %10d %14d %14s %12s            %0d:%05.2f        %0d:%05.2f          %02d:%05.2f",
                i + 1, zavodnik[i].startovni_cislo, zavodnik[i].prijmeni, zavodnik[i].jmeno,
                zavodnik[i].cas.minuty_prvni, zavodnik[i].cas.sekundy_prvni,
                zavodnik[i].cas.minuty_druhe, zavodnik[i].cas.sekundy_druhe,
                vysledny_cas.minuty_prvni, vysledny_cas.sekundy_prvni);

        if(i == 0){
            fprintf(soubor, "\n");
        }else{
            fprintf(soubor, "        +%02d:%05.2f\n", celkova_ztrata.minuty_prvni, celkova_ztrata.sekundy_prvni);
        }
    }
    if(fclose(soubor) == EOF){
        printf("Soubor %s se nepodarilo zavrit. \n", VYSTUPNI_SOUBOR);
    }
}

int main() {
    ZAVODNIK zavodnik[POCET_RADKU];
    int pocet = nacti_soubor(zavodnik);
    vypis(zavodnik, pocet);
    serad_zavodniky(zavodnik, pocet);
    zapis_do_souboru(zavodnik, pocet);

    return 0;
}
