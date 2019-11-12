#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <mysql.h>
#include <pthread.h>

void *AtenderCliente (void *socket);


int main(int argc, char *argv[])
{
	int sock_conn, sock_listen;
	struct sockaddr_in serv_adr;
	// INICIALITZACIONS
	
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	// Fem el bind al port
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	// asocia el socket a cualquiera de las IP de la m?quina. 
	// htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// escucharemos en el port 9070
	serv_adr.sin_port = htons(9070);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	// La cola de peticiones pendientes no podra ser superior a 4
	if (listen(sock_listen, 4) < 0)
		printf("Error en el Listen");
	
	int i;
	int sockets[100];
	pthread_t thread[100];
	
	for (i=0; i<100; i++)
	{
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf("he recibido conexion\n");
		
		sockets[i] = sock_conn;
		
		pthread_create (&thread[i], NULL, AtenderCliente, &sockets[i]);
	}
	
}


void *AtenderCliente (void *socket)
{
	int ret;
	int sock_conn;
	int *s;
	s= (int *) socket;
	sock_conn= *s;
	char buff[512];
	char buff2[512];
	
	//conexion mysql
	
	MYSQL *conn;
	int err;
	
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	
	conn = mysql_init(NULL);
	if (conn==NULL) {
		printf ("Error al crear la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	conn = mysql_real_connect (conn,"localhost","root","mysql","BBDD", 0, NULL, 0);
	if (conn==NULL) {
		printf ("Error al inicializar la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	int i;
	char codigo[20]="";
	codigo[20]="10";
	
	while(strcmp(codigo,"0")!=0){
		
		// Ahora recibimos su nombre, que dejamos en buff
		ret=read(sock_conn,buff, sizeof(buff));
		printf ("Recibido\n");
		
		// Tenemos que a?adirle la marca de fin de string 
		// para que no escriba lo que hay despues en el buffer
		buff[ret]='\0';
		
		//Escribimos el nombre en la consola
		
		printf ("Se ha conectado: %s\n",buff);
		
		
		char *p = strtok( buff, "/");
		sprintf(codigo,"\0");
		strcpy(codigo,p);
		p = strtok( NULL, "/");
		char nombre[20];
		strcpy (nombre, p);
		printf ("Codigo: %s", codigo);
		
		
		
		if (strcmp(codigo,"3")==0){ //Nombre con partidas de +5kills
			
			char consulta3[300]="";
			char consulta1[200]={"SELECT Relacion.idpartida FROM Relacion,Usuarios WHERE Relacion.idusuario=Usuarios.id AND Usuarios.Nombre='"};
			char consulta2[200]={"' AND Relacion.kills>=5"};
			strcat(consulta3, consulta1);
			strcat(consulta3, nombre);
			strcat(consulta3, consulta2);
			err=mysql_query (conn, consulta3);
			
			if (err!=0) {
				printf ("Error al consultar datos de la base %u %s\n", mysql_errno(conn), mysql_error(conn));
				exit (1);
			}		 
			resultado = mysql_store_result (conn);
			row = mysql_fetch_row (resultado);
			
			if (row == NULL){
				printf ("No se han obtenido datos en la consulta\n");
				strcpy(buff2,"0");
			}
			else{
				sprintf(buff2,"\0");
				while (row !=NULL){
					strcat(buff2,row[0]);
					strcat(buff2,",");
					
					row = mysql_fetch_row (resultado);
					
				};
				buff2[strlen(buff2)-1]=NULL;
				
				printf ("Partidas de %s con +5 kills: %s\n", nombre,buff2);
			}
			
			printf("%s", buff2);
		}
		if (strcmp(codigo,"4")==0){
			
			char consultaalberto[300]="";
			char consultaa1[200]={"SELECT * FROM Relacion,Usuarios WHERE Relacion.idusuario=Usuarios.id AND Usuarios.Nombre='"};
			char consultaa2[200]={"' AND  Relacion.Asesino='"};
			strcat(consultaalberto, consultaa1);
			strcat(consultaalberto, nombre);
			strcat(consultaalberto, consultaa2);
			p = strtok( NULL, "/");
			char nombre2[20];
			strcpy(nombre2,p);
			strcat(consultaalberto, nombre2);
			strcat(consultaalberto,"'");
			
			
			
			err=mysql_query (conn, consultaalberto);
			if (err!=0) {
				printf ("Error al consultar datos de la base %u %s\n",
						mysql_errno(conn), mysql_error(conn));
				exit (1);
			}
			
			resultado = mysql_store_result (conn);
			
			row = mysql_fetch_row (resultado);
			int killsasesino=0;
			sprintf(buff2,"\0");
			if (row == NULL){
				printf ("No se han obtenido datos en la consulta\n");
				strcpy(buff2,"0");
			}
			else{
				while (row !=NULL){
					killsasesino=killsasesino+1;
					row = mysql_fetch_row (resultado);
				}
				
				sprintf(buff2,"%d",killsasesino);
			}
			if (err!=0)
				printf ("El total de kills de %s a %s es: %d\n", nombre2,nombre, killsasesino);
		}
		if (strcmp(codigo,"5")==0){
			char consultadiego[300]="";
			char consultad1[200]={"SELECT SUM(Relacion.kills) FROM Relacion,Usuarios WHERE Relacion.idusuario=Usuarios.id AND Usuarios.Nombre='"};
			strcat(consultadiego, consultad1);
			strcat(consultadiego, nombre);
			strcat(consultadiego, "'");
			
			err=mysql_query (conn, consultadiego);
			if (err!=0) {
				printf ("Error al consultar datos de la base %u %s\n", mysql_errno(conn), mysql_error(conn));
				exit (1);
			}
			
			resultado = mysql_store_result (conn);
			
			row = mysql_fetch_row (resultado);
			int killstotales=0;
			sprintf(buff2,"\0");
			if (row == NULL){
				printf ("No se han obtenido datos en la consulta\n");
				strcpy(buff2,"0");
			}
			else{	
				
				killstotales = atoi (row[0]);
				
				printf ("El total de kills de %s es: %d\n", nombre, killstotales);
				
			}
			strcpy(buff2,row[0]);;
			
			
			
		}
		if (strcmp(codigo,"1")==0){
			char consulta[300]="";
			char consultad1[200]={"SELECT Usuarios.Nombre FROM Usuarios WHERE Usuarios.Nombre='"};
			strcat(consulta, consultad1);
			strcat(consulta, nombre);
			strcat(consulta, "'");
			
			err=mysql_query (conn, consulta);
			if (err!=0) {
				printf ("Error al consultar datos de la base %u %s\n", mysql_errno(conn), mysql_error(conn));
				exit (1);
			}
			
			resultado = mysql_store_result (conn);
			
			row = mysql_fetch_row (resultado);
			int killstotales=0;
			sprintf(buff2,"\0");
			if (row != NULL){
				printf ("Usuario existente\n");
				strcpy(buff2,"0");
			}
			else{
				char consultamax[200]="SELECT MAX(Usuarios.id) FROM Usuarios";
				err=mysql_query (conn, consultamax);
				if (err!=0) {
					printf ("Error al consultar datos de la base %u %s\n", mysql_errno(conn), mysql_error(conn));
					exit (1);
				}
				resultado = mysql_store_result (conn);
				
				row = mysql_fetch_row (resultado);
				int newid=atoi(row[0])+1;
				char finalid[20]="";
				sprintf(finalid,"\0");
				sprintf(finalid,"%d",newid);
				
				
				char pwd[30]="";
				p = strtok( NULL, "/");
				strcpy(pwd,p);
				
				char consulta2[300]="";
				char consulta3[200]="INSERT INTO Usuarios VALUES ('";
				strcat(consulta2,consulta3);
				strcat(consulta2,finalid);
				strcat(consulta2,"', '");
				strcat(consulta2,nombre);
				strcat(consulta2,"', '");
				strcat(consulta2,pwd);
				strcat(consulta2,"', '1')");
				err=mysql_query (conn, consulta2);
				if (err!=0) {
					printf ("Error al consultar datos de la base %u %s\n", mysql_errno(conn), mysql_error(conn));
					exit (1);
				}				
				strcpy(buff2,"1");
			}
			
			
			
			
		}
		if (strcmp(codigo,"2")==0){
			char pwd[30];
			p = strtok( NULL, "/");
			strcpy(pwd,p);
			
			char consulta[300]="";
			char consultad1[200]={"SELECT Usuarios.Nombre FROM Usuarios WHERE Usuarios.Nombre='"};
			strcat(consulta, consultad1);
			strcat(consulta, nombre);
			strcat(consulta, "' AND Usuarios.Contrasena='");
			strcat(consulta,pwd);
			strcat(consulta,"'");
			printf ("%s",consulta);
			
			err=mysql_query (conn, consulta);
			if (err!=0) {
				printf ("Error al consultar datos de la base %u %s\n", mysql_errno(conn), mysql_error(conn));
				exit (1);
			}
			
			resultado = mysql_store_result (conn);
			
			row = mysql_fetch_row (resultado);
			sprintf(buff2,"\0");
			if(row!=NULL)
				strcpy(buff2,"1");
			else
				strcpy(buff2,"0");
		}
		
		
		
		write (sock_conn,buff2, strlen(buff2));
		
		// Se acabo el servicio para este cliente
		
	}
	close(sock_conn); 
	mysql_close (conn);
	printf("GoodBye");
}