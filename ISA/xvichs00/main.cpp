// Include vsech potrebnych hlavickovych funkci
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <thread>
#include <math.h>
#include <errno.h>

// Pomocne makra

// Maximalni velikost pro prijmana data z jednoho paketu
#define BUFFER 2048
// Pocet prvnich odesilanych znaku zpravy u UDP paketu, ktere si pamatujeme
#define UDP_ID_LENGTH 6

// Pomocne struktury

// Struktura reprezentujici jeden odeslany paket 
// Jako id paketu se odesle id aktualniho podprocesu
struct node {
	timeval t_send; // Cas kdy byl paket odeslan
	long long rtt;  // Round trip time - spocteno jakmile paket dorazi zpet
	int seq;        // Poradove cislo 
	int isReplied;  // Boolean hodnota - Prisla/Zatim neprisla odpoved
	int isLost;     // Boolean hodnota - Zatim neprisla odpoved/na odpoved uz ani necekat 
	int threshold;  // Boolean hodnota - Prekrocil/neprekrocil threshold
	char* msg;      // Alokuje se pouze u udp paketu - ukladame zde [UDP_ID_LENGTH] pocet znaku zpravy
	node* next;     // Ukazatel na dalsi odeslany paket (zretezeny seznam)
}; 

// Struktura reprezentujici icmpv6 hlavicku
struct icmpv6_hdr {
	uint8_t		icmpv6_type; // Type icmp zpravy - v nasem pripade 128	
	uint8_t		icmpv6_code; // Kod icmp zpravy  - v nasem pripade 0
	uint16_t	icmpv6_cksum;// Checksum
	uint16_t        id;          // ID odesilaneho paketu - zde ID podprocesu
	uint16_t        seq;	     // Sekvencni cislo odesilaneho paketu
};

// Ukazatel na prvni prvek zretezeneho seznamu u odeslanych paketu
struct node* first_node;
// Ukazatel na posledni prvek ve zretezenem seznamu
struct node* node_p;

// Pamet pro statistiky, kdyz dealokujeme pamet pomocnych struktur pomoci free
struct stats_mem {
	int sendedPackets = 0;
	int repliedPackets = 0;
	int lostPackets = 0;
	int thresholdPackets = 0;
	double rttSum = 0;
	double rttSumSquare = 0;
	double rttMin = 99999;
	double rttMax = 0;
	double distanceSum = 0;
};
struct stats_mem stats;

// Metoda pro uvolneni pameti zretezeneho seznamu
void free_nodes() {
	struct node * help_p = first_node;
	struct node * ptr;
	node_p = NULL;
	first_node = NULL;
	while(help_p != NULL) {
		ptr = help_p;		
		help_p = help_p->next;
		free(ptr);
	}
	return;
};


// Vychozi hodnoty pro jednotlive hodnoty
int dataSended = 56;
int udpFlag  = 0;
int verboseFlag = 0;
unsigned int timeoutIntervalS     = 300;
unsigned int messageIntervalMs    = 100;
unsigned int recieveTimeoutS      = 2;
unsigned int rttThresholdMs;
unsigned int port = 0;
int udpServer = 0;
double calculatedRTT;
int err = 0;
char*address;
char*ip_address;
char ip_address_buff[INET6_ADDRSTRLEN];

unsigned int isipv6 = 0;


// Vytiskne aktualni datum a cas
// Pomocna funkce
// Splnuje format ze zadani
void printTime() {
	// Pro aktualni milisekundy je nutne vyuzit funkci gettimeofday
	struct timeval tv;
	time_t nowtime;
	struct tm *nowtm;
	char tmbuf[64], buf[64];
	// funkce pro zjisteni aktualnich ms
	gettimeofday(&tv, NULL);
	nowtime = tv.tv_sec;
	nowtm = localtime(&nowtime);
	tv.tv_usec = tv.tv_usec * 10e-5;
	// spravne naformatovani zpravy
	strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
	snprintf(buf, sizeof buf, "%s.%02ld", tmbuf, tv.tv_usec);	
	fprintf(stdout, "%s ", buf);
}

