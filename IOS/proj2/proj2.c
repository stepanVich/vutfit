#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include "proj2.h"

int main(int argc, char *argv[] )
{
  // Basic init
    // Set buffer
  output_file = fopen(OUTPUT, "w");
  setbuf(output_file, NULL);
    // Nastaveni signalu
  zpracuj_signaly();
    // Zpracovani argumentu
  options = zpracuj_vstupy(argc, argv);
  // Prepare share memory
  share_int_var(cislo_procesu_id, cislo_procesu, 1);
  *cislo_procesu = 0;
  share_int_var(cislo_pasazera_id, cislo_pasazera, 2);
  *cislo_pasazera = 0;
  share_int_var(boarders_id, boarders, 3);
  *boarders = 0;
  share_int_var(unboarders_id, unboarders, 4);
  *unboarders = 0;
  share_int_var(cislo_vypisu_id, cislo_vypisu, 5);
  *cislo_vypisu = 0;
  share_int_var(board_order_id, board_order, 6);
  *board_order = 0;
  share_int_var(unboard_order_id, unboard_order, 7);
  *unboard_order = 0;
  share_int_var(final_barrier_counter_id, final_barrier_counter, 8);
  *final_barrier_counter = 0;

  // Prepare semaphores
  share_sem(mutex, 1, 1);
  share_sem(mutex2, 1, 2);
  share_sem(boardQueue, 0, 3);
  share_sem(unboardQueue, 0, 4);
  share_sem(allAboard, 0, 5);
  share_sem(allAshore, 0, 6);
  share_sem(mutex_output, 1, 7);
  share_sem(mutex_board_order, 1, 8);
  share_sem(mutex_unboard_order, 1, 9);
  share_sem(mutex_randezvous, 1, 10);
  share_sem(final_barrier, 0, 11);
  // Create first level of processes

  // Array of pids for passengers
  pid_t passengers_pid[options.P];
  // Initialize array
  for(int i = 0; i < options.P; i++)
    passengers_pid[i] = 0;

  // New branch for car process and for process that generate passenger process
  pid_main = fork();

  if( pid_main < 0 )
  {
    vycisteni_pameti();
    fatal_error("Chyba u forku vozitka.\n");
  }

  if( pid_main == 0 )
  {
    // Code for car process...
    car_process();
  }
  else
  // pid_main > 0
  {
    // Parent process and generating of passengers processes...
    // Cyklus generovani pasazeru
    for(int i = 0; i < options.P; i++)
    {
      int generation_time_ms = ( options.PT == 0 ? 0 : (random() % options.PT) * 1000 );
      // Uspi se pekne, pred generovanim procesu
      if(generation_time_ms > 0) usleep(generation_time_ms);
      // Vytvor proces pasazera
      pid_passenger = fork();
      if( pid_passenger < 0 )
      {
        // Zabij hlavni proces
        kill(pid_main, SIGKILL);
        // Zabij vsechny jiz vytvorene procesy
        for( int killer = 0; killer < i; killer++ )
          kill(passengers_pid[killer], SIGKILL);
        // Vycisti pamet
        vycisteni_pameti();
        // Vypis zpravu a ukonci se
        fatal_error("Chyba pri vytvareni podprocesu.\n");
      }
      if( pid_passenger == 0 )
      {
        // Jsme primo v procesu pasazera
        passenger_process();
      }
      if( pid_passenger > 0 )
      {
        passengers_pid[i] = pid_passenger;
      }
    }
  }

  // Vytvoreni cekaci fronty
    // Cekame na vsechny pasazery
  for(int i = 0; i < options.P; i++)
    waitpid( passengers_pid[i], NULL, 0);

    // Cekame na hlavni proces, ktery generuje vozitko a pasazery
  waitpid(pid_main, NULL, 0);

  // Slavnoste vycistit pamet
  vycisteni_pameti();
  // Uzavrit soubor
  fclose(output_file);
  // A rozloucit se :)
  return EXIT_SUCCESS;
}

void zpracuj_signaly()
{
  signal(SIGINT, vycistit_ukoncit);
  signal(SIGKILL, vycistit_ukoncit);
}

