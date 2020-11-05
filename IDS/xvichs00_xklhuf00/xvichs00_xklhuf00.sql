-- xklhuf00, MICHAL KLHUFEK 
-- xvichs00, STEPAN VICH
-- ----------------------------------------------------------------------- 
--                    VYCISTENI OD PUVODNICH POLOZEK   
-- -----------------------------------------------------------------------  
DROP TABLE bankovni_ucet CASCADE CONSTRAINTS; 

DROP TABLE sporici CASCADE CONSTRAINTS; 

DROP TABLE bezny CASCADE CONSTRAINTS; 

DROP TABLE uverovy CASCADE CONSTRAINTS; 

DROP TABLE klient CASCADE CONSTRAINTS; 

DROP TABLE pracovnik_banky CASCADE CONSTRAINTS; 

DROP TABLE disponuje CASCADE CONSTRAINTS; 

DROP TABLE operace CASCADE CONSTRAINTS; 

DROP SEQUENCE pocitadlo_uctu;

-- -----------------------------------------------------------------a------ 
--                    SESTAVENI NOVYCH TABULEK SQL DATABAZE   
-- ----------------------------------------------------------------------- 
CREATE TABLE bankovni_ucet 
  ( 
     c_uctu        NUMBER(9) NOT NULL PRIMARY KEY, 
     stav_uctu     NUMBER DEFAULT 0, 
     datum_zrizeni DATE NOT NULL 
  ); 

CREATE TABLE pracovnik_banky 
  ( 
     id_pracovnik NUMBER(10) PRIMARY KEY, 
     r_cislo      NUMBER(10) NOT NULL UNIQUE, 
     jmeno        VARCHAR(12) NOT NULL, 
     prijmeni     VARCHAR(18) NOT NULL, 
     adresa       VARCHAR(60), 
     telefon      VARCHAR(17), 
     email        VARCHAR(35) 
  ); 

CREATE TABLE klient 
  ( 
     cislo_op VARCHAR(36) PRIMARY KEY, 
     r_cislo  NUMBER(10) NOT NULL UNIQUE, 
     jmeno    VARCHAR(12) NOT NULL, 
     prijmeni VARCHAR(15) NOT NULL, 
     adresa   VARCHAR(60), 
     telefon  VARCHAR(17), 
     email    VARCHAR(35) 
  ); 

CREATE TABLE operace 
  ( 
     c_operace   NUMBER(16) NOT NULL, 
     typ_operace VARCHAR(20) NOT NULL, 
     kod_banky   NUMBER(4) NOT NULL, 
     var_symbol  NUMBER(4), 
     castka      NUMBER, 
     datum       DATE NOT NULL, 
     zadal       VARCHAR(36), 
     provadi     NUMBER(10) 
  ); 

CREATE TABLE disponuje 
  ( 
     disponujici VARCHAR(36),
     vlastni     NUMBER(9),
     limit       NUMBER 
  ); 

CREATE TABLE sporici 
  ( 
     c_uctu NUMBER(9) PRIMARY KEY REFERENCES bankovni_ucet(c_uctu), 
     urok   VARCHAR(5) 
  ); 

CREATE TABLE bezny 
  ( 
     c_uctu  NUMBER(9) PRIMARY KEY REFERENCES bankovni_ucet(c_uctu), 
     d_limit NUMBER 
  ); 

CREATE TABLE uverovy 
  ( 
     c_uctu    NUMBER(9) PRIMARY KEY REFERENCES bankovni_ucet(c_uctu), 
     typ_uveru VARCHAR(25), 
     castka    NUMBER, 
     urok      VARCHAR(5) 
  ); 

-- Pocitadlo pro vlastni counter
CREATE SEQUENCE pocitadlo_uctu START WITH 10 INCREMENT BY 1 NOMAXVALUE;

-- -----------------------------------------------------------------------      
--                      PRIDANI CIZICH KLICU DO TABULEK        
-- -----------------------------------------------------------------------      
 
ALTER TABLE disponuje 
  ADD CONSTRAINT fk_klient1 FOREIGN KEY (disponujici) REFERENCES klient ON 
  DELETE CASCADE; 

ALTER TABLE disponuje 
  ADD CONSTRAINT fk_uctu FOREIGN KEY (vlastni) REFERENCES bankovni_ucet ON 
  DELETE CASCADE; 

