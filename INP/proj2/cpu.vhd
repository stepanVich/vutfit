library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
entity cpu is
 -- predpripravene vstupy a vystupy
 port (
  CLK         : in  std_logic;
  RESET       : in  std_logic;
  EN          : in  std_logic;
  CODE_ADDR   : out std_logic_vector(11 downto 0);
  CODE_DATA   : in  std_logic_vector(7 downto 0);
  CODE_EN     : out std_logic;
  DATA_ADDR   : out std_logic_vector(9 downto 0);
  DATA_RDATA  : in  std_logic_vector(7 downto 0);
  DATA_WDATA  : out std_logic_vector(7 downto 0);
  DATA_EN     : out std_logic;
  DATA_RDWR   : out std_logic;
  IN_REQ      : out std_logic;
  IN_VLD      : in  std_logic;
  IN_DATA     : in  std_logic_vector(7 downto 0);
  OUT_BUSY    : in  std_logic;
  OUT_DATA    : out std_logic_vector(7 downto 0);
  OUT_WE      : out std_logic
 );
end cpu;

architecture behavioral of cpu is

-- vycet instrukci
type instrukce is (
 I_HALT,
 I_ULOZ,
 I_TMP,
 I_CELL_PLUS,
 I_NACTI,
 I_CELL_MINUS,
 I_ZAPIS,
 I_PTR_PLUS,
 I_PTR_MINUS,
 I_WHILE_START,
 I_WHILE_END,
 I_IGNORE
);



 -- jednotlive stavy automatu - podle obrazku automatu
 type stav_automatu is (S1,S2,S3,S4,S5,S6,S7,S8,S9,S10,S11,S12,S13,S14,S15,S16,S17,S18,S19,S20,S21,S22,S23,S24,S25,S26,S27,S28,S29,S30,S31,S32,S33,S34,S35);


-- pouzite promenne
signal pc_plus           : std_logic;
signal pc_minus          : std_logic;
signal ptr_minus         : std_logic;
signal ptr_plus          : std_logic;
signal cnt_plus          : std_logic;
signal cnt_minus         : std_logic;
signal cti_tmp           : std_logic;
signal mx                : std_logic_vector(1 downto 0)  := (others => '0');
signal registr_pc        : std_logic_vector(11 downto 0) := (others => '0');
signal registr_ptr       : std_logic_vector(9 downto 0)  := (others => '0');
signal registr_cnt       : std_logic_vector(7 downto 0)  := (others => '0');
signal registr_tmp       : std_logic_vector(7 downto 0)  := (others => '0');

signal aktualni_stav     : stav_automatu;
signal nasledujici_stav  : stav_automatu;
signal aktivni_instrukce : instrukce;

