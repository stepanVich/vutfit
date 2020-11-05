#include <iostream>
#include <string>
#include <string.h>
#include <time.h>
#include "cell.cpp"
#include <unistd.h>
using namespace std;
enum mods {SYNCHRONIZED, GREEN_WAVE, RANDOMIZED};

/**
 * @author xvichs00, Stepan Vich
 * @brief Hlavni funkce modelujici celularni automat. Reprezentuje krizovatku o 2*N silnicich.
 * @return double Prumerny tok v systemu. Pri zadanem koncovem parametru returnMeanSpeed = true vraci prumernou rychlost aut v systemu
 * @param N - pocet vozovek v jedne ose -> Celkem 2*N vozovek, ktere vytvareji N*N krizovatek	
 * @param D - pocet bunek mezi krizovatkami + 1 (Pocet bunek mezi krizovatkami je tedy D - 1). Rozmer automatu se vypocte jako N*D
 * @param rho - recke oznaceni pro hustotu (density). Na jeji velikosti zavisi pocet aut v systemu
 * @param vMax - maximalni rychlost automobilu v systemu
 * @param T - perioda prepinani svetel
 * @param R - randomization parametr - rozsah od 0 do 1 - cim vetsi, tim je vetsi pravdepodobnost, ze nastane zpomeni automobilu ve 3. kroku prechodove funkce CA.
 * @param iterationCount - pocet iteraci celularniho automatu
 * @param verboseFlag - urcuje zda se ma pri simulaci vypisovat pomocne informace - vhodne pro debugging
 * @return double Vraci hodnoty vetsi nebo rovne nule, ktere se rovnaji prumerne rychlosti automobilu v systemu. Pokud se v systemu nenachazi automobil vraci 0 (take prumerna rychlost). Pri spatnem zadani parametru, nebo chybe v simulaci vraci -1
 **/