programArgs zpracuj_vstupy(int argc, char *argv[])
{
  
  programArgs args;
  // Default values
  args.P = 1;
  args.C = 1;
  args.PT = 0;
  args.RT = 0;

  // Parsing values from arguments
  // Parsing first argument P
  if( argc >= 2 && isnumber(argv[1]) && atoi(argv[1]) > 0 )
  {
    args.P = atoi(argv[1]);
  }
  else
  {
    arg_error("Parametr P nezadan.\n");
  }
  // Parsing second argument C
  if( argc >= 3 && isnumber(argv[2]) && atoi(argv[2]) > 0  )
  {
    args.C = atoi(argv[2]);
  }
  else
  {
    arg_error("Parametr C nezadan.\n");
  }

  // Kontrola parametru P vzhledem k C
  if(  args.P <= args.C )
  {
    arg_error("Parametr P je mensi nebo roven C.\n");
  }
  if( args.P % args.C != 0  )
  {
    arg_error("Parametr P neni nasobkem C.\n");
  }

  // Parsing third argument PT
  if( argc >= 4 && isnumber(argv[3]) )
  {
    int number = atoi(argv[3]);
    args.PT = number;

    if( number < 0 )
    {
      arg_error("Parametr PT je mensi nez 0.\n");
    }
    else if( number > 5000 )
    {
      arg_error("Parametr PT je vetsi nez 5000.\n");
    }
  }
  else
  {
    arg_error("Parametr PT nezadan.\n");
  }
  // Parsing fourt argument RT
  if( argc >= 5 && isnumber(argv[4]) )
  {
    int number = atoi(argv[4]);
    args.RT = number;
  
    if( number < 0 )
    { 
      arg_error("Parametr RT mensi nez 0.\n");
    }
    else if( number > 5000 )
    { 
      arg_error("Parametr RT vetsi nez 5000.\n");
    } 
  }
  else
  {
    arg_error("Parametr RT nezadan.\n");
  }

  return args;  
}

// Help function recognizing digit in string
bool isnumber(char *string)
{
  int len = strlen(string);
  if(len == 0) return false;
  while(len--)
  {
    if( ! isdigit(string[len]) ) return false;
  }
  return true;
}

// Vola se pri chybe za behu programu, kdy jsou jiz naalokovany vsechny sdilene prostredky -> smaze vsechny sdilene promenne
void vycisteni_pameti()
{
  smazani_sdilenych_prom(8);
  smazani_semaforu(11);
}
void vycistit_ukoncit()
{
  vycisteni_pameti();
  exit(2);
}
void smazani_sdilenych_prom(int index)
{
  if(index >= 1) remove_share_var(cislo_procesu_id, cislo_procesu); 
  if(index >= 2) remove_share_var(cislo_pasazera_id, cislo_pasazera);
  if(index >= 3) remove_share_var(boarders_id, boarders);
  if(index >= 4) remove_share_var(unboarders_id, unboarders);
  if(index >= 5) remove_share_var(cislo_vypisu_id, cislo_vypisu);
  if(index >= 6) remove_share_var(board_order_id, board_order);
  if(index >= 7) remove_share_var(unboard_order_id, unboard_order);
  if(index >= 8) remove_share_var(final_barrier_counter_id, final_barrier_counter);
}
void smazani_semaforu(int index)
{
  if(index >= 1) remove_share_sem(mutex); 
  if(index >= 2) remove_share_sem(mutex2);
  if(index >= 3) remove_share_sem(boardQueue);
  if(index >= 4) remove_share_sem(unboardQueue);
  if(index >= 5) remove_share_sem(allAboard);
  if(index >= 6) remove_share_sem(allAshore);
  if(index >= 7) remove_share_sem(mutex_output); 
  if(index >= 8) remove_share_sem(mutex_board_order);
  if(index >= 9) remove_share_sem(mutex_unboard_order); 
  if(index >= 10) remove_share_sem(mutex_randezvous);
  if(index >= 11) remove_share_sem(final_barrier);
}

// Procesy
void car_process()
{

  // Inkrementace procesovych informaci
  (*cislo_procesu) ++;

  car_started();

  // Hlavni cyklus procesu - P/C cyklu
  for(int i = 0; i < options.P / options.C; i++)
  {
    // I.    load()
    load();
    // Nastavi hodnotu semaforu na C
    // II.   boardQueue.signal(C)
    for(int i = 0; i < options.C ; i++)
    {
      if( sem_post(boardQueue) == -1 ) sem_fail();
    }
    // III.  allAboard.wait()
    if( sem_wait(allAboard) == -1 ) sem_fail();

    // IV.   run()
    run();

    // V.    unload()
    unload();

    // VI.   unboardQueue.signal(C)
    for(int i = 0; i < options.C ; i++)
    {
      if( sem_post(unboardQueue) == -1 ) sem_fail();
    }

    // VII.  allAshore.wait()
    if( sem_wait(allAshore) == -1 ) sem_fail();

  } // Konec hlavniho cyklu
  final_barrier_fc();
  tiskni_radek('C', 1, FINISH, 0);
  ukonceni_podprocesu();
}
void final_barrier_fc()
{
  // randezvous for finished indicate
  if(sem_wait(mutex_randezvous) == -1) sem_fail();
    (*final_barrier_counter) ++;
  if(sem_post(mutex_randezvous) == -1) sem_fail();

  if( (*final_barrier_counter) == options.P + 1 )
  {
    if(sem_post(final_barrier) == -1) sem_fail();  
  }  

  if(sem_wait(final_barrier) == -1) sem_fail();
  if(sem_post(final_barrier) == -1) sem_fail();
}
void car_started()
{
  tiskni_radek('C', 1, STARTED, 0);
}
void load()
{
  tiskni_radek('C', 1, LOAD, 0);
}
void run()
{
  tiskni_radek('C', 1, RUN, 0);
  int generation_time_ms = ( options.RT == 0 ? 0 : (random() % options.RT) * 1000 );
  // Uspi se pekne, pred generovanim procesu
  if(generation_time_ms > 0) usleep(generation_time_ms);  
}
void unload()
{
  tiskni_radek('C', 1, UNLOAD, 0);
}
void passenger_process()
{

  // Inkrementace procesovych informaci
  int fix_cislo_pasazera = ++ (*cislo_pasazera);

  passenger_started(fix_cislo_pasazera);

  // I.    boardQueue.wait()
  if( sem_wait(boardQueue) == -1 ) sem_fail();

  // II.   board()
  board(fix_cislo_pasazera);

  // III.  mutex.wait()
  if( sem_wait(mutex) == -1 ) sem_fail();
    (*boarders) ++;
    if( (*boarders) == options.C )
    {
      // III.I  allAboard.signal()
      if( sem_post(allAboard) == -1 ) sem_fail();
      (*boarders) = 0;
      (*board_order) = 0;
    }
  // IV.   mutex.signal()
  if( sem_post(mutex) == -1 ) sem_fail();

  // V.    unboardQueue.wait()
  if( sem_wait(unboardQueue) == -1 ) sem_fail();

  // VI.   unboard()
  unboard(fix_cislo_pasazera);

  // VII.  mutex2.wait()
  if( sem_wait(mutex2) == -1 ) sem_fail();

    (*unboarders) ++;
    if( (*unboarders) == options.C )
    {
      // III.I  allAshore.signal()
      if( sem_post(allAshore) == -1 ) sem_fail();
      (*unboarders) = 0;
    }
  // IV.   mutex2.signal()
  if( sem_post(mutex2) == -1 ) sem_fail();  

  // randezvous for finished indicate
  final_barrier_fc();
  tiskni_radek('P', fix_cislo_pasazera, FINISH, 0);
  ukonceni_podprocesu();

}

