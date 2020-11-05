// TODO: border and border order - vytvorit paralelne ne sekvencne
// TODO: pridat smazani promennych

// Macros
  // Print error message and exit program 
#define fatal_error(error_msg) \
do { \
  fprintf(stderr, error_msg); \
  fclose(output_file); \
  exit(2); \
} while(0)
  // Create variable in shared memory
#define share_int_var(var_id, var_name, index) \
  do \
  { \
    var_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666); \
    if (var_id == -1) fatal_error("Chyba pri ziskavani id promenne ve sdilene pameti.\n"); \
    var_name = (int*)shmat(var_id, NULL, 0); \
    if (var_name == (void*)-1) \
    { \
      smazani_sdilenych_prom(index); \
      fatal_error("Chyba pri attachi promenne.\n"); \
    } \
  } while(0)

#define remove_share_var(var_id, var_name) \
  do \
  { \
    shmdt(var_name); \
    shmctl(var_id, IPC_RMID, NULL); \
  } while(0)

#define share_sem(sem_name, sem_init_value, index) \
  do \
  { \
    sem_name = (sem_t *) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0); \
    if( ( sem_init(sem_name, 1, sem_init_value) ) == -1 ) \
    { \
      smazani_sdilenych_prom(4); \
      smazani_semaforu(index); \
      fatal_error("Chyba pri tvorbe semaforu.\n"); \
    } \
  } while(0)    

#define remove_share_sem(sem_name) \
  do \
  { \
    if ( sem_destroy( &(* sem_name)) == -1 ) \
    { \
      fatal_error("Chyba pri odstraneni semaforu.\n"); \
    } \
  } while(0)


  // String macros for better program editing
#define STARTED "started"
#define LOAD "load"
#define BOARD "board"
#define ORDER "order"
#define LAST_IN_ORDER "last"
#define RUN "run"
#define UNLOAD "unload"
#define UNBOARD "unboard"
#define FINISH "finished"

#define OUTPUT "proj2.out"

#define arg_error(error_msg) \
  do \
  { \
    fprintf(stderr,error_msg); \
    fclose(output_file); \
    exit(1); \
  } while(0)


// Custom type defs
struct program_args {
  int P; // Pocet pasazeru
  int C; // Kapacita voziku
  int PT; // maximalni doba generovani pasazera <0, 5000>
  int RT; // maximalni doba prujezdu <0, 5000>
};
typedef struct program_args programArgs;

// Global variables
  // Share id
int cislo_procesu_id = 0, cislo_pasazera_id = 0, cislo_vypisu_id = 0, boarders_id = 0, unboarders_id = 0, board_order_id = 0, unboard_order_id = 0, final_barrier_counter_id = 0;
  // Share int
int *cislo_procesu, *cislo_pasazera, *cislo_vypisu, *boarders, *unboarders, *board_order, *unboard_order, *final_barrier_counter;
  // Share semaphores
sem_t *mutex, *mutex2, *boardQueue, *unboardQueue, *allAboard, *allAshore, *mutex_output, *mutex_board_order, *mutex_unboard_order, *mutex_randezvous, *final_barrier;
  // Share processes
pid_t pid_main, pid_passenger;
  // Program arguments
programArgs options;
  // Output file descriptor
FILE *output_file;

// Prototypes
  // Castecne pomocne metody
void zpracuj_signaly();
programArgs zpracuj_vstupy();
void vycisteni_pameti();
void vycistit_ukoncit();
bool isnumber(char *string);
void smazani_sdilenych_prom(int index);
void smazani_semaforu(int index);
void ukonceni_podprocesu();
void sem_fail();
void tiskni_radek(char process_name, int process_index, char *output, int order);
void final_barrier_fc();

  // Obecne procesy
void car_process();
void passenger_process();
  // Metody voziku
void car_started();
void load();
void run();
void unload();
  // Metody pasazera
void passenger_started();
void board();
void unboard();