double simulation(unsigned int N = 1, unsigned int D = 100, double rho = 0.1, unsigned int vMax = 5, unsigned int T = 30, double R = 0.1, unsigned int iterationCount = 100000, mods mod = SYNCHRONIZED, bool verboseFlag = false, bool returnMeanSpeed = false) {
	// Osetreni vstupnich parametru
	if(rho < 0 || rho > 1) return -1;
	if(R < 0 || R > 1) return -1;	
	// Parametry simulace
	// Velikost rozmeru automatu
	unsigned int L = N*D;
	if(L <= 0) return -1;
	if(vMax <= 0) return -1;
	// Pocet aut v systemu
	unsigned int Nv = rho*(double(N*N*(2*D-1)));
	unsigned int zbytek = Nv % 2;
	unsigned int Nx = Nv / 2;
	unsigned int Ny = Nx + zbytek;
	// Pokud v systemu neni ani jedno auto vrat automaticky 0
	if(Nv == 0) return 0;
	
	// Alokace 2d pole reprezentujici celularni automat
	Cell **automaton;
	try {
		automaton = new Cell*[L];
		for(int i = 0; i < L; ++i) {
			automaton[i] = new Cell[L];
		}
	} catch(std::bad_alloc&) {
		// Chyba s alokaci pameti 2d pole
		printf("Nepodarilo se alokovat pamet pro zadane rozmery celularniho automatu.\n");
		return(-1);
	}

	// Vypocet ostatnich hodnot
	if(verboseFlag) printf("Pocet automobilu v systemu: %i\n", Nv);
	if(verboseFlag) printf("Pocet automobilu v ose x:   %i\n", Nx);
	if(verboseFlag) printf("Pocet automobilu v ose y:   %i\n", Ny);
	// Pocet automobilu v jedne ulici 
	// Rozdelit rovnomerne i se zbytkem	
	// Kazdy pruh ma delku L 
	// Pocet bunek ktere se mohou naplnit auty = L - N
	double stepX = (N*(L-N)) / double(Nx);
	double stepY = (N*(L-N)) / double(Ny);
	unsigned int lastIndexX = 0;
	unsigned int lastIndexY = 0;
	unsigned int roadIndexY = 0;
	unsigned int roadIndexX = 0;
	bool firstArray = true; 

	// Naplneni automatu daty	
	// Nastaveni seminka nahodnosti pro celou simulaci
	srand(time(NULL));
	for(int x = 0; x < N; x++) {
		unsigned int xindex = D*x;
		for(int y = 0; y < L; y++) {
			// Silnice Y
			automaton[xindex][y].setYCell();
			if(y % D == 0) {
				// Krizovatka
				int offset = 0;
				if(T == 0) {
					offset = 0;	
				} else {
					if(mod == SYNCHRONIZED) {
						offset = 0;
					}
					if(mod == GREEN_WAVE) {
						offset = ((x + int(y/D))*(D/(vMax-1)))%(2*T);
					}
					if(mod == RANDOMIZED) {
						offset = rand() % (2*T);
					}
				}
				automaton[xindex][y].setIntersection(offset);
				if(offset > T) {
					automaton[xindex][y].xFree = false;
				}
			} else {
				// Cesta pro auto
				if(int(lastIndexY * stepY) == roadIndexY) {
					automaton[xindex][y].setCar(rand() % vMax, false);
					lastIndexY ++;	
				}		
				roadIndexY ++;
			}
		}
	}	

	// Pruchod po ose X
	for(int y = 0; y < N; y++) {
		unsigned int yindex = D*y;
		for(int x = 0; x < L; x++) {
			// Silnice X
			automaton[x][yindex].setXCell();
			if(x % D == 0) {
				// Krizovatka
				// Jiz nastaveno v predeslem pruchodu
			} else {
				// Cesta pro auto
				if(int(lastIndexX * stepX) == roadIndexX) {
					int randSpeed = rand()%vMax;
					automaton[x][yindex].setCar(randSpeed, true);
					lastIndexX ++;	
				}		
				roadIndexX ++;
			}
		}
	}

	// Prumerna rychlost napric iteracemi
	double meanSpeedIterationSum = 0;

	// Zacatek simulace
	if(verboseFlag) printf("Spusteni simulace...\n");
	for(int i = 0; i < iterationCount; i++) {
		// Prumerna rychlost v systemu
		double meanSpeed = 0;	
		double meanSpeedSum = 0;

		// Prechodova funkce
		// I. Acceleration
		for(int m = 0; m < L; m++) {
			for(int n = 0; n < L; n++) {
				// I. Acceleration
				if( automaton[m][n].isCar && automaton[m][n].speed < vMax) {
					automaton[m][n].speed += 1;
				}
				// Oprava chybnych cteni
				if(automaton[m][n].isCar && automaton[m][n].xDirection == false && automaton[m][n].yDirection == false) {
					double r = rand() / (float)RAND_MAX;
					if(r <= 0.5) {
						automaton[m][n].xDirection = true;	
					} else {
						automaton[m][n].yDirection = true;
					} 
				}
				if(automaton[m][n].isCar && automaton[m][n].xDirection && automaton[m][n].yDirection) {
					double r = rand() / (float)RAND_MAX;
					if(r <= 0.5) {
						automaton[m][n].xDirection = false;	
					} else {
						automaton[m][n].yDirection = false;
					} 
				}
				// II. Slowing down
				for(int offset = 1; offset <= automaton[m][n].speed; offset++) {
					unsigned int xPos = m;
					unsigned int yPos = n;
					unsigned int xPosGridlock = m;
					unsigned int yPosGridlock = n;
					if(automaton[m][n].xDirection) {
						xPos = (xPos + offset) % L;
						xPosGridlock = (xPos + offset + 1) % L;
					}
					if(automaton[m][n].yDirection) {
						yPos = (yPos + offset) % L;
						yPosGridlock = (yPos + offset + 1) % L;
					}
					// Kontrola pred nasledujicim autem
					if(automaton[xPos][yPos].isCar) {
						// Trim speed
						automaton[m][n].speed = offset - 1;
						continue;
					}

					// Kontrola pred semaforem 
					if(automaton[xPos][yPos].isIntersection) {
						if((automaton[m][n].yDirection && automaton[xPos][yPos].xFree) || (automaton[m][n].xDirection && ! automaton[xPos][yPos].xFree)) {
							// cervena barva
							// Trim speed
							automaton[m][n].speed = offset - 1;
							continue;
						} else {
							// zelena barva
							// Kontrola proti gridlocku
							if(automaton[xPosGridlock][yPosGridlock].isCar) {
								// Trim speed
								automaton[m][n].speed = offset - 1;
							}
						}
					}
				}
				

				// III. Randomization
				if(automaton[m][n].isCar) {
					if(automaton[m][n].speed > 0) {
						// random between 0..1
						double r = rand() / (float)RAND_MAX;
						if(r <= R) {
							automaton[m][n].speed -= 1;
						} 			
						// Vypocet prumerne rychlosti v systemu
						meanSpeedSum += automaton[m][n].speed;
					}
					// IV. Movement
					unsigned int speedOffset = automaton[m][n].speed;
					if(automaton[m][n].xDirection) {
						automaton[(m+speedOffset)%L][n].speedRewrite = speedOffset;
						automaton[(m+speedOffset)%L][n].xDirectionRewrite = true;
						automaton[(m+speedOffset)%L][n].yDirectionRewrite = false;
						automaton[(m+speedOffset)%L][n].rewrited = true;
					} else if(automaton[m][n].yDirection) {
						automaton[m][(n+speedOffset)%L].speedRewrite = speedOffset;
						automaton[m][(n+speedOffset)%L].yDirectionRewrite = true;
						automaton[(m+speedOffset)%L][n].xDirectionRewrite = false;
						automaton[m][(n+speedOffset)%L].rewrited = true;
					}
				}
			}
		}		
		if(verboseFlag && firstArray) {
			for(int n = 0; n < L; n++) {
				for(int m = 0; m < L; m++) {
					if(automaton[m][n].isCar) {
						if(automaton[m][n].xDirection) {
							printf(" %i ", automaton[m][n].speed);
						} else if(automaton[m][n].yDirection) {
							printf(" %i ", automaton[m][n].speed);
						} else {
							printf(" ? ");
						}
					} else if(automaton[m][n].isIntersection) {
						if(automaton[m][n].xFree) {
							printf(" + ");
						} else {
							printf(" @ ");
						}
					} else if(automaton[m][n].rewrited){
						printf(" r ");
					} else if(automaton[m][n].isXCell){
						printf(" - ");
					} else if(automaton[m][n].isYCell) {
						printf(" | ");
					} else {
						printf(" . ");
					}
					
				}
				printf("\n");
			}
			firstArray = false;
		}

		meanSpeed = meanSpeedSum / Nv; 
		meanSpeedIterationSum += meanSpeed;
		// Prepsani automobilu
		for(int m = 0; m < L; m++) {
			for(int n = 0; n < L; n++) {
				// Vymazani puvodniho auta
				if(automaton[m][n].isCar) {
					automaton[m][n].isCar      = false;
					automaton[m][n].xDirection = false;
					automaton[m][n].yDirection = false;
				}
				// Prevod prepsaneho auta na normalni
				if(automaton[m][n].rewrited) {
					automaton[m][n].isCar      = true;
					automaton[m][n].xDirection = automaton[m][n].xDirectionRewrite;
					automaton[m][n].yDirection = automaton[m][n].yDirectionRewrite;
					automaton[m][n].speed      = automaton[m][n].speedRewrite;
					automaton[m][n].xDirectionRewrite = false;
					automaton[m][n].yDirectionRewrite = false;
					automaton[m][n].rewrited = false;
				}
				// V. Prepnuti semaforu
				if(automaton[m][n].isIntersection) {
					automaton[m][n].cycle ++;
					if(automaton[m][n].cycle == T) {
						// Toggle light
						automaton[m][n].xFree = !automaton[m][n].xFree;
					}
					if(automaton[m][n].cycle >= 2*T) {
						automaton[m][n].xFree = !automaton[m][n].xFree;
						automaton[m][n].cycle = 0;
					}
				}
			}
		}
		if(verboseFlag) printf("Iterace %i / %i\r", iterationCount, i+1);
	}
	if(verboseFlag) printf("\n");
	int count = 0;
	for(int m = 0; m < L; m++) {
		for(int n = 0; n < L; n++) {
			if(automaton[m][n].isCar) {
				count++;
			}
		}
	}
	if(verboseFlag) printf("Pocet automobilu v systemu pred simulaci:%i\n", Nv);
	if(verboseFlag) printf("Pocet automobilu v systemu po simulaci:  %i\n", count);
	double avgFlow;
	if(iterationCount == 0) {
		avgFlow = 0;
	} else {
		avgFlow = meanSpeedIterationSum / iterationCount;
	}
	if(verboseFlag) printf("Prumerna rychlost v systemu (meanSpeed): %f\n", avgFlow);
	if(verboseFlag) printf("Prumerny tok v systemu (meanFlow = meanSpeed * rho): %f\n", avgFlow * rho);
	if(returnMeanSpeed) return avgFlow;
	// return meanFlow
	return avgFlow * rho;	
}