ALTER TABLE operace 
  ADD CONSTRAINT fk_klient2 FOREIGN KEY (zadal) REFERENCES klient ON DELETE 
  CASCADE; 

ALTER TABLE operace 
  ADD CONSTRAINT fk_pracovnik FOREIGN KEY (provadi) REFERENCES pracovnik_banky 
  ON DELETE CASCADE; 

-- -----------------------------------------------------------------------      
--                      OSETRENI RODNNEHO CISLA U KLIENTA        
-- -----------------------------------------------------------------------
 
ALTER SESSION SET nls_date_format='dd.mm.yy'; 

  CREATE OR replace TRIGGER rcisla_klient 
    BEFORE INSERT OR UPDATE OF r_cislo ON klient 
  FOR EACH ROW 
DECLARE 
    r_cislo     NUMBER(10); 
    den         NUMBER(2); 
    mesic       NUMBER(2); 
    rok         NUMBER(2); 
    datum       DATE; 
  
  BEGIN 
    r_cislo := :NEW.r_cislo; -- nove rodne cislo klienta 

    mesic := MOD(( r_cislo / 1000000 ), 100); --vypocteni mesice z rodneho cisla

    den := MOD(( r_cislo / 10000 ), 100); --vypocteni dnu z rodneho cisla  

    rok := r_cislo / 100000000; --vypocteni roku z rodneho cisla 
    
    IF ( MOD ( r_cislo , 11 ) <> 0 ) THEN -- test dìlitelnosti 11 
      Raise_application_error (-20011, 'ERROR - RODNE CISLO NENI DELITELNE 11' 
      ); 
  END IF; 
 
    IF ( mesic > 50 ) THEN   -- zjisti, zda se jedna o muze nebo zenu
      mesic := mesic - 50; 
    END IF; 

    BEGIN -- test platneho datumu 
        datum := den 
                 ||'.' 
                 ||mesic 
                 ||'.' 
                 ||rok; 
    EXCEPTION -- vyjimka
        WHEN OTHERS THEN 
          Raise_application_error (-20012, 'ERROR - SPATNE DATUM'); 
    END; 
END rcisla_klient; 
/
ALTER SESSION SET nls_date_format='dd.mm.yyyy'; 

-- -----------------------------------------------------------------------      
--                OSETRENI RODNNEHO CISLA U PRACOVNIKA BANKY        
-- -----------------------------------------------------------------------
 
ALTER SESSION SET nls_date_format='dd.mm.yy'; 

CREATE OR replace TRIGGER rcisla_pracovnik 
  BEFORE INSERT OR UPDATE OF r_cislo ON pracovnik_banky 
  FOR EACH ROW 
DECLARE 
    r_cislo NUMBER(10); 
    den         NUMBER(2); 
    mesic       NUMBER(2); 
    rok         NUMBER(2); 
    datum       DATE; 
BEGIN 
    r_cislo := :NEW.r_cislo; -- ulozeni rodneho cisla klienta do promenne 

    mesic := MOD(( r_cislo / 1000000 ), 100); -- zjistime mesic 

    den := MOD(( r_cislo / 10000 ), 100); -- den 

    rok := r_cislo / 100000000; -- rok 
    -- rodnne cislo musi byt delitelne 11 
    IF ( MOD(r_cislo, 11) <> 0 ) THEN  -- test dìlitelnosti 11 
        Raise_application_error (-20011, 'ERROR - RODNE CISLO NENI DELITELNE 11' 
      ); 
    END IF; 
  
    IF (mesic > 50) THEN -- zjisti, zda se jedna o muze nebo zenu  
      mesic := mesic - 50; 
    END IF; 

    
    BEGIN -- test platneho datumu 
        datum := den 
                 ||'.' 
                 ||mesic 
                 ||'.' 
                 ||rok; 
    EXCEPTION -- vyjimka
        WHEN OTHERS THEN 
          Raise_application_error (-20012, 'ERROR - SPATNE DATUM'); 
    END; 
END rcisla_pracovnik; 
/
ALTER SESSION SET nls_date_format='dd.mm.yyyy'; 



-- -----------------------------------------------------------------------      
--                VLOZENI DAT DO JEDNOTLIVYCH TABULEK        
-- -----------------------------------------------------------------------

