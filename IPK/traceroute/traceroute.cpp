/**
 * @brief Tento program predstavuje jednoduchou implementaci nastroje traceroute v jazyce C++.
 * @author Stepan Vich - xvichs00 - stepan.vich96@gmail.com
 * @date 7.4.2017
 */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/time.h>
#include <assert.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <ctime>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <linux/errqueue.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>

#define BUFFER_MAX_SIZE 1024

// Makra formatujici vystup
#define normal_station "%2i   %s (%s)   %.3lf ms\n"
#define error_station "%2i   %s (%s)   %c!\n"
#define timeout_station "%2i   *\n"


int main(int argc, char ** argv) {

  // Zpracovavani argumentu pomoci getopt
  // Pomocne promenne pro ulozeni hodnot argumentu
  int first_ttl_flag = 0;
  char *first_ttl;
  int first_ttl_int = 1;
  int max_ttl_flag = 0;
  char *max_ttl;
  int max_ttl_int = 30;
  char *ip_address;
  int c;
  int index;

  // Cyklus nad argumenty s hodnotou
  while((c = getopt(argc, argv, "f:m:p")) != -1) {
    switch(c) {
      case 'f':
        first_ttl = optarg;
        first_ttl_flag = 1;
        break;
      case 'm':
        max_ttl = optarg;
        max_ttl_flag = 1;
        break;
    }
  }

  // Povinne argumenty  
  // IP adresa
  index = optind;  
  ip_address = argv[index];

  // Kontrola dalsich nepovolenych argumentu
  index ++;
  if(index < argc) {
    // V poli jsou jeste nejake hodnoty
    fprintf(stderr, "Nepovoleny pocet argumetu.\n");
    exit(1);
  }
  // Kontrola hodnota parametru
  // Int u max_ttl a first_ttl
  if(first_ttl_flag) {
    int first_ttl_length = strlen(first_ttl);
    for(index = 0; index < first_ttl_length; index++) {
      if(isdigit(first_ttl[index])) continue;
      fprintf(stderr, "Hodnota first_ttl neni rovna cislu.\n");
      exit(1);
    }
    first_ttl_int = atoi(first_ttl);
  }
  if(max_ttl_flag) {  
    int max_ttl_length = strlen(max_ttl);
    for(index = 0; index < max_ttl_length; index++) {
      if(isdigit(max_ttl[index])) continue;
      fprintf(stderr, "Hodnota max_ttl neni rovna cislu.\n");
      exit(1);
    }
    max_ttl_int = atoi(max_ttl);
  }
  // Kontrola rozsahu hodnot
  if(max_ttl_int < 0 || first_ttl_int < 0) {
    fprintf(stderr, "Hodnota max ttl a first ttl je mimo platny rozsah.\n");
    exit(1);
  }
  // Kontrola ip_adresy
  if(ip_address == NULL) {
    fprintf(stderr, "Zadejte IP adresu.\n");
    exit(1);
  }

  // Nastaveni IP adresy
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;

  if(getaddrinfo(ip_address, "33434", &hints, &result) != 0) {
    fprintf(stderr, "Zadejte validni IP adresu.\n");
    exit(1);
  }
  // Vytvoreni UDP Socketu
  int sock_udp;
  for(rp = result; rp != NULL; rp = rp->ai_next) {
    sock_udp = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if(sock_udp == -1) continue;
    if(connect(sock_udp, rp->ai_addr, rp->ai_addrlen) != -1) break;
    close(sock_udp);
  }

  if(rp == NULL) {
    fprintf(stderr, "Nebylo mozne se pripojit k adrese.\n");
    exit(2);
  }

  int actual_ttl = first_ttl_int;
  int destinationReached = 0;
  int currentHop = 1;
  
  // Cyklus pro posilani ICMP zprav pomoci protokolu UDP
  for(; actual_ttl <= max_ttl_int && !destinationReached ; actual_ttl++) {

    // Nastaveni TTL
    if(rp->ai_family == AF_INET) {
      setsockopt(sock_udp, IPPROTO_IP, IP_TTL, &actual_ttl, sizeof(actual_ttl));
    } else {
      setsockopt(sock_udp, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &actual_ttl, sizeof(actual_ttl));
    }

    // Nastaveni casu  
    struct timeval start, end;
    int terminated = 1;
    unsigned long long elapsed = 0;
    gettimeofday(&start, NULL);
    gettimeofday(&end, NULL);


    int on = 1;
    /* Set the option, so we can receive errors */
    if(rp->ai_family == AF_INET) {
      setsockopt(sock_udp, SOL_IP, IP_RECVERR,(char*)&on, sizeof(on));
    } else {
      setsockopt(sock_udp, SOL_IPV6, IPV6_RECVERR, (char*)&on, sizeof(on));
    }

    char secretMessage[] = "Tajna zprava\n";
    if(send(sock_udp, secretMessage, sizeof(secretMessage) + 1, 0) < 0) {
      fprintf(stderr, "Doslo k chybe pri odeslani paketu.\n");
      break;
    }

    // Prijmani ICMP chyb
    int return_status;
    char buffer[BUFFER_MAX_SIZE];
    struct iovec iov;                       
    struct msghdr msg;                     
    struct cmsghdr *cmsg;                 
    struct icmphdr icmph;
    struct sockaddr_storage target;
    
    while (terminated)
    {
      // Pomocne struktury
      iov.iov_base = &icmph;
      iov.iov_len = sizeof(icmph);
      msg.msg_name = &target;
      msg.msg_namelen = sizeof(target);
      msg.msg_iov = &iov;
      msg.msg_iovlen = 1;
      msg.msg_flags = 0;
      msg.msg_control = buffer;
      msg.msg_controllen = sizeof(buffer);

      // Ziskame cas prichodu paketu
      // Nastaveni casu
      // 2s
      if(elapsed >= 2000000) {
        // Doslo k ukonceni smycky z duvodu vyprseni timeoutu
        terminated = 0;
        printf(timeout_station, currentHop);
        break;
      }

      // Hlavni funkce kernelu pro prijmani ICMP chyb 
      return_status = recvmsg(sock_udp, &msg, MSG_ERRQUEUE);
      if (return_status < 0) {
        gettimeofday(&end, NULL);
        elapsed =
	  ((unsigned long long)(end.tv_sec - start.tv_sec))*1000000 +
	  ((unsigned long long)(end.tv_usec - start.tv_usec));
        continue;
      }

      gettimeofday(&end, NULL);
      elapsed =
        ((unsigned long long)(end.tv_sec - start.tv_sec))*1000000 +
        ((unsigned long long)(end.tv_usec - start.tv_usec));
      // Preved mikrosekundy na milisekunkdy
      double elapsedMs = elapsed / 1000.0;
      int badMsg = 0;

      // Projizdej jednotlive zpravy
      for (cmsg = CMSG_FIRSTHDR(&msg);cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
        // IP Uroven
        if (cmsg->cmsg_level == SOL_IP || cmsg->cmsg_level == SOL_IPV6) {
          // Jednotlive zpravy
          if (cmsg->cmsg_type == IP_RECVERR || cmsg->cmsg_type == IPV6_RECVERR) {
            struct sock_extended_err *sock_err = (struct sock_extended_err*)CMSG_DATA(cmsg);
            if(sock_err) {

              // Ziskej retezce IP adres              
              // Jakmile ziskas validni ipv4/6 retezec, ziskej hostname pokud mozno
              char networkString[INET6_ADDRSTRLEN];
              char hostString[NI_MAXHOST];
              char *ipv4;
              struct hostent *he;
              // Ziskej ipv4 adresu zdroje
              if(rp->ai_family == AF_INET) {
                struct sockaddr_in* source_ip = (struct sockaddr_in *)(SO_EE_OFFENDER(sock_err));
                // Ziskej IP adresu
                ipv4 = inet_ntoa(source_ip->sin_addr);
                strcpy(networkString, ipv4);
                // Ziskej hostname
                struct in_addr ipv4addr;
                inet_pton(AF_INET, networkString, &ipv4addr); 
                he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
                if(he != NULL) {
               	  strcpy(hostString, he->h_name);
                } else {
                  // Prevod se nezdaril
                  strcpy(hostString, networkString);
                }
              // Ziskej ipv6 adresu zdroje  
              } else if(rp->ai_family == AF_INET6) {
                struct sockaddr_in6* source_ip = (struct sockaddr_in6 *)(SO_EE_OFFENDER(sock_err));
                // Ziskej IP adresu
                struct in6_addr sin = (struct in6_addr)(source_ip->sin6_addr);
                inet_ntop(AF_INET6, sin.s6_addr, networkString, INET6_ADDRSTRLEN);
                // Ziskej hostname
                struct in6_addr ipv6addr;
                inet_pton(AF_INET6, networkString, &ipv6addr); 
                he = gethostbyaddr(&ipv6addr, sizeof ipv6addr, AF_INET6);
                if(he != NULL) {
                  strcpy(hostString, he->h_name);
                } else {
                  // Prevod se nezdaril
                  strcpy(hostString, networkString);
                }
	      }


              // Kontrola ze adresa zdroje je stejna se zadanou adresou
              if(strcmp(networkString, ip_address) == 0 || strcmp(hostString, ip_address) == 0) {
                destinationReached = 1;
              }

              if(sock_err->ee_origin == SO_EE_ORIGIN_ICMP) {
                // Setridime prichozi errory podle typu a kodu chyby
                
                if(sock_err->ee_type == 3) {
                  switch(sock_err->ee_code) {
                    // Destination network unreachable
                    case 0:
                      printf(error_station, currentHop, hostString, networkString, 'H');
                      destinationReached = 1;
                      break;
                    // Destination host unreachable
                    case 1:
                      printf(error_station, currentHop, hostString, networkString, 'N');
                      destinationReached = 1;
                      break;
                    // Destination protocol unreachable
                    case 2:
                      printf(error_station, currentHop, hostString, networkString, 'P');
                      destinationReached = 1;
                      break;
                    // Destination port unreachable
                    // Dosazeno ciloveho mista
                    case 3:
                      printf(normal_station, currentHop, hostString, networkString, elapsedMs);
                      destinationReached = 1;
                      break;
                    // communication administratively prohibited
                    case 13:
                      printf(error_station, currentHop, hostString, networkString, 'X');
                      destinationReached = 1;
                      break;
                    // Jine pripady
                    default:
                      badMsg = 1;
                      break;
                  }
                } else if (sock_err->ee_type == 11) {
                  if(sock_err->ee_code == 0) {
                    // TTL Expired in transit
                    printf(normal_station, currentHop, hostString, networkString, elapsedMs);
                  } else {
                    // Zpracovani zpravy s neznamym kodem
                    badMsg = 1;
                  }
                }
              } else if(sock_err->ee_origin == SO_EE_ORIGIN_ICMP6) {
                // Destination network unreachable
                if (sock_err->ee_type == 1 && sock_err->ee_code == 0) {
                  printf(error_station, currentHop, hostString, networkString, 'H');
                  destinationReached = 1;
                } 
                // Destination host unreachable
                else if (sock_err->ee_type == 1 && sock_err->ee_code == 3) {
                  printf(error_station, currentHop, hostString, networkString, 'N');
                  destinationReached = 1;
                }
                // Destination protocol unreachable
                else if (sock_err->ee_type == 4 && sock_err->ee_code == 1) {
                  printf(error_station, currentHop, hostString, networkString, 'P');
                  destinationReached = 1;
                }
                // Communication administratively prohibited
                else if (sock_err->ee_type == 1 && sock_err->ee_code == 1) {
                  printf(error_station, currentHop, hostString, networkString, 'X');
                  destinationReached = 1;
                }
                // TTL Expired in transit
                else if (sock_err->ee_type == 3 && sock_err->ee_code == 0) {
                  printf(normal_station, currentHop, hostString, networkString, elapsedMs);
                }
                // Destination port unreachable
                // Dosazeno ciloveho mista
                else if (sock_err->ee_type == 1 && sock_err->ee_code == 4) {
                  printf(normal_station, currentHop, hostString, networkString, elapsedMs);
                  destinationReached = 1;
                }
                // Jine pripady
                 else {
                  badMsg = 1;
                }
              } else {
                // Zpracovani zpravy s neznamym typem
                badMsg = 1;
              }

              // Pokud byla prijata ICMP zprava vporadku, prejde se na odeslani dalsiho paketu s vyssim ttl
              // Pri prijeti zpravy, kterou nerozpozname se ceka na ukonceni timeoutu, nebo prijeti zpravy kterou rozezname
              if(!badMsg) {
                // Ukonceni vnejsiho cyklu
                terminated = 0;
                // Ukonceni vnitrniho (tohoto) cyklu
                break;
              }
            }
          }
        } 
      }
    }

    // Inkrementace aktualniho cyklu
    currentHop ++;
  }

  // Vse probehlo v poradku, ukoncime program
  return 0;
};