// Pomocna funkce
// Tiskne napovedu help
void printHelp() {
	printf("Pouziti:\n");
	printf("sudo ./testovac -h\n");
        printf("sudo ./testovac -l port\n");
        printf("sudo ./testovac\t[-s velikost_dat] [-t interval_ztratovost]\n");
	printf("\t\t[-i interval_test] [-w timeout] [-p port]\n");
	printf("\t\t[-r rtt_threshold] [-u] [-v]\n");
	printf("\t\thostname1 hostname2 ...\n");
}


// Funkce pro vypocet checksumu ICMP paketu
// Prevzata z RFC 
unsigned short in_cksum(unsigned short *ptr, int nbytes)
{
	register long sum;
	u_short oddbyte;
	register u_short answer;
	sum = 0;
	while (nbytes > 1) {
		sum += *ptr++;
		nbytes -= 2;
	}
	if (nbytes == 1) {
		oddbyte = 0;
		*((u_char *) & oddbyte) = *(u_char *) ptr;
		sum += oddbyte;
	}
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;
	return (answer);
};


// Metoda ktera bezi ve smycce pokud je zvoleno naslouchani na portu
// Sposti se volbou -l [port]
void udpServerStart() {
	// Tvorba soketu
	int sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sd < 0) {
		exit(3);
	}
	// Definice promennych
	socklen_t clen; 
	struct sockaddr_in saddr; 
	struct sockaddr_in caddr;
	struct hostent *hostp; 
	char buf[BUFFER]; 
	char *hostaddrp; 

	int optval = 1;
	int n; 
	// Nastaveni soketu
	if(setsockopt(sd,SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int))) {
		exit(3);
	}
	// Resoluce aktualni adresy
	bzero((char *) &saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	// Prirazeni portu zvoleneho ve volbe -p
	// Vyhozi hodnota je 0
	
	saddr.sin_port = htons((unsigned short)port);
	// Pripojeni na port pomoci funkce bind
	if (bind(sd,(struct sockaddr *) &saddr,sizeof(saddr)) < 0) {
		exit(3);
	}
	clen = sizeof(caddr);
	// Cekani v nekonecne smycce na prichozi pakety
	while(1) {
		// Vynulovani bufferu obsahujiciho zpravy
		bzero(buf, BUFFER);
		// Prijem zprav pomoci recvfrom
		n = recvfrom(sd, buf, BUFFER, 0, (struct sockaddr *) &caddr, &clen);
		if (n < 0) {
			exit(3);
		}		

		hostp = gethostbyaddr(( const char *)&caddr.sin_addr.s_addr, sizeof(caddr.sin_addr.s_addr), AF_INET);
		if (hostp == NULL) exit(3);
		hostaddrp = inet_ntoa(caddr.sin_addr);
		if (hostaddrp == NULL)
			exit(3); 
		// Prijem a odeslani zpravy, kterou jsme prijali zpet odesilateli
		// Jedine co se zmeni je prehozeni adresy odesilate a prijemce (programator neovlivni)
		n = sendto(sd, buf, strlen(buf), 0, (struct sockaddr *) &caddr, clen);
		if (n < 0) 
			exit(3); 
	}
	return;
}

