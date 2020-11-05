SET NAMES utf8;
SET foreign_key_checks = 0;
SET time_zone = 'SYSTEM';
SET sql_mode = 'NO_AUTO_VALUE_ON_ZERO';

-- -----------------------------------------------------------------------------------
--
-- DROP TABLE (if exists)
--
-- -----------------------------------------------------------------------------------

DROP TABLE IF EXISTS bankovni_ucet; 
DROP TABLE IF EXISTS sporici;
DROP TABLE IF EXISTS bezny;
DROP TABLE IF EXISTS uverovy;
DROP TABLE IF EXISTS klient; 
DROP TABLE IF EXISTS pracovnik_banky;
DROP TABLE IF EXISTS disponuje;
DROP TABLE IF EXISTS operace;

-- -----------------------------------------------------------------------------------
--
-- CREATE TABLES
--
-- -----------------------------------------------------------------------------------

CREATE TABLE bankovni_ucet 
  ( 
     c_uctu        bigint(15) NOT NULL PRIMARY KEY, 
     stav_uctu     bigint(15) DEFAULT 0, 
     datum_zrizeni DATE NOT NULL 
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci; 

CREATE TABLE pracovnik_banky 
  ( 
     id_pracovnik bigint(15) PRIMARY KEY, 
     r_cislo      bigint(15) NOT NULL UNIQUE, 
     jmeno        VARCHAR(12) NOT NULL, 
     prijmeni     VARCHAR(18) NOT NULL, 
     adresa       VARCHAR(60), 
     telefon      VARCHAR(17), 
     email        VARCHAR(35),
     login        VARCHAR(15) NOT NULL UNIQUE,
     heslo        VARCHAR(15) NOT NULL,
     pozicia	  int
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci; 

CREATE TABLE klient 
  ( 
     cislo_op VARCHAR(36) PRIMARY KEY, 
     r_cislo  bigint(15) NOT NULL UNIQUE, 
     jmeno    VARCHAR(12) NOT NULL, 
     prijmeni VARCHAR(15) NOT NULL, 
     adresa   VARCHAR(60), 
     telefon  VARCHAR(17), 
     email    VARCHAR(35)
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci; 

CREATE TABLE operace 
  ( 
     c_operace   int(16) NOT NULL, 
     typ_operace VARCHAR(20) NOT NULL, 
     kod_banky   int(4) NOT NULL, 
     var_symbol  int(4), 
     castka      int, 
     datum       DATE NOT NULL, 
     zadal       VARCHAR(36), 
     provadi     bigint(15)
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci; 

CREATE TABLE disponuje 
  ( 
     disponujici VARCHAR(36),
     vlastni     bigint(15),
     limit_uctu  int 
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci; 

CREATE TABLE sporici 
  (
     c_uctu bigint(15) PRIMARY KEY REFERENCES bankovni_ucet(c_uctu), 
     urok   VARCHAR(5) 
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci; 

CREATE TABLE bezny 
  ( 
     c_uctu  bigint(15) PRIMARY KEY REFERENCES bankovni_ucet(c_uctu), 
     d_limit int 
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci; 

CREATE TABLE uverovy 
  ( 
     c_uctu    bigint(15) PRIMARY KEY REFERENCES bankovni_ucet(c_uctu), 
     typ_uveru VARCHAR(25), 
     castka    int, 
     urok      VARCHAR(5) 
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci; 

-- -----------------------------------------------------------------------------------
--
-- FOREIGN KEYS
--
-- -----------------------------------------------------------------------------------



-- -----------------------------------------------------------------------------------
--
-- INSERT VALUES INTO TABLES
--
-- -----------------------------------------------------------------------------------

-- PRACOVNICI BANKY
INSERT INTO pracovnik_banky 
VALUES      (8609606469, 
             9307114476, 
             'Tomas', 
             'Jirasek', 
             'U Dubu 74, Markvartovice', 
             '+420745775657', 
             'tom.jirasek@airbank.cz' ,
              'xjirasek01',
              '1234',
              0); 

INSERT INTO pracovnik_banky 
VALUES      (2996196621, 
             9307114487, 
             'Jan', 
             'Novak', 
             'Horni 1, Ludgerovice', 
             '+420785456788', 
             'jan.novak@airbank.cz',
              'xnovak03',
              '1234',
              0); 

INSERT INTO pracovnik_banky 
VALUES      (8300156735, 
             9307114498, 
             'Lucie', 
             'Houskova', 
             'Vitezna 34, Praha', 
             '+420775457342', 
             'lucie.houskova@airbank.cz' ,
              'xholus03',
              '1234',
              0); 

INSERT INTO pracovnik_banky 
VALUES      (2596012842, 
             9307114509, 
             'Petr', 
             'Rychly', 
             'Horni 3, Ludgerovice', 
             '+420739369741', 
             'petr.rychly@airbank.cz' ,
              'xrychly',
              '1234',
              0); 

INSERT INTO pracovnik_banky 
VALUES      (4781673107, 
             9307114520, 
             'Stepan', 
             'Vranesic', 
             'U Dubu 75, Markvartovice', 
             '+420745567123', 
             'stepan.vranesic@airbank.cz' ,
              'xvranesic1',
              '1234',
              1); 

-- KLIENTI
INSERT INTO klient 
VALUES      ('2523590254CZE3338805M02502507991<<<1', 
             9603125917, 
             'Tomas', 
             'Vich', 
             'U Dvora 6, Hlucin, 74801', 
             '+420732892012', 
             'tom.vich@google.com'
             ); 

INSERT INTO klient 
VALUES      ('4741877452CZE4426316M62042443149<<<2', 
             9603125928, 
             'Viktor', 
             'Koncecny', 
             'Severni 2, Cheb, 35002', 
             '+420732904231', 
             'viktorkonecny@seznam.cz' 
             ); 

INSERT INTO klient 
VALUES      ('5243178069CZE4390176M98085153655<<<1', 
             9603125939, 
             'Irena', 
             'Novakova', 
             'Pasteurova 6, Olomouc, 77900', 
             '+420743852013', 
             'novakova.irena@stream.cz' 
             ); 

INSERT INTO klient 
VALUES      ('0926764459CZE2370056M07907911161<<<2', 
             9603125950, 
             'Anna', 
             'Hanzlikova', 
             'Stodolni 6, Ostrava, 72200', 
             '+420764327729', 
             'anna.hanz@aktualne.cz' 
             ); 

INSERT INTO klient 
VALUES      ('0069157745CZE6495001M06499849537<<<1', 
             9603125961, 
             'Lukas', 
             'Konecny', 
             'Opavska 171, Velka Polom, 74764', 
             '+420766892013', 
             'anna.hanz@aktualne.cz' 
             ); 

-- BANKOVE UCTY
INSERT INTO bankovni_ucet 
VALUES 		(234729834, 
			 50000, 
			 STR_TO_DATE('1-01-2012', '%d-%m-%Y'));

INSERT INTO bankovni_ucet 
VALUES      (286900237, 
             840350, 
             STR_TO_DATE('1-01-2012', '%d-%m-%Y')); 

INSERT INTO bankovni_ucet 
VALUES      (468745903, 
             34322, 
             STR_TO_DATE('1-01-2012', '%d-%m-%Y')); 

INSERT INTO bankovni_ucet 
VALUES      (148348875, 
             34903, 
             STR_TO_DATE('1-01-2012', '%d-%m-%Y')); 

INSERT INTO bankovni_ucet 
VALUES      (398564230, 
             250000, 
             STR_TO_DATE('1-01-2012', '%d-%m-%Y')); 

INSERT INTO bankovni_ucet 
VALUES      (895672310, 
             400500, 
             STR_TO_DATE('1-01-2012', '%d-%m-%Y')); 

INSERT INTO bankovni_ucet 
VALUES      (890345128, 
             41200, 
             STR_TO_DATE('1-01-2012', '%d-%m-%Y')); 

-- HODNOTY
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

-- OPERACIE
INSERT INTO operace 
VALUES      (4590558839381184, 
             'Vyber penez z uctu', 
             0500, 
             1555, 
             80000, 
             STR_TO_DATE('1-01-2012', '%d-%m-%Y'), 
             '5243178069CZE4390176M98085153655<<<1', 
             4781673107); 

INSERT INTO operace 
VALUES      (9290832837593667, 
             'Vklad penez na ucet', 
             0400, 
             3450, 
             25000, 
             STR_TO_DATE('1-01-2012', '%d-%m-%Y'), 
             '0926764459CZE2370056M07907911161<<<2', 
             4781673107); 

INSERT INTO operace 
VALUES      (6109136097613474, 
             'Vyber penez z uctu', 
             1200, 
             1334, 
             4200, 
             STR_TO_DATE('1-01-2012', '%d-%m-%Y'), 
             '0926764459CZE2370056M07907911161<<<2', 
             8300156735); 

INSERT INTO operace 
VALUES      (1381000959235647, 
             'Vklad penez na ucet', 
             0200, 
             2556, 
             25000, 
             STR_TO_DATE('1-01-2012', '%d-%m-%Y'), 
             '0069157745CZE6495001M06499849537<<<1', 
             8300156735); 

-- UROKY A UVERY
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