INSERT INTO klient 
VALUES      ('2523590254CZE3338805M02502507991<<<1', 
             9603125917, 
             'Tomas', 
             'Vich', 
             'U Dvora 6, Hlucin, 74801', 
             '+420732892012', 
             'tom.vich@google.com' ); 

INSERT INTO klient 
VALUES      ('4741877452CZE4426316M62042443149<<<2', 
             9603125928, 
             'Viktor', 
             'Koncecny', 
             'Severni 2, Cheb, 35002', 
             '+420732904231', 
             'viktorkonecny@seznam.cz' ); 

INSERT INTO klient 
VALUES      ('5243178069CZE4390176M98085153655<<<1', 
             9603125939, 
             'Irena', 
             'Novakova', 
             'Pasteurova 6, Olomouc, 77900', 
             '+420743852013', 
             'novakova.irena@stream.cz' ); 

INSERT INTO klient 
VALUES      ('0926764459CZE2370056M07907911161<<<2', 
             9603125950, 
             'Anna', 
             'Hanzlikova', 
             'Stodolni 6, Ostrava, 72200', 
             '+420764327729', 
             'anna.hanz@aktualne.cz' ); 

INSERT INTO klient 
VALUES      ('0069157745CZE6495001M06499849537<<<1', 
             9603125961, 
             'Lukas', 
             'Konecny', 
             'Opavska 171, Velka Polom, 74764', 
             '+420766892013', 
             'anna.hanz@aktualne.cz' ); 

------ BANKOVNI UCTY ------  
INSERT INTO bankovni_ucet 
VALUES      (234729834, 
             50000, 
             To_date('12-09-2017', 'DD-MM-YYYY')); 

INSERT INTO bankovni_ucet 
VALUES      (286900237, 
             840350, 
             To_date('06-06-2004', 'DD-MM-YYYY')); 

INSERT INTO bankovni_ucet 
VALUES      (468745903, 
             34322, 
             To_date('23-10-2014', 'DD-MM-YYYY')); 

INSERT INTO bankovni_ucet 
VALUES      (148348875, 
             34903, 
             To_date('25-12-2016', 'DD-MM-YYYY')); 

INSERT INTO bankovni_ucet 
VALUES      (398564230, 
             250000, 
             To_date('25-12-2016', 'DD-MM-YYYY')); 

INSERT INTO bankovni_ucet 
VALUES      (895672310, 
             400500, 
             To_date('25-12-2016', 'DD-MM-YYYY')); 

INSERT INTO bankovni_ucet 
VALUES      (890345128, 
             41200, 
             To_date('25-12-2016', 'DD-MM-YYYY')); 

------ HODNOTY ------  
INSERT INTO disponuje 
VALUES      ('2523590254CZE3338805M02502507991<<<1', 
             234729834, 
             200000); 

INSERT INTO disponuje 
VALUES      ('4741877452CZE4426316M62042443149<<<2', 
             286900237, 
             300000); 

INSERT INTO disponuje 
VALUES      ('5243178069CZE4390176M98085153655<<<1', 
             468745903, 
             300000); 

INSERT INTO disponuje 
VALUES      ('0926764459CZE2370056M07907911161<<<2', 
             148348875, 
             30000); 

INSERT INTO disponuje 
VALUES      ('0069157745CZE6495001M06499849537<<<1', 
             398564230, 
             5000); 

INSERT INTO disponuje 
VALUES      ('5243178069CZE4390176M98085153655<<<1', 
             895672310, 
             5000); 

INSERT INTO disponuje 
VALUES      ('0926764459CZE2370056M07907911161<<<2', 
             890345128, 
             60000); 

------ PRACOVNICI BANKY ------  
INSERT INTO pracovnik_banky 
VALUES      (8609606469, 
             9307114476, 
             'Tomas', 
             'Jirasek', 
             'U Dubu 74, Markvartovice', 
             '+420745775657', 
             'tom.jirasek@airbank.cz' ); 

INSERT INTO pracovnik_banky 
VALUES      (2996196621, 
             9307114487, 
             'Jan', 
             'Novak', 
             'Horni 1, Ludgerovice', 
             '+420785456788', 
             'jan.novak@airbank.cz' ); 

INSERT INTO pracovnik_banky 
VALUES      (8300156735, 
             9307114498, 
             'Lucie', 
             'Houskova', 
             'Vitezna 34, Praha', 
             '+420775457342', 
             'lucie.houskova@airbank.cz' ); 