// Funkce slouzi pro odesilani ICMP paketu (verze 4/6) na zadany soket
// V programu se vola z hlavni funkce z podprocesu a v samostatnem vlaknu
// V nekoncene smycce odesila ICMP pakety
void ping(int sd) {
	// Definice promennych
        struct sockaddr_in r_addr;
	unsigned int seq = 0;
        socklen_t len=sizeof(r_addr);
	// Struktura pro icmp hlavicku
	// Prevzata z vestavene knihovny netinet/ip_icmp.h
	struct icmp *icmp_recv;

	// Velikost struktury, kterou odesleme funkci send
	int packet_size = sizeof (struct icmphdr) + dataSended;
	// Vysledna struktura, kterou odesleme funkci send
    	char *packet = (char *) malloc (packet_size);
	if(packet == NULL) {
		close(sd);
		exit(2);
	}

	// Nekonecna smycka, odesilajici pakety v intervalu MessageIntervalMs
	// Pro casovani intervalu se pouziva funkce usleep
	while(1) {
		struct icmphdr *icmp = (struct icmphdr *) packet;
		// Vynulovani dat paketu
		memset(packet, 0, packet_size);
		// Reinicializace dat
		icmp->type = (isipv6 ? 128 : ICMP_ECHO);
		icmp->code = 0;
		icmp->un.echo.sequence = seq;
		// Jako id zpravy je pouzito process id
		// Zajisti jedinecnost id
		icmp->un.echo.id = getpid();
		icmp->checksum = 0;  
		struct timeval t;
		gettimeofday(&t, NULL);
		// Seed nahodne funkce, ktery bereme z aktualnich milisekund
		srand((unsigned) t.tv_usec);
		// Prida nahodne znaky na konec zpravy 
		for(int n = 0; n < dataSended; n++) {
			memset(packet + sizeof(struct icmphdr) + n, rand()%40 + '0', 1);		
		};
		 
		icmp->checksum = 0;
		// Prepocitani checksumu
		icmp->checksum = in_cksum((unsigned short *)icmp, sizeof(struct icmphdr) + dataSended);	
		// Pocet prijatych dat
		int result;
		if ( (result=send(sd, packet, packet_size, 0)) <= 0 ) {
			exit(3);
		} else {
			// Prida uzel do zretezeneho seznamu
			// Kontrola aby nevznikl segmentation fault
			if(first_node == NULL) {
				node_p = (node*)malloc(sizeof(struct node));
				if(node_p == NULL) {
					exit(2);
				}
			} else {
				node_p->next = (node*)malloc(sizeof(struct node));
				if(node_p->next == NULL) {
					exit(2);
				}
				node_p = node_p->next;
				node_p->next = NULL;
			}
			node_p->next = NULL;
			node_p->seq = seq;	
			gettimeofday(&node_p->t_send, NULL);
			// Zprava je odelana -> prozatim neni ani ztracena ani na ni neprisla odpoved
			node_p->isLost    = 0;
			node_p->isReplied = 0;
			// Inicializace prvniho prvku v seznamu
			if(first_node == NULL)
				first_node = node_p;
		}
		// Cekani pred odeslanim dalsi zpravy 
		usleep(messageIntervalMs * 1000);
		// Inkrementace sekvencniho cisla zpravy
		seq++;
	}
	return;
}