void passenger_started(int passenger_id)
{
  tiskni_radek('P', passenger_id, STARTED, 0);
}

void board(int passenger_id)
{
  if (sem_wait(mutex_board_order) == -1) sem_fail();
    (*unboard_order) = 0;
    // Najit lepsi reseni
    tiskni_radek('P', passenger_id, BOARD, 1);
    (*board_order) ++;
    (*cislo_vypisu) ++;
    if( (*board_order) == options.C )
    {
      fprintf(output_file, "%-8i: P %-4i: board last\n", (*cislo_vypisu), passenger_id);
    }
    else
    {
      fprintf(output_file, "%-8i: P %-4i: board order %i\n", (*cislo_vypisu), passenger_id, (*board_order));
    }
  if (sem_post(mutex_board_order) == -1) sem_fail();
}

void unboard(int passenger_id)
{
  if (sem_wait(mutex_board_order) == -1) sem_fail();
    (*board_order) = 0;
    // Najit lepsi reseni
    tiskni_radek('P', passenger_id, UNBOARD, 1); 
    (*unboard_order) ++;
    (*cislo_vypisu) ++;
    if( (*unboard_order) == options.C )
    {
      fprintf(output_file, "%-8i: P %-4i: unboard last\n", (*cislo_vypisu), passenger_id);
    }
    else
    {
      fprintf(output_file, "%-8i: P %-4i: unboard order %i\n", (*cislo_vypisu), passenger_id, (*unboard_order));
    }
  if (sem_post(mutex_board_order) == -1) sem_fail();  
  
}

void ukonceni_podprocesu()
{

  // Spravne odstrani s podprocesu fragmenty promennych
  shmctl(cislo_procesu_id, IPC_RMID, NULL);
  shmctl(cislo_pasazera_id, IPC_RMID, NULL);
  shmctl(boarders_id, IPC_RMID, NULL);
  shmctl(unboarders_id, IPC_RMID, NULL);
  shmctl(cislo_vypisu_id, IPC_RMID, NULL);
  shmctl(board_order_id, IPC_RMID, NULL);
  shmctl(unboard_order_id, IPC_RMID, NULL);
  shmctl(final_barrier_counter_id, IPC_RMID, NULL);
  // ukonci podproces
  exit(EXIT_SUCCESS);
}

void sem_fail()
{
  vycisteni_pameti();
  fatal_error("Chyba pri praci se semaforem.\n");
}
void tiskni_radek(char process_name, int process_index, char *output, int order)
{
  // Zabraneni ze dva procesy pracuji s vystupnim terminalem
  if( sem_wait(mutex_output) == -1 ) sem_fail();
    (*cislo_vypisu) ++;
    // Hlavni formatovaci retezce.. (Grafici a spropadeni klikari, divejte se tady :D)
    fprintf(output_file, "%-8i: %c %-4i: %s\n", *cislo_vypisu, process_name, process_index, output);

    if( strcmp(output, BOARD) == 0 && strcmp(output, UNBOARD) == 0 )
    {
      (*cislo_vypisu) ++;
      fprintf(output_file, "%-8i: %c %-4i: %s order %i\n", *cislo_vypisu, process_name, process_index, output, order);
    }
    
  if( sem_post(mutex_output) == -1 ) sem_fail();
}