-- jednotlive procesy procesoru
begin



 cnt_zmena : process(RESET, CLK, cnt_plus , cnt_minus)
 begin
  if RESET = '1' then
   registr_cnt <= (others => '0');
  elsif CLK'event and CLK = '1' then
   if cnt_plus = '1' then
    registr_cnt <= registr_cnt + 1;
   elsif cnt_minus = '1' then
    registr_cnt <= registr_cnt - 1;
   end if;   
  end if;
 end process cnt_zmena;


 pc_zmena : process(RESET, CLK, pc_plus, pc_minus)
 begin
  if RESET = '1' then
   -- vynuluj registr
   registr_pc <= (others => '0');
  elsif CLK'event and CLK = '1' then
   if pc_plus = '1' then
    -- inkrementuj registr
    registr_pc <= registr_pc + 1;
   elsif pc_minus = '1' then
    -- dekrementuj registr
    registr_pc <=  registr_pc - 1;
   end if;
  end if;
 end process pc_zmena;



 zmena_ukazatele : process(RESET, CLK, ptr_minus, ptr_plus)
 begin
  if RESET = '1' then
   -- vynuluj registr
   registr_ptr <=  (others => '0');
  elsif CLK'event and CLK = '1' then
   if ptr_minus = '1' then
    registr_ptr <=  registr_ptr -  1;
   elsif ptr_plus = '1' then
    registr_ptr <=  registr_ptr +  1;
   end if;
  end if;
 end process zmena_ukazatele;



 mx_zmena : process(mx)
 begin
  case (mx) is
   when "00"   =>  DATA_WDATA  <= IN_DATA;
   when "10"   =>  DATA_WDATA  <= DATA_RDATA - 1;
   when "01"   =>  DATA_WDATA  <= registr_tmp;
   when others =>  DATA_WDATA  <= DATA_RDATA + 1;
  end case;
 end process mx_zmena;


 cteni_tmp : process(RESET, CLK, cti_tmp)
 begin
  if (RESET = '1') then
  -- vynulovani registru
   registr_tmp <= (others => '0');
  else
   if CLK'event and CLK = '1' and cti_tmp = '1' then
    registr_tmp <= DATA_RDATA;
   end if;
  end if;
 end process cteni_tmp;


 cteni_instrukce : process(CODE_DATA)
 --nacti instrukci a prirad ji jednu z hodnot vyctoveho typu
 begin
  case CODE_DATA  is
   when X"00"  =>  aktivni_instrukce  <= I_HALT;
   when X"21"  =>  aktivni_instrukce  <= I_ULOZ;
   when X"24"  =>  aktivni_instrukce  <= I_TMP;
   when X"2B"  =>  aktivni_instrukce  <= I_CELL_PLUS;
   when X"2C"  =>  aktivni_instrukce  <= I_NACTI;
   when X"2D"  =>  aktivni_instrukce  <= I_CELL_MINUS;
   when X"2E"  =>  aktivni_instrukce  <= I_ZAPIS;      
   when X"3E"  =>  aktivni_instrukce  <= I_PTR_PLUS;
   when X"3C"  =>  aktivni_instrukce  <= I_PTR_MINUS;
   when X"5B"  =>  aktivni_instrukce  <= I_WHILE_START;
   when X"5D"  =>  aktivni_instrukce  <= I_WHILE_END;
   when others =>  aktivni_instrukce  <= I_IGNORE;
  end case;
 end process cteni_instrukce;



 posun_automatu : process(RESET, EN, CLK)
 begin
  if (RESET = '1') then
   aktualni_stav <= S2;
  elsif (CLK'event) and (CLK = '1') and (EN = '1') then
   aktualni_stav <= nasledujici_stav;
  end if;
 end process posun_automatu;



 vykonani_stavu : process(aktivni_instrukce , aktualni_stav, OUT_BUSY, DATA_RDATA , registr_cnt, IN_VLD)
 begin

  -- Inicializace vsech hodnot - krome aktualniho stavu
  cnt_plus <= '0';
  cnt_minus <= '0';
  pc_plus <= '0';
  ptr_minus <= '0';
  mx <= "00";
  DATA_RDWR <= '1';
  ptr_plus <= '0';
  IN_REQ <= '0';
  cti_tmp <= '0';
  DATA_EN <= '0';
  pc_minus <= '0';
  OUT_WE <= '0';

  -- srdce interpretu
  -- jednotlive stavy konecneho automatu

  case aktualni_stav is

   when S1 =>
    case aktivni_instrukce is

     when I_HALT =>
      nasledujici_stav <= S27;

     when I_ULOZ =>
      mx <= "01";
      DATA_RDWR <= '0';
      DATA_EN <= '1';
      nasledujici_stav <= S13;

     when I_TMP =>
      DATA_EN <= '1';
      nasledujici_stav <= S29;


     when I_CELL_PLUS =>
      nasledujici_stav <= S4;

     when I_NACTI =>
      IN_REQ <= '0';
      DATA_EN <= '1';
      mx <= "00";
      nasledujici_stav <= S8;

     when I_CELL_MINUS =>
      nasledujici_stav <= S6;

     when I_ZAPIS =>
      IN_REQ <= '1';
      DATA_EN <= '1';
      nasledujici_stav <= S11;

     when I_PTR_PLUS =>
      nasledujici_stav <= S14;
    

     when I_PTR_MINUS =>
      nasledujici_stav <= S15;    
    
    

     when I_WHILE_START =>
      CODE_EN <= '1';
      DATA_EN <= '1';
      nasledujici_stav <= S16;
    

     when I_WHILE_END =>
      CODE_EN <= '1';
      DATA_EN <= '1';
      nasledujici_stav <= S21;

     when I_IGNORE =>
      nasledujici_stav <= S28;      
    
     when others => null;
    
    end case;


   when S2 =>
    nasledujici_stav <= S3;


   when S3 =>
    CODE_EN <= '1';
    nasledujici_stav <= S1;


   when S4 =>
    DATA_EN <= '1';
    DATA_RDWR <= '1';
    nasledujici_stav <= S5;


   when S5 =>
    DATA_EN <= '1';
    DATA_RDWR <= '0';
    mx <= "11";
    pc_plus <= '1';
    nasledujici_stav <= S3;

   when S6 =>
    DATA_RDWR <= '1';
    DATA_EN <= '1';
    nasledujici_stav <= S7;

   when S7 =>
    pc_plus <= '1';
    DATA_EN <= '1';
    DATA_RDWR <= '0';
    mx <="10";
    nasledujici_stav <= S3;

   when S8 =>
    IN_REQ <= '1';
    DATA_EN <= '1';
    mx <= "00";
    if (IN_VLD = '1') then
     DATA_RDWR <= '0';
     nasledujici_stav <= S9;
    else
     nasledujici_stav <= S8;
    end if;


   when S9 =>
    DATA_RDWR <= '0';
    DATA_EN <= '1';
    nasledujici_stav <= S10;


   when S10 =>
    pc_plus <= '1';
    nasledujici_stav <= S3;


   when S11 =>
    DATA_EN <= '1';
    nasledujici_stav <= S12;


   when S12 =>
    if (OUT_BUSY = '0') then
     OUT_DATA <= DATA_RDATA;
     OUT_WE <= '1';
     pc_plus <= '1';             
     nasledujici_stav <= S3;
    else
     nasledujici_stav <= S12;
    end if;


   when S13 =>
    pc_plus <= '1';
    nasledujici_stav <= S3;

   when S14 =>
    ptr_plus <= '1';
    pc_plus <= '1';
    nasledujici_stav <= S3;


   when S15 =>
    pc_plus <= '1';
    ptr_minus <= '1';
    nasledujici_stav <= S3;


   when S16 =>
    CODE_EN <= '1';
    DATA_RDWR <= '1';
    nasledujici_stav <= S17;


   when S17 =>
    pc_plus <= '1';
    if (DATA_RDATA = "00000000") then
     cnt_plus <= '1';
     CODE_EN <= '1';
     nasledujici_stav <= S18;
    else
     nasledujici_stav <= S3;
    end if;


   when S18 =>
    CODE_EN <= '1';
    nasledujici_stav <= S19;


   when S19 =>
    if (registr_cnt = "00000000") then
     nasledujici_stav <= S3;
    else
     CODE_EN <= '1';
     pc_plus <= '1';
     nasledujici_stav <= S20;
    end if;


   when S20 =>
    CODE_EN <='1';
    case (aktivni_instrukce) is
     when I_WHILE_START =>
      cnt_plus <= '1';
     when I_WHILE_END =>
      cnt_minus <= '1';
     when others => null;
    end case;
    nasledujici_stav <= S18;


   when S21 =>
    DATA_EN <= '1';
    nasledujici_stav <= S31;


   when S31 =>
    if (DATA_RDATA = "00000000") then
     pc_plus <= '1';
     nasledujici_stav <= S3;
    else
     cnt_plus <= '1';
     CODE_EN <= '1';
     pc_minus <= '1';
     nasledujici_stav <= S32;
    end if;


   when S32 =>
    CODE_EN <= '1';
    nasledujici_stav <= S33;


   when S33 =>
    CODE_EN <= '1';
    case (aktivni_instrukce) is
     when I_WHILE_START =>
      cnt_minus <= '1';
     when I_WHILE_END =>
      cnt_plus <= '1';
     when others => null;
    end case;
    nasledujici_stav <= S34;


   when S34 =>
    CODE_EN <= '1';
    nasledujici_stav <= S35;


   when S35 =>
    CODE_EN <= '1';
    if (registr_cnt = "00000000") then
     pc_plus <= '1';
     nasledujici_stav <= S3;
    else
     pc_minus <= '1';
     nasledujici_stav <= S32;
    end if;

   when S27 =>
    nasledujici_stav <= S27;

   when S28 =>
    pc_plus <= '1';
    nasledujici_stav <= S3;


   when S29 =>
    cti_tmp <= '1';
    nasledujici_stav <= S30;


   when S30 =>
    pc_plus <= '1';
    nasledujici_stav <= S3;

   when others => null;

  end case;
 end process vykonani_stavu;

 -- prirazeni aktualnich dat na vystupy
 DATA_ADDR <= registr_ptr;
 CODE_ADDR <= registr_pc;

end behavioral;
-- konec architektury