// Bezi paralelne vuci dalsim funkcim v jednom vlaknu
// Je volana z funkce main, v podprocesu
// Posloucha na ICMP socketu a prijma ICMPv4 a ICMPv6 pakety
void listening(int sd) {
	// Definice struktury pro prijmani dat
	struct icmp *icmp_recv;
	// Zacatek nekonecne smycky
	// Ceka v nekonecne smycce na prijem zpravy
	while(1) {
		// Buffer do ktereho nactemu prijmanou zpravu
		char buffer[BUFFER];
		struct ip *ip_recv;
		struct ip6 *ip6_recv;
		// Velikost prijatych dat
		int bytes_recieved = recv(sd, buffer, BUFFER, 0);
		if(bytes_recieved > 0) {
			struct timeval c_time;
			// Vezmeme timestamp pro vypocet 
			gettimeofday(&c_time, NULL);
			ip_recv = (struct ip *) buffer;
			// Prijem ICMPv4 zpravy
			if(ip_recv->ip_p == IPPROTO_ICMP) {
				icmp_recv = (struct icmp*)(buffer + ip_recv->ip_hl * 4);
				if( icmp_recv->icmp_id == getpid() && icmp_recv->icmp_type == ICMP_ECHOREPLY) {
					// Zkontroluj zretezeny seznam 
					struct node* help_p = first_node;
					double rtt;
					while(help_p != NULL) {
						if(icmp_recv->icmp_seq == help_p->seq && help_p->isLost != 1 && help_p->isReplied != 1) {
							// Nasel shodu s jiz odeslanym paketem
							help_p->isReplied = 1;
							// Vypocti rtt
							help_p->rtt = ((long long)(c_time.tv_sec - help_p->t_send.tv_sec))*1000000 + (long long)(c_time.tv_usec - help_p->t_send.tv_usec);
							// Preved rtt do ms a prenastav limit pro zatraceni zprav
							rtt = ((double)help_p->rtt)/1000;
							break;
						}
						help_p = help_p->next;
					}
	
					// Vytiskni informace o prijeti
					if(verboseFlag) {
						printTime();
						fprintf(stdout, "%i bytes from %s (%s) time=%.3f ms\n", bytes_recieved,address, ip_address_buff, rtt);
					}
					
				}
			} else {
				// Prijem ICMPv6 zpravy
				struct icmpv6_hdr * hdr = (struct icmpv6_hdr*) buffer;			
				if(hdr->icmpv6_type = 129 && hdr->icmpv6_code == 0 && hdr->id == getpid()) {
					struct node* help_p = first_node;
					double rtt;
					while(help_p != NULL) {
						// Najdi nezodpovezenou zpravu
						if(hdr->seq == help_p->seq && help_p->isLost != 1 && help_p->isReplied != 1) {
							help_p->isReplied = 1;
							// Vypocitej rtt
							help_p->rtt = ((long long)(c_time.tv_sec - help_p->t_send.tv_sec))*1000000 + (long long)(c_time.tv_usec - help_p->t_send.tv_usec);
							// Preved rtt do ms a prenastav limit pro zatraceni zprav
							rtt = ((double)help_p->rtt)/1000;
							break;
						}
						help_p = help_p->next;
					}

					// Vytiskni informace o prijeti
					if(verboseFlag) {
						printTime();
						fprintf(stdout, "%i bytes from %s (%s) time=%.3f ms\n", bytes_recieved,address, ip_address_buff, rtt);
					}

				}
			}
		}

	}
	return;
}


// Funkce posila udp pakety na zadeny soket
// Funguje v nekonecne smysce a pakety zasila v intervalu messageIntervalMs
void udpPing(int sock_udp) {
	// Nekonecna smycka
	while(1) {
		// Alokace odesilaneho obsahu
		char*content = (char*)malloc(dataSended);
		if(content == NULL) {
			exit(2);
		}
		memset (content, 0, dataSended);
		// Vytisknuti nahodnych hodnot do pameti
		struct timeval t;
		gettimeofday(&t, NULL);
		srand((unsigned) t.tv_usec);
		for(int n = 0; n < dataSended; n++) {
			memset(content + n, rand() % 40 + '0', 1);		
		};
		// Odeslani dat
		if(send(sock_udp, (dataSended > 0 ? content : NULL), dataSended, 0) > 0) {
			// Pridani informace o odeslanem paketu do zretezeneho seznamu
			if(first_node == NULL) {
				node_p = (node*)malloc(sizeof(struct node));
				if(node_p == NULL) {
					exit(2);
				}
			} else {
				node_p->next = (node*)malloc(sizeof(struct node));
				if(node_p->next == NULL) {
					exit(2);
				}
				node_p = node_p->next;
			}
			node_p->next = NULL;
			gettimeofday(&node_p->t_send, NULL);
			node_p->isLost    = 0;
			node_p->isReplied = 0;
			// Ulozeni prvnich [UDP_ID_LENGTH] znaku do struktury
			node_p->msg = (char*)malloc(UDP_ID_LENGTH);
			if(node_p->msg == NULL) {
				exit(2);
			}
			memcpy(node_p->msg, content, UDP_ID_LENGTH);
			if(first_node == NULL)
				first_node = node_p;
		}
		// Interval smycky
		usleep(messageIntervalMs * 1000);
	} // </while>
	return;
}

