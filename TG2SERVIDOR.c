#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <mysql.h>
#include <pthread.h>
#include <my_global.h>

typedef struct{
	char nombre[20];
}Conectado;
typedef struct{
	Conectado lista[100];
	int num;
}ListaConectados;

ListaConectados conectados;
int i;
int sockets[100];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

char buff[512];
char buff2[512];

MYSQL* mysqlconn(){
	MYSQL *conn;
	
	conn = mysql_init(NULL);
	if (conn==NULL) {
		printf ("Error al crear la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	conn = mysql_real_connect (conn,"shiva2.upc.es","root","mysql","TG2BBDD", 0, NULL, 0);
	if (conn==NULL) {
		printf ("Error al inicializar la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}	
	return conn;
}


int sockconn(){
	int sock_conn,sock_listen;
	struct sockaddr_in serv_adr;
	
	// INICIALITZACIONS
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	// Fem el bind al port
	
	
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	
	// asocia el socket a cualquiera las IP de la m?quina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// establecemos el puerto de escucha
	serv_adr.sin_port = htons(50054);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	
	if (listen(sock_listen, 3) < 0)
		printf("Error en el Listen");
	return sock_listen;
}

void * Consulta3(char *nombre, MYSQL_RES* resultado, MYSQL* conn, MYSQL_ROW* row){
	
	char consulta3[300]="";
	char consulta1[200]={"SELECT Relacion.idpartida FROM Relacion,Usuarios WHERE Relacion.idusuario=Usuarios.id AND Usuarios.Nombre='"};
	char consulta2[200]={"' AND Relacion.kills>=5"};
	strcat(consulta3, consulta1);
	strcat(consulta3, nombre);
	strcat(consulta3, consulta2);
	
	int err=mysql_query (conn, consulta3);
	
	if (err!=0) {
		printf ("Error al consultar datos de la base %u %s\n", mysql_errno(conn), mysql_error(conn));
		
	}
	resultado = mysql_store_result (conn);
	row = mysql_fetch_row (resultado);
	sprintf(buff2,"\0");
	
	if (row == NULL){
		printf ("No se han obtenido datos en la consulta\n");
		strcpy(buff2,"3/0");
	}else{
		strcat(buff2,"3/");
		while (row !=NULL){
			strcat(buff2,row[0]);
			strcat(buff2,",");
			
			row = mysql_fetch_row (resultado);
			
		};
		buff2[strlen(buff2)-1]="\0";
	}
}

void * Consulta4(char *nombre2, char *nombre, MYSQL_RES* resultado, MYSQL* conn, MYSQL_ROW* row){
	
	char consultaalberto[300]="";
	char consultaa1[200]={"SELECT * FROM Relacion,Usuarios WHERE Relacion.idusuario=Usuarios.id AND Usuarios.Nombre='"};
	char consultaa2[200]={"' AND  Relacion.Asesino='"};
	strcat(consultaalberto, consultaa1);
	strcat(consultaalberto, nombre);
	strcat(consultaalberto, consultaa2);
	
	strcat(consultaalberto, nombre2);
	strcat(consultaalberto,"'");
	
	
	
	int err=mysql_query (conn, consultaalberto);
	if (err!=0) {
		printf ("Error al consultar datos de la base %u %s\n",
				mysql_errno(conn), mysql_error(conn));
	}
	
	resultado = mysql_store_result (conn);
	
	row = mysql_fetch_row (resultado);
	int killsasesino=0;
	sprintf(buff2,"\0");
	if (row == NULL){
		printf ("No se han obtenido datos en la consulta\n");
		strcpy(buff2,"4/0");
	}
	else{
		while (row !=NULL){
			killsasesino=killsasesino+1;
			row = mysql_fetch_row (resultado);
		}
		
		sprintf(buff2,"4/%d",killsasesino);
	}
	
	if (err!=0)
		printf ("El total de kills de %s a %s es: %d\n", nombre2,nombre, killsasesino);
	
}


void * Consulta5( char *nombre, MYSQL_RES* resultado, MYSQL* conn, MYSQL_ROW* row){
	
	char consultadiego[300]="";
	char consultad1[200]={"SELECT Relacion.kills FROM Relacion,Usuarios WHERE Relacion.idusuario=Usuarios.id AND Usuarios.Nombre='"};
	strcat(consultadiego, consultad1);
	strcat(consultadiego, nombre);
	strcat(consultadiego, "'");
	
	int err=mysql_query (conn, consultadiego);
	if (err!=0) {
		printf ("Error al consultar datos de la base %u %s\n", mysql_errno(conn), mysql_error(conn));
	}
	
	resultado = mysql_store_result (conn);
	
	row = mysql_fetch_row (resultado);
	int killstotales=0;
	sprintf(buff2,"\0");
	if (row == NULL){
		printf ("No se han obtenido datos en la consulta\n");
		strcpy(buff2,"5/0");
	}
	else{	
		while (row!=NULL){		
			killstotales += atoi (row[0]);
			row = mysql_fetch_row (resultado);
			
		}
		printf ("El total de kills de %s es: %d\n", nombre, killstotales);
	}
	sprintf(buff2,"5/%d",killstotales);
}

void * Consulta1( char *pwd,char *nombre, MYSQL_RES* resultado, MYSQL* conn, MYSQL_ROW* row){
	char consulta[300]="";
	char consultad1[200]={"SELECT Usuarios.Nombre FROM Usuarios WHERE Usuarios.Nombre='"};
	strcat(consulta, consultad1);
	strcat(consulta, nombre);
	strcat(consulta, "'");
	
	int err=mysql_query (conn, consulta);
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
		strcpy(buff2,"1/0");
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
		strcpy(buff2,"1/1");
	}
}

void * Consulta2(char *pwd, char *nombre, MYSQL_RES* resultado, MYSQL* conn, MYSQL_ROW* row){
	
	char consulta[300]="";
	char consultad1[200]={"SELECT Usuarios.Nombre FROM Usuarios WHERE Usuarios.Nombre='"};
	strcat(consulta, consultad1);
	strcat(consulta, nombre);
	strcat(consulta, "' AND Usuarios.Contrasena='");
	strcat(consulta,pwd);
	strcat(consulta,"'");
	printf ("%s",consulta);
	
	int err=mysql_query (conn, consulta);
	if (err!=0) {
		printf ("Error al consultar datos de la base %u %s\n", mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	resultado = mysql_store_result (conn);
	
	row = mysql_fetch_row (resultado);
	sprintf(buff2,"\0");
	if(row!=NULL)
	{
		strcpy(buff2,"2/1");
		
		pthread_mutex_lock(&mutex);
		strcpy(conectados.lista[conectados.num].nombre,nombre);
		conectados.num=conectados.num+1;
		pthread_mutex_unlock(&mutex);
		
		char notificacion[1000];
		sprintf(notificacion,"\0");
		int j;
		sprintf(notificacion,"6/%d/",conectados.num);
		for(j=0; j<conectados.num; j++)
		{
			sprintf(notificacion,"%s%s_",notificacion,conectados.lista[j].nombre);
		}
		notificacion[strlen(notificacion)-1]="\0";
		int k;
		for(k=0; k<i; k++)
			write(sockets[k],notificacion,strlen(notificacion));
	}
	else
		strcpy(buff2,"2/0");
}

void *AtenderCliente (void *socket)
{
	int sock_conn;
	int *s;
	s= (int *) socket;
	sock_conn= *s;
	
	//int socket_conn = * (int *) socket;
	
	int ret;
	char codigo[20]="";
	
	//MYSQL
	
	MYSQL* conn = mysqlconn();
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	int err;
	
	// Entramos en un bucle para atender todas las peticiones de este cliente
	//hasta que se desconecte
	int terminar=0;
	while (terminar==0){
		
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
		printf ("Codigo: %s", codigo);
		
		char nombre[20];
		char user[20];
		if (strcmp(codigo,"0")!=0){
			p = strtok( NULL, "/");
			strcpy (nombre, p);
		}else{
			terminar=1;			
		}
		if (strcmp(codigo,"3")==0){ //Nombre con partidas de +5kills
			Consulta3(nombre,resultado,conn,row);
			printf("%s", buff2);
			
		}
		else if (strcmp(codigo,"4")==0){
			p = strtok( NULL, "/");
			char nombre2[20];
			strcpy(nombre2,p);
			Consulta4(nombre2,nombre,resultado,conn,row);
			printf("%s", buff2);
		}
		else if (strcmp(codigo,"5")==0){
			Consulta5(nombre,resultado,conn,row);
			printf("%s", buff2);
		}
		else if (strcmp(codigo,"1")==0){
			char pwd[30]="";
			p = strtok( NULL, "/");
			strcpy(pwd,p);
			Consulta1(pwd,nombre,resultado,conn,row);
		}
		else if (strcmp(codigo,"2")==0){
			char pwd[30];
			p = strtok( NULL, "/");
			strcpy(pwd,p);
			strcpy(user,nombre);
			Consulta2(pwd,nombre,resultado,conn,row);
		}
		if (strcmp(codigo,"0")!=0)
			write (sock_conn,buff2, strlen(buff2));
		if (terminar==1)
		{
			printf("Usuario a eliminar: %s",user);
			int encontrado = 0;
			for(int b=0; b<conectados.num && encontrado==0; b++)
			{
				if(strcmp(conectados.lista[b].nombre,user)==0)
				{
					encontrado=1;
				}
			}
			pthread_mutex_lock(&mutex);
			for(int b; b<(conectados.num-1); b++)
			{
				strcpy(conectados.lista[b].nombre,conectados.lista[b+1].nombre);
				sockets[b]=sockets[b+1];
			}
			strcpy(conectados.lista[conectados.num-1].nombre,"");
			sockets[conectados.num-1]='\0';
			conectados.num=conectados.num-1;
			pthread_mutex_unlock(&mutex);
			
			char notificacion[1000];
			sprintf(notificacion,"\0");
			int j;
			sprintf(notificacion,"6/%d/",conectados.num);
			for(j=0; j<conectados.num; j++)
			{
				sprintf(notificacion,"%s%s_",notificacion,conectados.lista[j].nombre);
			}
			notificacion[strlen(notificacion)-1]="\0";
			int k;
			for(k=0; k<i; k++)
				write(sockets[k],notificacion,strlen(notificacion));
		}
	}
	// Se acabo el servicio para este cliente
	close(sock_conn);
	mysql_close(conn);
}

int main(int argc, char *argv[])
{
	int sock_conn, sock_listen;
	sock_listen=sockconn();
	
	conectados.num=0;

	pthread_t thread;
	i=0;
	for (;;){
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexion\n");
		
		sockets[i] =sock_conn;
		//sock_conn es el socket que usaremos para este cliente
		
		// Crear thead y decirle lo que tiene que hacer
		
		pthread_create (&thread, NULL, AtenderCliente,&sockets[i]);
		i=i+1;
		
	}
	
}