INSERT INTO pracovnik_banky 
VALUES      (2596012842, 
             9307114509, 
             'Petr', 
             'Rychly', 
             'Horni 3, Ludgerovice', 
             '+420739369741', 
             'petr.rychly@airbank.cz' ); 

INSERT INTO pracovnik_banky 
VALUES      (4781673107, 
             9307114520, 
             'Stepan', 
             'Vranesic', 
             'U Dubu 75, Markvartovice', 
             '+420745567123', 
             'stepan.vranesic@airbank.cz' ); 

------ OPERACE ------  
INSERT INTO operace 
VALUES      (4590558839381184, 
             'Vyber penez z uctu', 
             0500, 
             1555, 
             80000, 
             To_date('26-04-2016', 'DD-MM-YYYY'), 
             '5243178069CZE4390176M98085153655<<<1', 
             4781673107); 

INSERT INTO operace 
VALUES      (9290832837593667, 
             'Vklad penez na ucet', 
             0400, 
             3450, 
             25000, 
             To_date('11-05-2017', 'DD-MM-YYYY'), 
             '0926764459CZE2370056M07907911161<<<2', 
             4781673107); 

INSERT INTO operace 
VALUES      (6109136097613474, 
             'Vyber penez z uctu', 
             1200, 
             1334, 
             4200, 
             To_date('17-08-2010', 'DD-MM-YYYY'), 
             '0926764459CZE2370056M07907911161<<<2', 
             8300156735); 

INSERT INTO operace 
VALUES      (1381000959235647, 
             'Vklad penez na ucet', 
             0200, 
             2556, 
             25000, 
             To_date('10-06-2009', 'DD-MM-YYYY'), 
             '0069157745CZE6495001M06499849537<<<1', 
             8300156735); 

------ UROKY & UVERY ------                    
INSERT INTO uverovy 
VALUES      (234729834, 
             'Akceptacni uver', 
             50000, 
             '5%'); 

INSERT INTO sporici 
VALUES      (468745903, 
             '1.2%'); 

INSERT INTO bezny 
VALUES      (398564230, 
             20000); 

INSERT INTO bezny 
VALUES      (890345128, 
             25000); 

-- SELECT

-- Spojeni dvou tabulek - dva dotazy
-- Vypsani uverovych uctu
SELECT bankovni_ucet.c_uctu, bankovni_ucet.datum_zrizeni, uverovy.urok
FROM   bankovni_ucet
INNER JOIN uverovy ON bankovni_ucet.c_uctu=uverovy.c_uctu;

-- Vypsani sporicich uctu
SELECT bankovni_ucet.c_uctu, bankovni_ucet.datum_zrizeni, sporici.urok
FROM   bankovni_ucet
INNER JOIN sporici ON bankovni_ucet.c_uctu=sporici.c_uctu;

-- Spojeni tri tabulek - jeden dotaz 
-- Spojeni klienta se svymi ucty a vypis zakladnich informaci o ucte
SELECT klient.jmeno, klient.prijmeni, klient.telefon, klient.r_cislo, bankovni_ucet.c_uctu, disponuje.limit
FROM klient
INNER JOIN disponuje ON disponuje.disponujici = klient.cislo_op
INNER JOIN bankovni_ucet ON bankovni_ucet.c_uctu = disponuje.vlastni;

-- Sjednoti operace ktere provedli jednotlivi pracovnici banky
SELECT pracovnik_banky.jmeno, pracovnik_banky.prijmeni, operace.provadi AS "ID", COUNT(operace.c_operace) AS "Pocet provedenych operaci"
FROM operace
LEFT JOIN pracovnik_banky ON pracovnik_banky.id_pracovnik = operace.provadi 
GROUP BY pracovnik_banky.jmeno, pracovnik_banky.prijmeni, operace.provadi;

-- Vypise informaci o poctu otevrenych uctu v bance u kazdeho klienta
SELECT klient.jmeno, klient.prijmeni, COUNT(disponuje.vlastni) as "Pocet otevrenych uctu"
FROM klient
INNER JOIN disponuje ON disponuje.disponujici = klient.cislo_op
GROUP BY klient.jmeno, klient.prijmeni;