// Funkce pro prijmani udpPaketu
// Prijma udp pakety na danem soketu
// Pracuje v nekonecne smycce bez intervalu
void udpListening(int sock_udp) {
	// Nekonecna smycka
	while(1) {
		// buffer pro prijem udp paketu
		char buff[BUFFER];
		// velikost prijatych dat
		int bytes_recieved = 0;
		bytes_recieved = recv(sock_udp,buff, BUFFER, 0);	
		if(bytes_recieved > 0)  {
			// Prijeti zpravy
			struct timeval c_time;
			// Ulozeni timestampu pri prijeti
			gettimeofday(&c_time, NULL);
			struct node* help_p = first_node;
			double rtt;
			// Projizdej zretezeny seznam dokud nenajdes odeslanou zpravu
			while(help_p != NULL) {
				if(help_p->isLost != 1 && help_p->isReplied != 1) {
					// Ulozeni prvnich [UDP_ID_LENGTH] znaku do bufferu a jejich porovnani
					char buff1[UDP_ID_LENGTH + 1];
					char buff2[UDP_ID_LENGTH + 1];
					strncpy(buff1, buff, UDP_ID_LENGTH + 1);
					strncpy(buff2, buff, UDP_ID_LENGTH + 1);
					if(strcmp(buff1, buff2) == 0) {
						help_p->isReplied = 1;
						// Vypocet rtt
						help_p->rtt = ((long long)(c_time.tv_sec - help_p->t_send.tv_sec))*1000000 + (long long)(c_time.tv_usec - help_p->t_send.tv_usec);
						rtt = ((double)help_p->rtt)/1000;
						break;
					}
				}
				help_p = help_p->next;
			}

			// Vypis o prijeti zpravy
			if(verboseFlag) {
				printTime();
				fprintf(stdout, "%i bytes from %s (%s) time=%.3f ms\n", bytes_recieved,address, ip_address_buff, rtt);
			}
		}
	} 
	return;
}

// Funkce pro kontrolu vyprseneho timeoutu
// Kontroluje zda nejaky odeslany paket neprekrocil limit timeout
// timeout limit je roven recieveTimeoutS nebo 2*rtt jakmile vypocteme statistiky
void requestTimeout() {
	while(1) {
		struct node* help_p = first_node;		
		struct timeval c_time;
		gettimeofday(&c_time, NULL);
		// Projizdej szretezeny seznam a kontroluj casy odeslanych paketu
		while(help_p != NULL) {
			if(help_p->isLost == 0 && help_p->isReplied == 0) {
				// Kontrola timeoutu
				long long ellapsed =  ((long long)(c_time.tv_sec - help_p->t_send.tv_sec))*1000000 + (long long)(c_time.tv_usec - help_p->t_send.tv_usec);		
				long long timeout;	
				if(calculatedRTT) {
					timeout = 2*calculatedRTT*1000;
				} else {
					timeout = recieveTimeoutS*1000000;
				}	
				if(ellapsed > timeout) {
					help_p->isLost = 1;
				}	
				// Kontrola rtt thresholdu
				if(rttThresholdMs) {
					if(ellapsed > rttThresholdMs*1000 && help_p->threshold != 1) {
						help_p->threshold = 1;
					}
				}
			}
			help_p = help_p->next;
		}
	}
	return;
}


