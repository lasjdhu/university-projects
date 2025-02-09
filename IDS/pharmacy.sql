--Drop Foreign Keys
ALTER TABLE LekNaPredpis DROP CONSTRAINT FK_LekNaPredpis_Lek_Nazev;
ALTER TABLE Prispevek DROP CONSTRAINT FK_Prispevek_LekNaPredpis_Nazev;
ALTER TABLE Prispevek DROP CONSTRAINT FK_Prispevek_Pojistovna_ICO;
ALTER TABLE Vydani DROP CONSTRAINT FK_Vydani_Lek_Nazev;
ALTER TABLE Vykaz DROP CONSTRAINT FK_Vykaz_LekNaPredpis_Nazev;
ALTER TABLE Vykaz DROP CONSTRAINT FK_Vykaz_Pojistovna_ICO;
ALTER TABLE Vykaz DROP CONSTRAINT FK_Vykaz_Prispevek_ID;

--Drop Tables
DROP TABLE Lek;
DROP TABLE LekNaPredpis;
DROP TABLE Pojistovna;
DROP TABLE Prispevek;
DROP TABLE Vykaz;
DROP TABLE Vydani;

--Drop Sequences
DROP SEQUENCE seq_Pojistovna_ICO;
DROP SEQUENCE seq_Prispevek_ID;
DROP SEQUENCE seq_Vydani_ID;
DROP SEQUENCE seq_Vykaz_ID;

--Drop Trigger
DROP TRIGGER rand_lek_nazev;
DROP TRIGGER rand_nazev;
DROP TRIGGER update_dostupnost;
DROP TRIGGER check_valid_psc;

--Generalizace -> jedna tabulka pro entitu s vyssi prioritou a druha pro jeji subtyp
CREATE TABLE Lek (
  Nazev VARCHAR(255),
  dostupnost INTEGER,
  nutnostPredpisu INTEGER,
  cenaLeku INTEGER,
  mnozstvi INTEGER
);

-- Vytvoreni nahodneho Nazev jestli je NULL (DBMS_RANDOM.STRING Oracle function)
CREATE OR REPLACE TRIGGER rand_nazev
  BEFORE INSERT ON Lek
  FOR EACH ROW
BEGIN
  IF :NEW.Nazev IS NULL THEN 
    :NEW.Nazev := DBMS_RANDOM.STRING('X', 10);
  END IF;
END;
/

CREATE TABLE LekNaPredpis (
  Lek_Nazev VARCHAR(255),
  doporuceni VARCHAR(255)
);

-- Vytvoreni nahodneho Lek_Nazev jestli je NULL (DBMS_RANDOM.STRING Oracle function)
CREATE OR REPLACE TRIGGER rand_lek_nazev
  BEFORE INSERT ON LekNaPredpis
  FOR EACH ROW
BEGIN
  IF :NEW.Lek_Nazev IS NULL THEN 
    :NEW.Lek_Nazev := DBMS_RANDOM.STRING('X', 10);
  END IF;
END;
/

CREATE SEQUENCE seq_Pojistovna_ICO START WITH 1 INCREMENT BY 1;
CREATE TABLE Pojistovna (
  ICO INTEGER DEFAULT seq_Pojistovna_ICO.nextval NOT NULL,
  nazevPojistovny VARCHAR(255),
  --Transformace adresy:
  ulice VARCHAR(255),
  mesto VARCHAR(255),
  PSC INTEGER
);

CREATE SEQUENCE seq_Prispevek_ID START WITH 1 INCREMENT BY 1;
CREATE TABLE Prispevek (
  ID NUMBER DEFAULT seq_Prispevek_ID.nextval NOT NULL,
  Pojistovna_ICO INTEGER,
  LekNaPredpis_Nazev VARCHAR(255),
  castka INTEGER
);

CREATE SEQUENCE seq_Vydani_ID START WITH 1 INCREMENT BY 1;
CREATE TABLE Vydani (
  ID NUMBER DEFAULT seq_Vydani_ID.nextval NOT NULL,
  Lek_Nazev VARCHAR(255),
  mnozstviProdane INTEGER
);

CREATE SEQUENCE seq_Vykaz_ID START WITH 1 INCREMENT BY 1;
CREATE TABLE Vykaz (
  ID NUMBER DEFAULT seq_Vykaz_ID.nextval NOT NULL,
  Pojistovna_ICO INTEGER,
  LekNaPredpis_Nazev VARCHAR(255),
  Prispevek_ID INTEGER,
  jmenoZakaznika VARCHAR(255)
);

ALTER TABLE Lek ADD CONSTRAINT PK_Lek PRIMARY KEY (Nazev);