/*
 * @author Stepan Vich
 * @brief hlavni funkce ktera spousti jednotlive simulace
 * @param [None] - funkce spusti prednastavene simulace a vysledky ulozi do datovych souboru
 * @param [parametry simulace] - pri zadani vlastnich parametru se spusti standardni jedna simulace a na stdout se vytiskne prumerna rychlost v systemu
 * @return int 0 - vse probehlo v poradku; 1 - spatne zadane parametry, chyba behem simulace nebo chyba pri alokaci pameti
 */
int main(int argc, char**argv) {
	// Parsovani argumentu	
	// Predpripravene simulace
	// Synchronizovane semafory
	// Vysledky se standartne zapisuji na stdout 
	// Pro pozdejsi vizualizaci v programu gnuplot pouzijte presmerovani do externiho souboru a vypis pomoci makefile
	//
	// Experimenty  Simulace pro overeni validity modelu a pro potvrzeni spravnosti clanku
	if(argc == 2 && (strcmp(argv[1], "1a") == 0)) {
		for(int T = 0; T < 150; T++) {
			double meanFlow = simulation(1, 100, 0.05, 5, T, 0.1, 3000, SYNCHRONIZED, false);
			printf("%i %f\n", T, meanFlow);
		} 

	} else if(argc == 2 && (strcmp(argv[1], "1b") == 0)){
		for(int T = 0; T < 150; T++) {
			double meanFlow = simulation(1, 100, 0.20, 5, T, 0.1, 3000, SYNCHRONIZED, false);
			printf("%i %f\n", T, meanFlow);
		} 

	} else if(argc == 2 && (strcmp(argv[1], "1c") == 0)){
		for(int T = 0; T < 150; T++) {
			double meanFlow = simulation(1, 100, 0.50, 5, T, 0.1, 3000, SYNCHRONIZED, false);
			printf("%i %f\n", T, meanFlow);
		} 

	} else if(argc == 2 && (strcmp(argv[1], "1d") == 0)){
		for(int T = 0; T < 150; T++) {
			double meanFlow = simulation(1, 100, 0.70, 5, T, 0.1, 3000, SYNCHRONIZED, false);
			printf("%i %f\n", T, meanFlow);
		} 

	} else if(argc == 2 && (strcmp(argv[1], "2a") == 0)){
		for(int T = 0; T < 150; T++) {
			double meanFlow = simulation(1, 100, 0.05, 5, T, 0.5, 3000, SYNCHRONIZED, false);
			printf("%i %f\n", T, meanFlow);
		} 

	} else if(argc == 2 && (strcmp(argv[1], "2b") == 0)){
		for(int T = 0; T < 150; T++) {
			double meanFlow = simulation(1, 100, 0.20, 5, T, 0.5, 3000, SYNCHRONIZED, false);
			printf("%i %f\n", T, meanFlow);
		} 

	} else if(argc == 2 && (strcmp(argv[1], "2c") == 0)){
		for(int T = 0; T < 150; T++) {
			double meanFlow = simulation(1, 100, 0.50, 5, T, 0.5, 3000, SYNCHRONIZED, false);
			printf("%i %f\n", T, meanFlow);
		} 

	} else if(argc == 2 && (strcmp(argv[1], "2d") == 0)){

		for(int T = 0; T < 150; T++) {
			double meanFlow = simulation(1, 100, 0.70, 5, T, 0.5, 3000, SYNCHRONIZED, false);
			printf("%i %f\n", T, meanFlow);
		} 
	} else if(argc == 2 && (strcmp(argv[1], "2c") == 0)){
		for(int T = 0; T < 150; T++) {
			double meanFlow = simulation(1, 100, 0.50, 5, T, 0.5, 3000, SYNCHRONIZED, false);
			printf("%i %f\n", T, meanFlow);
		} 
	} else if(argc == 2 && (strcmp(argv[1], "2d") == 0)){

		for(int T = 0; T < 150; T++) {
			double meanFlow = simulation(1, 100, 0.70, 5, T, 0.5, 3000, SYNCHRONIZED, false);
			printf("%i %f\n", T, meanFlow);
		} 


	// Experimenty na overeni ze prumerny tok nezavisi na poctu krizovatek v systemu
	} else if(argc == 2 && (strcmp(argv[1], "3a") == 0)){

		for(int T = 0; T < 150; T += 5) {
			double meanFlow = simulation(1, 100, 0.05, 5, T, 0.1, 3000, SYNCHRONIZED, false);
			printf("%i %f\n", T, meanFlow);
		} 
	} else if(argc == 2 && (strcmp(argv[1], "3b") == 0)){

		for(int T = 0; T < 150; T += 5) {
			double meanFlow = simulation(5, 100, 0.05, 5, T, 0.1, 3000, SYNCHRONIZED, false);
			printf("%i %f\n", T, meanFlow);
		} 

	// Experimenty pro vypocet prumerne rychlosti v systemu (ne prumerneho toku)
	} else if(argc == 2 && (strcmp(argv[1], "4") == 0)){

		for(int T = 0; T < 150; T++) {
			double meanFlow = simulation(1, 100, 0.05, 5, T, 0.1, 10000, SYNCHRONIZED, false, true);
			printf("%i %f\n", T, meanFlow);
		} 

	// Green wave strategy experimenty 
	} else if(argc == 2 && (strcmp(argv[1], "5a") == 0)){

		for(int T = 0; T < 150; T++) {
			double meanFlow = simulation(4, 50, 0.05, 5, T, 0.1, 5000, SYNCHRONIZED, false);
			printf("%i %f\n", T, meanFlow);
		} 
	} else if(argc == 2 && (strcmp(argv[1], "5b") == 0)){

		for(int T = 0; T < 150; T++) {
			double meanFlow = simulation(4, 50, 0.05, 5, T, 0.1, 5000, GREEN_WAVE, false);
			printf("%i %f\n", T, meanFlow);
		} 

	// Randomized experimenty 1
	} else if(argc == 2 && (strcmp(argv[1], "6a") == 0)){

		for(int T = 0; T < 150; T++) {
			double meanFlow = simulation(10, 100, 0.05, 5, T, 0.1, 3000, SYNCHRONIZED, false);
			printf("%i %f\n", T, meanFlow);
		} 
	} else if(argc == 2 && (strcmp(argv[1], "6b") == 0)){

		for(int T = 0; T < 150; T++) {
			double meanFlow = simulation(10, 100, 0.05, 5, T, 0.1, 3000, RANDOMIZED, false);
			printf("%i %f\n", T, meanFlow);
		} 

	// Randomized experimenty 2
	} else if(argc == 2 && (strcmp(argv[1], "7a") == 0)){

		for(int T = 0; T < 150; T++) {
			double meanFlow = simulation(10, 100, 0.70, 5, T, 0.1, 3000, SYNCHRONIZED, false);
			printf("%i %f\n", T, meanFlow);
		} 
	} else if(argc == 2 && (strcmp(argv[1], "7b") == 0)){

		for(int T = 0; T < 150; T++) {
			double meanFlow = simulation(10, 100, 0.70, 5, T, 0.1, 3000, RANDOMIZED, false);
			printf("%i %f\n", T, meanFlow);
		} 

	} else {
		// Vlastni simulace
		// Zpracovani argumentu
		// Vychozi hodnoty
		unsigned int N = 1;
		unsigned int D = 100;
		double rho = 0.1;
		unsigned int vMax = 5;
		unsigned int T = 30;
		double R = 0.1;
		unsigned int iterationCount = 100000;
		mods mod = SYNCHRONIZED;
		bool verboseFlag = false;
		bool returnMeanSpeed = false;
		// Parsovani argumentu pomoci getopt
	
		unsigned int c;	
		
		while((c = getopt(argc, argv, "n:d:r:v:t:R:i:m:VhS")) != -1) {
			switch(c) {
				case 'n':
					N = atoi(optarg);
					break;
				case 'd':
					D = atoi(optarg);
					break;
				case 'r':
					rho = atof(optarg);
					break;
				case 'v':
					vMax = atoi(optarg);
					break;
				case 't':
					T = atoi(optarg);
					break;
				case 'R':
					R = atoi(optarg);	
					break;
				case 'i':
					iterationCount = atoi(optarg);
					break;
				case 'm':
					if(strcmp(optarg,"GREEN_WAVE") == 0) mod = GREEN_WAVE;
					if(strcmp(optarg, "RANDOMIZED")== 0) mod = RANDOMIZED;
					break;
				case 'V':
					verboseFlag = true;
					break;
				case 'S':
					returnMeanSpeed = true;	
					break;

				case 'h':
					// vytiskni napovedu
					printf("usage:\t./sim\t[-n N] [-d D] [-r rho] [-v vMax]\n\t\t[-t T] [-R R] [-i iterationCount]\n\t\t[-m mod] [-V] [-S] [-h]\n");
					printf("\t-h\tvypise tuto napovedu\n");
					printf("\t-n N\tpocet krizovatek v modelu\n");
					printf("\t-d D\tpocet ulic mezi krizovatkami + 1\n");
					printf("\t-r rho\thustota rho - urcuje pocet automobilu v systemu\n");
					printf("\t-v vMax\tmaximalni rychlost vozidel\n");
					printf("\t-t T\tperioda prepinani svetel na krizovatce\n");
					printf("\t-R R\tRandomization parametr\n");
					printf("\t-i arg\tpocet iterace pro danou simulaci\n");
					printf("\t-m mod\tmod prepinani svetel: SYNCHRONIZED, GREEN_WAVE, RANDOMIZED\n");
					printf("\t-V\tverbose flag - behem simulace se vypisuji dodatecne informace\n");
					printf("\t-S\tsimulace vraci prumernou rychlost v systemu vsech automobilu\n");
					return 0;
					break;
			}	
		}
		// Vytisknuti parametru simulace
		const char * modString = (mod == SYNCHRONIZED ? "SYNCHRONIZED" : (mod == GREEN_WAVE ? "GREEN_WAVE" : "RANDOMIZED"));
		printf("Spusteni vlastni simulace: ");
		printf("N: %i, D: %i, rho: %f, vMax: %i, T: %i, R: %f, iterationCount: %i, mod: %s, verbose: %i, meanSpeed: %i\n", N, D, rho, vMax, T, R, iterationCount, modString, verboseFlag, returnMeanSpeed);
		double mean = simulation(N, D, rho, vMax, T, R, iterationCount, mod, verboseFlag, returnMeanSpeed);
		printf("%f\n", mean);
		if(mean < 0) return 1;
	}
	return 0;
}