// Vypocitava ztratovost jednotlivych uzlu
// Tiskne ztratovost na stdout
void lossability() {
	while(1) {
		sleep(timeoutIntervalS);
		struct node* help_p = first_node;
		int repliedPackets = stats.repliedPackets;
		int lostPackets = stats.lostPackets;
		int sendedPackets = stats.sendedPackets;
		int thresholdPackets = stats.thresholdPackets;
		while(help_p != NULL) {
			if(help_p->isReplied == 1 || help_p->isLost == 1) {
				sendedPackets++;
			}
			if(help_p->isReplied == 1) {
				repliedPackets++;
			}
			if(help_p->isLost == 1) {
				lostPackets++;
			} 
			if(help_p->isLost != 1 && help_p->threshold == 1) {
				thresholdPackets++;
			}
			help_p = help_p->next;
		}					
		double packetLoss;
		if(sendedPackets == 0) {
			packetLoss = 0;	
		} else {
			packetLoss = ((double)lostPackets/ (double)sendedPackets) * 100;
		}

		// Vytiskne informace o ztratovosti	
		printTime();
		if(sendedPackets == lostPackets) {
			fprintf(stdout, "%s: status down\n", address);
		} else {
			fprintf(stdout, "%s: %.*f%% packet loss, %i packet loss\n", address, (packetLoss == 0 ? 0 : 3), packetLoss, lostPackets);
		}

		// Vytiskne informace o prekroceni thresholdu
		if (rttThresholdMs && thresholdPackets > 0) {
			printTime();
			fprintf(stdout, "%s: %.*f%% (%i) packets exceeded RTT threshold  %ims\n", address, (packetLoss == 0 ? 0 : 3), packetLoss, thresholdPackets, rttThresholdMs);
		}
	}
	return;
}


// Vypocitava statistiky jednotlivych uzlu
// Tiskne statistiky na stdout
void statistics() {
	while(1) {
		// Kazdou 1 hodinu (60s * 60s = 1 hodina)
		sleep(60*60);
		// Vypocitej statistiky 
		int sendedPackets = 0;
		int repliedPackets = 0;
		int lostPackets  = 0;
		double rttSum = 0;
		double rttMin = stats.rttMin;
		double rttMax = stats.rttMax;
		double rttAvg = 0;
		double mdev = 0;
		struct node* help_p = first_node;
		rttMin = help_p->rtt;
		while(help_p != NULL) {
			// Spocitej odeslane pakety
			if(help_p->isReplied == 1 || help_p->isLost == 1) {
				sendedPackets++;
			}
			// Spocitej pakety na ktere se odpovedelo
			if(help_p->isReplied) {
				repliedPackets++;
			}
			// Spocitej ztracene pakety
			if(help_p->isLost == 1) {
				lostPackets++;
			} 
			// Zjisti maxima a minima
			if(help_p->isLost != 1 && help_p->isReplied == 1) {
				double rtt = (double)(help_p->rtt) / 1000;
				if(rtt > rttMax) {
					rttMax = rtt;
				}
				if(rtt < rttMin) {
					rttMin = rtt;
				}
				rttSum += rtt;
				stats.rttSumSquare += rtt*rtt;
			}
			help_p = help_p->next;
		}
		// Aktualizuj statistiky
		stats.sendedPackets += sendedPackets;
		stats.repliedPackets += repliedPackets;
		stats.lostPackets += lostPackets;
		stats.rttSum += rttSum;
		stats.rttMin = (stats.rttMin > rttMin ? rttMin : stats.rttMin);
		stats.rttMax = (stats.rttMax < rttMax ? rttMax : stats.rttMax);

		rttAvg = stats.rttSum / stats.repliedPackets;

		// Vypocti standardni odchylku 
		mdev = sqrt(((1/(double)stats.repliedPackets) * stats.rttSumSquare) - (rttAvg * rttAvg));

		calculatedRTT = rttAvg;

		// Vypocti ztratovost
		double packetLoss;
		packetLoss = ((double)stats.lostPackets / (double)stats.sendedPackets) * 100;

		// Vytiskni statistiky
		printTime();
		if(stats.sendedPackets == stats.lostPackets) {
			fprintf(stdout, "%s: status down\n", address);
		} else {
			fprintf(stdout, "%s: %.3f%% packet loss, %.3f/%.3f/%.3f/%.3f ms\n",address, packetLoss, stats.rttMin, rttAvg, stats.rttMax, mdev);
		}
		
		// Uvolni pamet zretezeneho seznamu
		free_nodes();
	}
	return;
};