ALTER TABLE LekNaPredpis ADD CONSTRAINT PK_LekNaPredpis PRIMARY KEY (Lek_Nazev);
ALTER TABLE LekNaPredpis ADD CONSTRAINT FK_LekNaPredpis_Lek_Nazev FOREIGN KEY (Lek_Nazev) REFERENCES Lek;

ALTER TABLE Pojistovna ADD CONSTRAINT PK_Pojistovna PRIMARY KEY (ICO);
-- Overi jestli ICO je prave 8 symbolu
ALTER TABLE Pojistovna ADD CONSTRAINT CHECK_ICO_SYMB CHECK (REGEXP_LIKE(ICO, '^[0-9]{8}$'));

ALTER TABLE Prispevek ADD CONSTRAINT PK_Prispevek PRIMARY KEY (ID);
ALTER TABLE Prispevek ADD CONSTRAINT FK_Prispevek_Pojistovna_ICO FOREIGN KEY (Pojistovna_ICO) REFERENCES Pojistovna;
ALTER TABLE Prispevek ADD CONSTRAINT FK_Prispevek_LekNaPredpis_Nazev FOREIGN KEY (LekNaPredpis_Nazev) REFERENCES LekNaPredpis;

ALTER TABLE Vydani ADD CONSTRAINT PK_Vydani PRIMARY KEY (ID);
ALTER TABLE Vydani ADD CONSTRAINT FK_Vydani_Lek_Nazev FOREIGN KEY (Lek_Nazev) REFERENCES Lek;

ALTER TABLE Vykaz ADD CONSTRAINT PK_Vykaz PRIMARY KEY (ID);
ALTER TABLE Vykaz ADD CONSTRAINT FK_Vykaz_Pojistovna_ICO FOREIGN KEY (Pojistovna_ICO) REFERENCES Pojistovna;
ALTER TABLE Vykaz ADD CONSTRAINT FK_Vykaz_LekNaPredpis_Nazev FOREIGN KEY (LekNaPredpis_Nazev) REFERENCES LekNaPredpis;
ALTER TABLE Vykaz ADD CONSTRAINT FK_Vykaz_Prispevek_ID FOREIGN KEY (Prispevek_ID) REFERENCES Prispevek;

INSERT INTO Lek (Nazev, dostupnost, nutnostPredpisu, cenaLeku, mnozstvi)
VALUES('Ibuprofen', 1, 0, 199, 27);
INSERT INTO Lek (Nazev, dostupnost, nutnostPredpisu, cenaLeku, mnozstvi)
VALUES('Multivitamin', 0, 0, 599, 0);
INSERT INTO Lek (Nazev, dostupnost, nutnostPredpisu, cenaLeku, mnozstvi)
VALUES('Antibiotikum', 1, 1, 249, 1002);
INSERT INTO Lek (Nazev, dostupnost, nutnostPredpisu, cenaLeku, mnozstvi)
VALUES('AntibiotikumNova', 1, 1, 1049, 2);
INSERT INTO Lek (Nazev, dostupnost, nutnostPredpisu, cenaLeku, mnozstvi)
VALUES('Trittico', 0, 1, 149, 0);

INSERT INTO LekNaPredpis (Lek_Nazev, doporuceni)
VALUES('Antibiotikum', '1x denne');
INSERT INTO LekNaPredpis (Lek_Nazev, doporuceni)
VALUES('AntibiotikumNova', 'nepouzivat pri nachlazeni');
INSERT INTO LekNaPredpis (Lek_Nazev, doporuceni)
VALUES('Trittico', '1x vecer');

INSERT INTO Pojistovna (ICO, nazevPojistovny, ulice, mesto, PSC)
VALUES(12345678, 'VZP', 'Nadrazni', 'Prague', 15000);
INSERT INTO Pojistovna (ICO, nazevPojistovny, ulice, mesto, PSC)
VALUES(82345671, 'pVZP', 'Kolejni', 'Brno', 61200);
INSERT INTO Pojistovna (ICO, nazevPojistovny, ulice, mesto, PSC)
VALUES(34567890, 'Alfa', 'Kolejni', 'Brno', 61200);
INSERT INTO Pojistovna (ICO, nazevPojistovny, ulice, mesto, PSC)
VALUES(55555555, 'Beta', 'Kolejni', 'Brno', 61200);
INSERT INTO Pojistovna (ICO, nazevPojistovny, ulice, mesto, PSC)
VALUES(11111111, 'America', 'Kolejni', 'Brno', 61200);
INSERT INTO Pojistovna (ICO, nazevPojistovny, ulice, mesto, PSC)
VALUES(22222222, 'Europe', 'Kolejni', 'Brno', 61200);

