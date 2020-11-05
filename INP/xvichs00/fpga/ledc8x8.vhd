-- Vypracoval: Stepan Vich, xvichs00, inicialy: SV

LIBRARY IEEE;
USE IEEE.std_logic_1164.ALL;
USE IEEE.std_logic_arith.ALL;
USE IEEE.std_logic_unsigned.ALL;

ENTITY ledc8x8 IS
	PORT 
	(-- Sem doplnte popis rozhrani obvodu.

		ROW : OUT std_logic_vector(7 DOWNTO 0 );
		LED : OUT std_logic_vector(7 DOWNTO 0 );
		SMCLK : IN std_logic;
		RESET : IN std_logic

	);
END ledc8x8;

ARCHITECTURE main OF ledc8x8 IS
	-- pocitadlo hodin, vektor
	SIGNAL pocitadlo   : std_logic_vector(22 DOWNTO 0)  := (OTHERS => '0' );
	SIGNAL hodiny      : std_logic_vector( 7 DOWNTO 0 ) := (OTHERS => '0' );
	-- prepinac znaku
	SIGNAL mutex       : std_logic                      := '0';
	-- prepinac radku, jakmile nabude urcite hodnoty, je prepnut aktivni radek na displeji
	SIGNAL mutex_hodin : std_logic                      := '0';
	-- pole radku pro ledky, nakonec jej namapujeme na maticovy displej
	SIGNAL rows        : std_logic_vector( 7 DOWNTO 0 ) := (0 => '1', OTHERS => '0' );
	-- pole ledek v jednom radku
	SIGNAL leds        : std_logic_vector( 7 DOWNTO 0 ) := (OTHERS => '1' );
 
BEGIN
	-- Sem doplnte popis funkce obvodu (zakladni konstrukce VHDL jako napr.
	-- prirazeni signalu, multiplexory, dekodery, procesy...).
	-- DODRZUJTE ZASADY PSANI SYNTETIZOVATELNEHO VHDL UVEDENE NA WEBU:
	-- http://merlin.fit.vutbr.cz/FITkit/docs/navody/synth_templates.html

 
pocitanihodin : PROCESS 
(RESET, SMCLK, hodiny, mutex_hodin )
-- Proces pro hodiny systemu
-- prepina mutex hodin a upravuje tak frekvenci s jakou se prepinaji diody
BEGIN
	IF RESET = '1' THEN -- vetev resetu
		hodiny <=(OTHERS => '0' );

	ELSIF SMCLK = '1' AND SMCLK'EVENT THEN
		-- Zvys pocitadlo hodin
		hodiny<= 1 + hodiny;
 
    -- Pokud hodiny jiz dovrsili
		IF hodiny( 7 DOWNTO 0 )  =  "11111111" THEN
			mutex_hodin<='1';

		ELSE
			mutex_hodin<='0' ;
		END IF;
	END IF;
END PROCESS pocitanihodin;

 
nastavradek : PROCESS 
(RESET, rows, SMCLK, mutex_hodin )
-- jakmile prijde hodinovy vstup a mutex hodin je true, posune aktivni radek
-- posun provadi pomoci konkatenace a binarniho operatoru AND
BEGIN
	IF RESET = '1' THEN -- vetev resetu
		rows <=( 0 =>  '1', OTHERS => '0' );

	ELSIF mutex_hodin  = '1' AND SMCLK'EVENT AND SMCLK =  '1' THEN
		-- provedeme konkatenaci pomoci binarniho operatoru AND
		rows <=rows( 6 DOWNTO 0  ) &  rows(7);
	END IF;
END PROCESS nastavradek;


prepnimutex : PROCESS 
(RESET, SMCLK, pocitadlo, mutex )
BEGIN
	IF  RESET =  '1' THEN -- vetev resetu
		pocitadlo <=(OTHERS => '0');
 
	ELSIF  SMCLK = '1' AND SMCLK'EVENT THEN
		-- Inkrementuj pocitadlo
		pocitadlo <= 1 + pocitadlo;
		IF ( pocitadlo(22 DOWNTO 0 )  =  "11111111111111111111111" ) THEN
			-- Pokud se pocitadlo hodin, dostalo do tohoto stavu tak prepni row
			IF  mutex  = '1'  THEN
				-- Mutex se z jedna, zmeni na nula
				mutex<= '0';
			ELSE   -- mutex = 1
				-- mutex se z nula, prepne na 1
				mutex<= '1' ;
			END IF ;
		END IF;
	END IF ;
END PROCESS prepnimutex;
 
rozvitdiody : PROCESS 
(mutex, rows, leds )
-- Dekoder
-- Mapuje jednotlive stavy, na jednotlive radky diod
-- Vola se jakmile, se zmeni rows, leds, nebo mutex znaku
-- O zobrazovanem znaku rozhoduje mutex
BEGIN
	IF  mutex = '0'  THEN
  
		-- Zobraz prvni znak S
		CASE rows IS
			WHEN  "00000001" => leds <= "11100111";
			WHEN "00000010" =>  leds <= "11011011";
			WHEN  "00000100" => leds <= "11011111";
			WHEN "00001000" => leds  <= "11101111";
			WHEN "00010000" => leds  <= "11110111";
			WHEN "00100000" => leds  <= "11111011";
			WHEN "01000000" => leds  <= "11011011";
			WHEN "10000000" => leds  <= "11100111";
			WHEN OTHERS     => leds  <= "11111111";
		END CASE;
	
  ELSE
  
		CASE rows IS
			-- Zobraz druhy znak V
			WHEN "00000001" => leds  <= "01111110";
			WHEN "00000010" => leds  <= "01111110";
			WHEN "00000100" => leds  <= "10111101";
			WHEN "00001000" => leds  <= "10111101";
			WHEN "00010000" => leds  <= "11011011";
			WHEN "00100000" => leds  <= "11011011";
			WHEN "01000000" => leds  <= "11100111";
			WHEN "10000000" => leds  <= "11100111";
			WHEN OTHERS     => leds  <= "11111111";
		END CASE;
  
	END IF;     
END PROCESS rozvitdiody;
 

namapovat : PROCESS 
(rows, leds )
-- Proces, ktery mapuje jednotlive led diody
-- Musi probehnout pri kazde zmene rows a leds, jinak se zmena neprojevi na displeji
-- Je dulezite spravne zapsat ROW a LED v souboru .ucf
BEGIN

	ROW<=rows;
	LED<=leds;

END PROCESS namapovat;
 
-- Nezapomente take doplnit mapovani signalu rozhrani na piny FPGA
-- v souboru ledc8x8.ucf.
-- Splneno

END main;

-- Konec souboru

