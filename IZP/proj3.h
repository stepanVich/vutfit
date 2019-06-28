/**
 * Kostra hlavickoveho souboru 3. projekt IZP 2015/16
 * a pro dokumentaci Javadoc.
 */


/**
 * @brief      Datova struktura pro ulozeni informaci o jednotlivych objektech.
 */
struct obj_t {
	/// id objektu
    int id;
    /// souradnice x objektu
    float x;
    /// souradnice y objektu
    float y;
};

/**
 * @brief      Datova struktura pro ulozeni informaci o shlucich obsahujicich objekty.
 */
struct cluster_t {
	/// pocet objektu ve shluku
    int size;
    /// alokovany prostor pro pole objektu
    int capacity;
    /// ukazatel na pole objektu
    struct obj_t *obj;
};

/**
 * @defgroup shluk Operace se shlukem
 * @defgroup pole_shluku Operace nad polem shluku
 * @addtogroup shluk
 * @{ 
 */


/**
 * @brief      Nastavi hodnoty shluku a alokuje pamet pro pole objektu shluku pomoci funkce malloc.
 *
 * @param      c     Shluk
 * @param[in]  cap   Hodnota na kterou ma byt nastavena kapacita shluku
 */
void init_cluster(struct cluster_t *c, int cap);

/**
 * @brief      Deaklokuje pamet urcenou pro pole objektu ve shluku a pote inicializuje shluk na kapacitu 0. 
 *
 * @param      c     Shluk
 */
void clear_cluster(struct cluster_t *c);

/**
 * Konstanta slouzici pro pocatecni velikost kapacity shluku.
 */
extern const int CLUSTER_CHUNK;

/**
 * @brief      Realokuje pamet urcenou pro pole objektu shluku pomoci funkce realloc a prepise hodnotu kapacity shluku na novou hodnotu. 
 *
 * @param      c        Shluk
 * @param[in]  new_cap  Nova kapacita shluku, na kterou ma byt realokovan
 *
 * @return     Vraci zmeneny shluku, nebo hodnotu NULL pokud nastala chyba pri realokaci.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap);

/**
 * @brief      Pridava jeden objekt do pole objektu daneho shluku. Pro zmenu kapacity shluku vola funkci resize_cluster. 
 *
 * @param      c     Shluk
 * @param[in]  obj   Objekt, ktery se pridava do pole objektu daneho shluku.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj);

/**
 * @brief      Pridava vsechny objekty ze shluku c2 do pole objektu shluku c1.
 *
 * @param      c1    Prvni shluk
 * @param      c2    Druhy shluk
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);

/**
 * @}
 */

/**
 * @addtogroup pole_shluku
 * @{
 */

/**
 * @brief      Odstrani shluk na urcenem indexu z pole objektu shluku a vraci novy pocet objektu ve shluku. 
 *
 * @param      carr  Pole objektu shluku
 * @param[in]  narr  Velikost pole objektu shluku
 * @param[in]  idx   Index na kterem se nachazi objekt, jenz bude odstranen
 *
 * @return     Hodnota udavajici novou velikost pole objektu shluku
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx);

/**
 * @brief      Vypocitava vzdalenost mezi dvema objekty pomoci jejich souradnic
 *
 * @param      o1    Prvni objekt shluku
 * @param      o2    Druhy objekt shluku
 *
 * @return     Vzdalenost mezi dvema objekty vypocitana pomoci pythagorovy vety a souradnic x a y obou objektu.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2);

/**
 * @brief      Vypocitava vzdalenost dvou shluku c1 a c2.
 *
 * @param      c1    Prvni shluk c1
 * @param      c2    Druhy shluk c2
 *
 * @return     Vzdalenost mezi dvema shluky zalozena na nejmensi vzdalenosti mezi vsemi objekty.
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);

/**
 * @brief      Nalezne dva nejblizsi shluky z pole shluku carr
 *
 * @param      carr  Pole shluku
 * @param[in]  narr  Velikost pole shluku
 * @param      c1    Hodnota urcujici index shluku, ktery je spolecne se shlukem c2 nejblize se vsech ostatnich dvojic shluku.
 * @param      c2    Hodnota urcujici index shluku, ktery je spolecne se shlukem c1 nejblize se vsech ostatnich dvojic shluku.
 * 
 * @pre narr > 0
 * @post 0 <= *c1 < *c2 < narr
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);

/**
 * @}
 */

 /**
  * @addtogroup shluk
  * @{
  */

/**
 * @brief      Setridi objekty shluku vzestupne podle id objektu.
 *
 * @param      c     Shluk
 */
void sort_cluster(struct cluster_t *c);

/**
 * @brief      Vypise objekty shluku, spolecne s jejich id a souradnicemi x a y.
 *
 * @param      c     Shluk
 */
void print_cluster(struct cluster_t *c);

/**
 * @}
 */

 /**
  * @addtogroup pole_shluku
  * @{
  */

/**
 * @brief      Cte ze souboru jednotlive objekty, pro kazdy vytvari samostatny shluk a ten pak pridave do pole shluku. Pred ctenim pro toto pole alokuje pamet.
 *
 * @param      filename  Nazev, popripade adresa souboru ze ktereho se ctou objekty.
 * @param      arr       
 *
 * @return     { description_of_the_return_value }
 */
int load_clusters(char *filename, struct cluster_t **arr);

/**
 * @brief      Vytiskne cele pole shluku, spolecne s objekty ktere shluky obsahuji. Uvnitr tela vola funkci print_cluster
 *
 * @param      carr  Pole shluku
 * @param[in]  narr  Velikost pole shluku
 * 
 * @pre arr != 0
 */
void print_clusters(struct cluster_t *carr, int narr);

/**
 * @}
 */