// Hlavni funkce programu
// Pri sledovani [n] uzlu se program rozdeli na [n] podprocesu
// Kazdy proces sleduje jeden uzel
int main(int argc, char ** argv)
{
	// Parsovani argumentu
	int c;
	while((c = getopt(argc, argv, "huvt:i:w:p:l:s:r:")) != -1) {
		switch(c) {
			case 'h':
				// Tiskne napovedu
				printHelp();
				exit(0);
				break;
			case 'u':
				// pouzij UDP Protokol
				udpFlag = 1;
				break;
			case 't':
				timeoutIntervalS = atoi(optarg);
				break;
			case 'v':
				// verbose mode
				verboseFlag = 1;
				break;
			case 'i':
				// Interval v ms
				messageIntervalMs = atoi(optarg);
				if(messageIntervalMs < 0) messageIntervalMs = 0;
				break;
			case 'w':
				recieveTimeoutS = atoi(optarg);
				break;
			case 'p':
				port = atoi(optarg);
				break;
			case 'l':
				udpServer = 1;
				port = atoi(optarg);
				break;
			case 's':
				dataSended = atoi(optarg);
				break;
			case 'r':
				rttThresholdMs = atoi(optarg);
				if(rttThresholdMs <= 0) {
					fprintf(stdout, "Hodnota RTT musi byt vetsi nez 0\n");
					exit(1);
				}
				break;
			default:
				exit(1);
		}
	}

	// Kontrola hodnot parametru	
	// velikost Odesilanych dat vetsi nez nula
	if(dataSended < 0) {
		fprintf(stdout, "Zadejte validni hodnotu pro parametr -s\n");
		exit(1);
	} 
	// UDP paket musi odesilat nejaka data
	if(udpFlag && dataSended == 0) {
		fprintf(stdout, "Pri volbe UDP paketu musi byt velikost odesilanych dat vetsi nez 0.\n");
		exit(1);
	}
	// Omezeni rozsahu portu
	if(port < 0 || port > 65535) {
		fprintf(stdout, "Zadejte validni hodnotu sitoveho portu parametru -p\n");
		exit(1);
	}
	// Intervaly musi byt vetsi nez 0
	if(timeoutIntervalS == 0 || messageIntervalMs == 0 || recieveTimeoutS == 0) {
		fprintf(stdout, "Zadejte rozmezi intervalu vetsi nez 0\n");
		exit(1);
	} 

	// Index hodnot pro zparsovani
	unsigned int index1 = 0;
	unsigned int index2 = 0;

	if(udpServer) {
		// Nastartuj udp server - bezi v jednom podprocesu
		std::thread udpServer(udpServerStart);
		udpServer.join();
	} else {
		// Parsovani jednotlivych uzlu
		pid_t pid;
		if(optind >= argc) {
			// Nebyl zadan zadny uzel pro statistiky
			fprintf(stdout, "Zadejte alespon jednu adresu uzlu\n");
			exit(1);
		}
		for(index1 = optind; index1 < argc; index1++) {
			// Parallel proccesing of nodes
			pid = fork();
			if(pid < 0) {
				exit(4);
			}
			// Vytvoreni podprocesu
			if(pid == 0) {
				// Nastaveni adresy pro pomocne vytisky adresy
				address = argv[index1];
				if(udpFlag) {
					// Zprasovani adresy zadane programu 
					struct addrinfo hints;
					struct addrinfo *result, *rp;
					struct sockaddr_in *remote;
					memset(&hints, 0, sizeof(struct addrinfo));
					hints.ai_family = AF_UNSPEC;
					hints.ai_socktype = SOCK_DGRAM;
					char port_buff[20];
					sprintf(port_buff, "%i", port);
					if(getaddrinfo(argv[index1], port_buff, &hints, &result) != 0) {
						exit(3);
					}
					// Tvorba udp soketu
					// Budeme jej predat do udpPing a udpListener
					int sock_udp;
					for(rp = result; rp != NULL; rp = rp->ai_next) {
						sock_udp = socket(rp->ai_family, SOCK_DGRAM, IPPROTO_UDP);
						if(sock_udp == -1) continue;
						void *addr1;
						remote = (struct sockaddr_in *)rp->ai_addr;
						addr1 = &(remote->sin_addr);
						// Prevod IP adresy do string
						// IPv6/v4 verze kompatibilni
						inet_ntop(rp->ai_family, addr1, ip_address_buff, sizeof(ip_address_buff));
						if(connect(sock_udp, rp->ai_addr, rp->ai_addrlen) != -1) break;
						close(sock_udp);
					}

					if(rp == NULL) {
						// Nevalidni adresa
						exit(3);
					}

					if ( fcntl(sock_udp, F_SETFL, O_NONBLOCK) != 0 ) {
						exit(3);
					}

					// Vytvoreni vlaken programu 
					// Vlkno pro pingovani
					std::thread udpPinger(udpPing, sock_udp);
					// Vlakno pro sledovani prichozich paketu
					std::thread udpListener(udpListening, sock_udp);
					std::thread timeout(requestTimeout);
					std::thread stats(statistics);
					std::thread loss(lossability);
					udpPinger.join();
					udpListener.join();
					timeout.join();
					stats.join();
					loss.join();
				} else {
					// Ziskej informace o adrese zadanou programu 
					int sd;	
					const int val=255;
					int status;
					struct sockaddr_in *remote;
					struct addrinfo hints, *res, *p;
					if (getaddrinfo(argv[index1], NULL, NULL, &res) != 0) {
    						exit(1);
					}

					for(p = res; p != NULL; p = p->ai_next) {
						if(p->ai_family == AF_INET) {
						sd = socket(p->ai_family, SOCK_RAW, IPPROTO_ICMP);
						} else if(p->ai_family == AF_INET6) {
							sd = socket(p->ai_family, SOCK_RAW, IPPROTO_ICMPV6);	
						}
						void *addr1;
						remote = (struct sockaddr_in *)p->ai_addr;
						addr1 = &(remote->sin_addr);
						// Prevod IP adresy do stringu
						inet_ntop(p->ai_family, addr1, ip_address_buff, sizeof(ip_address_buff));
						if(connect(sd, p->ai_addr, p->ai_addrlen) != -1) break;
						close(sd);
					}

					// Nevalidni adresa
					if ( sd < 0 ) {
						exit(3);
					}

					if(p->ai_family == AF_INET6) {
						// Jedna se o pomocnou promennou
						isipv6 = 1;
					}

					// Nastaveni TTL
					if(p->ai_family == AF_INET) {
						setsockopt(sd, IPPROTO_IP, IP_TTL, &val, sizeof(val));
					} else {
						setsockopt(sd, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &val, sizeof(val));
					}

					// Neblokujici soket
					if ( fcntl(sd, F_SETFL, O_NONBLOCK) != 0 ) {
						exit(3);
					} 
			
					// Vytvoreni nekolika vlaken	
					std::thread pinger(ping, sd);
					std::thread listener(listening, sd);
					std::thread timeout(requestTimeout); 
					std::thread stats(statistics);
					std::thread loss(lossability);
					pinger.join();
					listener.join();
					stats.join();
					timeout.join();
					loss.join();
				}
				break;
			}
		}
		 
		if(pid > 0) {
			// Cekej na ukonceni jednotlivych podprocesu
			for(index2 = optind; index2 < argc; index2++) {
				wait(&err);
				// Pokud nejaky podproces skonci s chybou, ukonci hlavni program a vrat tuto chybu
				if(WIFEXITED(err)) {
					return WEXITSTATUS(err);
				}
			}
		}
	}

	// Vse probehlo v poradku
	return 0;
}