-- Dotaz s predikatem EXISTS
-- Dotaz vypise vsechny klienty kteri maji vice nez jeden ucet
SELECT kl.jmeno, kl.prijmeni, kl.telefon, di.vlastni
FROM klient kl, disponuje di
WHERE kl.cislo_op = di.disponujici AND EXISTS(
    SELECT * FROM disponuje dis
    WHERE kl.cislo_op = dis.disponujici AND dis.vlastni <> di.vlastni
)
ORDER BY prijmeni, jmeno;

-- dotaz s IN s vnorenym SELECTEM
-- vypise jen sporici a uverove ucty
SELECT bankovni_ucet.c_uctu
FROM bankovni_ucet 
WHERE bankovni_ucet.c_uctu IN (SELECT c_uctu FROM sporici) OR bankovni_ucet.c_uctu IN (SELECT c_uctu FROM uverovy);


-- Triger pro navysovani cisla uctu
CREATE OR REPLACE TRIGGER ucet_counter
BEFORE INSERT ON bankovni_ucet
FOR EACH ROW
WHEN (new.c_uctu IS NULL)
BEGIN
  :new.c_uctu := pocitadlo_uctu.NEXTVAL;
END;
/

-- Predvedeni trigeru
INSERT INTO bankovni_ucet VALUES(null, 50000, To_date('12-09-2017', 'DD-MM-YYYY'));


-- Triger pro overeni rodneho cisla klienta
CREATE OR REPLACE TRIGGER rodne_cislo_kontrola_klient
  BEFORE INSERT ON klient
  FOR EACH ROW
DECLARE
  rod_c klient.r_cislo%TYPE;
  den NUMBER(2);
  mesic NUMBER(2);
  rok NUMBER(2);
BEGIN
  rod_c := :NEW.r_cislo; 
  mesic := MOD( (rod_c / 1000000), 100);
  den   := MOD( (rod_c / 10000), 100);
  rok   := rod_c / 100000000;

  IF (LENGTH(rod_c) < 9) OR (LENGTH(rod_c) > 10) THEN
    Raise_Application_Error (-20010, 'Nespravna delka rodneho cisla');
  END IF;
  
  IF ( mesic > 50 ) THEN
    mesic := mesic - 50;
  END IF;
  
  -- kontrola toho, zda mesic je v rozsahu 1-12
  IF ( mesic > 12 ) OR ( mesic = 0) THEN
    Raise_Application_Error (-20012, 'Mesic ve spatnem rozsahu');
  END IF;
  
  -- kontrola toho, zda den je v rozsahu 1-31
  IF ( den > 31 ) OR ( den = 0) THEN
    Raise_Application_Error (-20013, 'Den ve spatnem rozsahu');
  END IF;
  -- pokud se nejedna o platne datum, pak vznikne vyjimka
  
    IF ( MOD(rod_c, 11) <> 0 ) THEN
    Raise_Application_Error (-20014, 'Rodne cislo neni delitelne 11');
  END IF;

END rodne_cislo_kontrola_klient;
/

-- Predvedeni trigeru
INSERT INTO klient 
VALUES      ('5245178069CZE4390176M98085153655<<<1', 
             9603126071, 
             'Irena', 
             'Novakova', 
             'Pasteurova 6, Olomouc, 77900', 
             '+420743852013', 
             'novakova.irena@stream.cz' ); 


-- Procedury
-- Procedura ktera prijme
DROP PROCEDURE PRIDEJ_PRACOVNIKA;

CREATE PROCEDURE PRIDEJ_PRACOVNIKA (
    ID_PRAC VARCHAR,
    R_CISLO NUMBER,
    JMENO VARCHAR,
    PRIJMENI VARCHAR,
    ADRESA VARCHAR,
    TELEFON VARCHAR,
    EMAIL VARCHAR
) IS 
    CURSOR cursor_pracovnikID IS   
        SELECT ID_PRACOVNIK FROM PRACOVNIK_BANKY WHERE ID_PRAC = ID_PRACOVNIK;

    pracovnik32 pracovnik_banky.id_pracovnik%TYPE;

BEGIN
    OPEN cursor_pracovnikID;

    FETCH cursor_pracovnikID INTO pracovnik32;
    IF (cursor_pracovnikID%NOTFOUND) THEN
        INSERT INTO PRACOVNIK_BANKY VALUES (ID_PRAC,R_CISLO,JMENO,PRIJMENI,ADRESA,TELEFON,EMAIL);
    ELSE
        Raise_Application_Error(-20000, 'Tento pracovnik jiz existuje.');
    END IF;

    CLOSE cursor_pracovnikID;