INSERT INTO Prispevek (Pojistovna_ICO, LekNaPredpis_Nazev, castka)
VALUES(12345678, 'Antibiotikum', 50);
INSERT INTO Prispevek (Pojistovna_ICO, LekNaPredpis_Nazev, castka)
VALUES(82345671, 'Antibiotikum', 55);
INSERT INTO Prispevek (Pojistovna_ICO, LekNaPredpis_Nazev, castka)
VALUES(12345678, 'AntibiotikumNova', 100);
INSERT INTO Prispevek (Pojistovna_ICO, LekNaPredpis_Nazev, castka)
VALUES(82345671, 'AntibiotikumNova', 99);

INSERT INTO Vydani (Lek_Nazev, mnozstviProdane)
VALUES('Ibuprofen', 2);
INSERT INTO Vydani (Lek_Nazev, mnozstviProdane)
VALUES('Ibuprofen', 3);
INSERT INTO Vydani (Lek_Nazev, mnozstviProdane)
VALUES('Antibiotikum', 1);
INSERT INTO Vydani (Lek_Nazev, mnozstviProdane)
VALUES('AntibiotikumNova', 1);

INSERT INTO Vykaz (Pojistovna_ICO, LekNaPredpis_Nazev, Prispevek_ID, jmenoZakaznika)
VALUES (12345678, 'Antibiotikum', 1, 'Honza');
INSERT INTO Vykaz (Pojistovna_ICO, LekNaPredpis_Nazev, Prispevek_ID, jmenoZakaznika)
VALUES (12345678, 'AntibiotikumNova', 1, 'Hana');

COMMIT;

-- Vsechny sloupce dvou tabulek Prispevek a Pojišťovna, kde hodnota "Pojistovna_ICO"
-- v tabulce Prispevek odpovida hodnote "ICO" v tabulce Pojišťovna
SELECT *
FROM Prispevek
JOIN Pojistovna ON Prispevek.Pojistovna_ICO = Pojistovna.ICO;

-- Vsechny sloupce dvou tabulek Vykaz a LekNaPredpis, kde hodnota "LekNaPredpis_Nazev"
-- v tabulce Vykaz odpovida hodnote "Lek_Nazev" v tabulce LekNaPredpis
SELECT *
FROM Vykaz
JOIN LekNaPredpis ON Vykaz.LekNaPredpis_Nazev = LekNaPredpis.Lek_Nazev;

-- Sloupce "jmenoZakaznika" tabulky Vykaz, "castka" tabulky Prispevek a "doporuceni" tabulky
-- LekNaPredpis, se spojenim odpovidajicich hodnot ve vsech trech tabulkach
SELECT vk.jmenoZakaznika, pr.castka, lnp.doporuceni
FROM Vykaz vk
JOIN Prispevek pr ON vk.Prispevek_ID = pr.ID
JOIN LekNaPredpis lnp ON vk.LekNaPredpis_Nazev = lnp.Lek_Nazev;

-- Sloupce "Nazev" tabulky Lek a jeho celkovy prodej jako sloupec "TotalSell"
-- pomoci agregacni funkce SUM. Vysledek pak seskupime podle sloupce "Nazev" z "Lek"
SELECT l.Nazev, SUM(vd.mnozstviProdane) AS TotalSell
FROM Lek l
JOIN Vydani vd ON l.Nazev = vd.Lek_Nazev
GROUP BY l.Nazev;

-- Sloupce "nazevPojistovny" tabulky Pojistovna, "Lek_Nazev" tabulky LekNaPredpis a
-- "TotalForMed" - celkova castka zaplacena kazdou pojistovnou za kazdy lek,
-- se spojenim tri tabulek: Pojistovna, Prispevek a LekNaPredpis.
SELECT ps.nazevPojistovny, lnp.Lek_Nazev, SUM(pr.castka) AS TotalForMed
FROM Pojistovna ps
JOIN Prispevek pr ON ps.ICO = pr.Pojistovna_ICO
JOIN LekNaPredpis lnp ON pr.LekNaPredpis_Nazev = lnp.Lek_Nazev
GROUP BY ps.nazevPojistovny, lnp.Lek_Nazev;

-- Sloupce "Lek_Nazev", ktere maji alespon jeden odpovdajici zaznam v tabulce Prispevek,
-- WHERE kontroluje, jestli je v tabulce Prispevek alespon jeden radek, ktery
-- ma stejnou hodnotu "LekNaPredpis_Nazev" jako "Lek_Nazev" v tabulce LekNaPredpis
SELECT lnp.Lek_Nazev as LekySPrispevkem
FROM LekNaPredpis lnp
WHERE EXISTS (
  SELECT *
  FROM Prispevek pr
  WHERE pr.LekNaPredpis_Nazev = lnp.Lek_Nazev
);

