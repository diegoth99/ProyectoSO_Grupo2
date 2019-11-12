DROP DATABASE IF EXISTS BBDD;
CREATE DATABASE BBDD;
USE BBDD;
CREATE TABLE Usuarios (
id INTEGER NOT NULL,
Nombre VARCHAR(255) NOT NULL,
Contrasena VARCHAR(255) NOT NULL,
Nivel INTEGER,
PRIMARY KEY (id)
)ENGINE = InnoDB;

CREATE TABLE Partidas (
id INTEGER NOT NULL,
Ganador VARCHAR(255) NOT NULL,
Numjug INTEGER NOT NULL,
Fecha INTEGER NOT NULL,
Hora INTEGER NOT NULL,
Duracion INTEGER NOT NULL,
PRIMARY KEY (id)
)ENGINE = InnoDB;

CREATE TABLE Relacion (
idpartida INTEGER NOT NULL,
idusuario INTEGER NOT NULL,
kills INTEGER NOT NULL,
puntuacion INTEGER NOT NULL,
asesino VARCHAR(255),
FOREIGN KEY (idpartida) REFERENCES Partidas(id),
FOREIGN KEY (idusuario) REFERENCES Usuarios(id)
)ENGINE = InnoDB;

INSERT INTO Usuarios VALUES (1, "Diego", "estaesmicontrasena99", 30);
INSERT INTO Usuarios VALUES (2, "Alberto", "estupidoflanders", 30);
INSERT INTO Usuarios VALUES (3, "Luis", "mellamoralph", 30);

INSERT INTO Partidas VALUES (1, "Luis", 3, 18032019, 1205, 23);
INSERT INTO Partidas VALUES (2, "Diego", 3, 06092019, 1750, 25);
INSERT INTO Partidas VALUES (3, "Alberto", 3, 04102019, 1025, 27);
INSERT INTO Partidas VALUES (4, "Diego", 3, 08102019, 0925, 18);

INSERT INTO Relacion VALUES (1, 1, 1, 30, "Luis");
INSERT INTO Relacion VALUES (1, 2, 0, 0, "Diego");
INSERT INTO Relacion VALUES (1, 3, 1, 30, "NULL");


INSERT INTO Relacion VALUES (2, 1, 2, 60, "NULL");
INSERT INTO Relacion VALUES (2, 2, 0, 0, "Diego");
INSERT INTO Relacion VALUES (2, 3, 0, 0, "Diego");


INSERT INTO Relacion VALUES (3, 1, 0, 0, "Alberto");
INSERT INTO Relacion VALUES (3, 2, 2, 60, NULL);
INSERT INTO Relacion VALUES (3, 3, 0, 0, "Alberto");

INSERT INTO Relacion VALUES (4, 1, 1, 30, "NULL");
INSERT INTO Relacion VALUES (4, 2, 1, 30, "Diego");
INSERT INTO Relacion VALUES (4, 3, 0, 0, "Alberto");