END;
/
--SELECT * FROM KLIENT;
EXEC PRIDEJ_PRACOVNIKA(1234, 9603125917,'Lukas','Nemona','Ulice 8', '+420723456125','lukasnemona@seznam.cz');
SELECT * FROM PRACOVNIK_BANKY;

-- Procedura, ktera smaze pracovnika banky se zadanym ID
DROP PROCEDURE ODSTRAN_PRACOVNIKA;

SET serveroutput ON;

-- PROCEDURA, KTERA SMAZE PRACOVNIKA BANKY S DANYM ID
CREATE PROCEDURE ODSTRAN_PRACOVNIKA(id VARCHAR) IS
  CURSOR cursor_pracovnik IS 
    SELECT ID_PRACOVNIK 
    FROM PRACOVNIK_BANKY 
    WHERE ID_PRACOVNIK = id;  

  id_prac PRACOVNIK_BANKY.ID_PRACOVNIK%TYPE;
  
BEGIN
  
 OPEN cursor_pracovnik;  -- otevreni kurzoru
  LOOP
    FETCH cursor_pracovnik INTO id_prac;    -- nalezeny vysledek v promenne id_prac
    EXIT WHEN cursor_pracovnik%NOTFOUND;    
    DELETE FROM Pracovnik_Banky WHERE ID_PRACOVNIK = id_prac;  -- Smazani dat v databazi
  END LOOP;
  CLOSE cursor_pracovnik;   -- uzavreni kurzoru
END;
/

SELECT * FROM PRACOVNIK_BANKY;    -- vypsani vsechn pracovniku banky

EXECUTE ODSTRAN_PRACOVNIKA(2996196621);

SELECT * FROM PRACOVNIK_BANKY;    -- ukazka smazani radku

--  Vytvoreni zakladnich indexu pro zrychleni dotazovani s pouzitim EXPLAIN PLAN
DROP INDEX index1;
DROP INDEX index2;

EXPLAIN PLAN FOR
SELECT klient.jmeno, klient.prijmeni, COUNT(disponuje.vlastni) as "Pocet otevrenych uctu"
FROM klient
INNER JOIN disponuje ON disponuje.disponujici = klient.cislo_op
GROUP BY klient.jmeno, klient.prijmeni;

SELECT plan_table_output
  from table(dbms_xplan.display());


CREATE INDEX index1 ON klient(cislo_op, jmeno, prijmeni);
CREATE INDEX index2 ON disponuje(disponujici, vlastni);

EXPLAIN PLAN FOR
SELECT klient.jmeno, klient.prijmeni, COUNT(disponuje.vlastni) as "Pocet otevrenych uctu"
FROM klient
INNER JOIN disponuje ON disponuje.disponujici = klient.cislo_op
GROUP BY klient.jmeno, klient.prijmeni;

SELECT plan_table_output
  from table(dbms_xplan.display());




--- Ziskani pristupovych prav pro dalsiho uzivatele
GRANT ALL ON BANKOVNI_UCET TO xklhuf00;
GRANT ALL ON BEZNY TO xklhuf00;
GRANT ALL ON SPORICI TO xklhuf00;
GRANT ALL ON UVEROVY TO xklhuf00;
GRANT ALL ON DISPONUJE TO xklhuf00;
GRANT ALL ON KLIENT TO xklhuf00;
GRANT ALL ON OPERACE TO xklhuf00;
GRANT ALL ON PRACOVNIK_BANKY TO xklhuf00;

-- Tvorba materialized pohledu
DROP MATERIALIZED VIEW Klienti;

CREATE MATERIALIZED VIEW Klienti

AS 
  SELECT *
  FROM Bankovni_ucet;
  
  SELECT * FROM Klienti;
  
GRANT ALL ON Klienti TO xklhuf00;

SELECT * FROM Klienti;

INSERT INTO klient(cislo_op, r_cislo, jmeno, prijmeni) 
VALUES      ('0069157745CZE6495011M06499849537<<<1', 
             9603126104, 
             'Lukas', 
             'Konecny'); 
             
SELECT * FROM Klienti;



COMMIT; 