-- Celkova castka penez, ktera byla zaplacena vsemi pojistovnami v Brne
-- (soucet sloupce "castka" z tabulky Prispevek, kde je hodnota sloupce "Pojistovna_ICO"
-- nalezena v tabulce Pojistovna jako "ICO", a  kde je hodnota "mesto" - Brno
SELECT SUM(pr.castka) as TotalMoneyInBrno
FROM Prispevek pr
WHERE pr.Pojistovna_ICO IN (
  SELECT ps.ICO
  FROM Pojistovna ps
  WHERE mesto = 'Brno'
);

COMMIT;

-------------------
-- Pokrocile dotazy
-------------------

-- automaticke aktualizuje dostupnost pri vydani
CREATE OR REPLACE TRIGGER update_dostupnost
AFTER INSERT ON Vydani
FOR EACH ROW
BEGIN
  UPDATE Lek SET mnozstvi = mnozstvi - :NEW.mnozstviProdane
  WHERE Nazev = :NEW.Lek_Nazev;
END;
/
-- verifikace
INSERT INTO Vydani (Lek_Nazev, mnozstviProdane)
VALUES ('Ibuprofen', 10);

-- overi spravnost PSC pri vytvoreni/aktualizaci pojistovny
CREATE OR REPLACE TRIGGER check_valid_psc
BEFORE INSERT OR UPDATE ON Pojistovna
FOR EACH ROW
BEGIN
  IF :NEW.PSC IS NOT NULL AND LENGTH(TO_CHAR(:NEW.PSC)) <> 5 THEN
    RAISE_APPLICATION_ERROR(-20001, 'Invalid PSC');
  END IF;
END;
/
-- verifikace check_valid_psc (povede k chybe)
-- INSERT INTO Pojistovna (ICO, nazevPojistovny, PSC)
-- VALUES (98765432, 'Maxima', 12345678);

-- dotaz EXPLAIN PLAN (spojeni Prispevek a Pojistovna,
-- agregacni funkce SUM, pouziti GROUP BY)
-- bez indexu
EXPLAIN PLAN FOR
  SELECT ps.nazevPojistovny, SUM(pr.castka)
  FROM Pojistovna ps
  INNER JOIN Prispevek pr ON ps.ICO = pr.Pojistovna_ICO
GROUP BY ps.nazevPojistovny;
--verifikace EXPLAIN PLAN
SELECT *
FROM TABLE(DBMS_XPLAN.DISPLAY());
-- vytvoreni indexu pro optimalizaci prace s Pojistovna
CREATE INDEX idx_Pojistovna_nazev ON Pojistovna(nazevPojistovny);
EXPLAIN PLAN FOR
  SELECT ps.nazevPojistovny, SUM(pr.castka)
  FROM Pojistovna ps
  INNER JOIN Prispevek pr ON ps.ICO = pr.Pojistovna_ICO
GROUP BY ps.nazevPojistovny;
--verifikace EXPLAIN PLAN x2
SELECT *
FROM TABLE(DBMS_XPLAN.DISPLAY());
-- je mozne urychlit pomoci zavedeni noveho indexu
CREATE INDEX idx_Prispevek_Pojistovna_ICO ON Prispevek(Pojistovna_ICO);
-- zopakujeme stejny dotaz
EXPLAIN PLAN FOR
SELECT p.nazevPojistovny, SUM(pr.castka)
FROM Pojistovna p
INNER JOIN Prispevek pr ON p.ICO = pr.Pojistovna_ICO
GROUP BY p.nazevPojistovny;
-- verifikace EXPLAIN PLAN x3
SELECT *
FROM TABLE(DBMS_XPLAN.DISPLAY());

-- pristupova prava to XFEDOR18
GRANT SELECT, INSERT, UPDATE, DELETE ON Lek TO XFEDOR18;
GRANT SELECT, INSERT, UPDATE, DELETE ON LekNaPredpis TO XFEDOR18;
GRANT SELECT, INSERT, UPDATE, DELETE ON Pojistovna TO XFEDOR18;
GRANT SELECT, INSERT, UPDATE, DELETE ON Prispevek TO XFEDOR18;
GRANT SELECT, INSERT, UPDATE, DELETE ON Vykaz TO XFEDOR18;
GRANT SELECT, INSERT, UPDATE, DELETE ON Vydani TO XFEDOR18;

-- complex SELECT dotaz s WITH a CASE
-- analyzuje prodeje leku a vypise Nazev Leku a jeho Kategorii prodeje
WITH
  total_sales AS (
    SELECT
      Lek.Nazev,
      SUM(Vydani.mnozstviProdane) AS sold_count
    FROM
      Lek
      JOIN Vydani ON Lek.Nazev = Vydani.Lek_Nazev
    GROUP BY
      Lek.Nazev
  )
SELECT
  total_sales.Nazev,
  CASE
    WHEN total_sales.sold_count < 10 THEN 'Nizke'
    WHEN total_sales.sold_count >= 10 AND total_sales.sold_count < 100 THEN 'Stredni'
    ELSE 'Velke'
  END AS sales_category
FROM
  total_sales;

COMMIT;
