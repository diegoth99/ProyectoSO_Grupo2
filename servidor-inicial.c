#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <mysql.h>

int main(int argc, char *argv[])
{
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	char buff[512];
	char buff2[512];
	// INICIALITZACIONS
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	// Fem el bind al port
	
	
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	
	// asocia el socket a cualquiera de las IP de la m?quina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// escucharemos en el port 9050
	serv_adr.sin_port = htons(9010);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	//La cola de peticiones pendientes no podr? ser superior a 4
	if (listen(sock_listen, 3) < 0)
		printf("Error en el Listen");
	
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
	int codigo=10;
	
	while(codigo!=0){
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexi?n\n");
		//sock_conn es el socket que usaremos para este cliente
		
		// Ahora recibimos su nombre, que dejamos en buff
		ret=read(sock_conn,buff, sizeof(buff));
		printf ("Recibido\n");
		
		// Tenemos que a?adirle la marca de fin de string 
		// para que no escriba lo que hay despues en el buffer
		buff[ret]='\0';
		
		//Escribimos el nombre en la consola
		
		printf ("Se ha conectado: %s\n",buff);
		
		
		char *p = strtok( buff, "/");
		codigo =  atoi (p);
		p = strtok( NULL, "/");
		char nombre[20];
		strcpy (nombre, p);
		printf ("Codigo: %d", codigo);
		
		if (codigo ==3){ //Nombre con partidas de +5kills
			
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
			
			if (row == NULL)
				printf ("No se han obtenido datos en la consulta\n");
			else
			{
				sprintf(buff2,"\0");
				while (row !=NULL){
					strcat(buff2,row[0]);
					strcat(buff2,",");
					
					row = mysql_fetch_row (resultado);
					
				};
				buff2[strlen(buff2)-1]=NULL;
				
				printf ("Partidas de %s con +5 kills: %s\n", nombre,buff2);
			};
			
			printf("%s", buff2);
		}
		else if (codigo==4){
			
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
			if (row == NULL)
				printf ("No se han obtenido datos en la consulta\n");
			else{
				sprintf(buff2,"\0");
				while (row !=NULL){
					killsasesino=killsasesino+1;
					row = mysql_fetch_row (resultado);
				}
				buff2[0]=killsasesino;
			};
			if (err!=0)
				printf ("El total de kills de %s a %s es: %d\n", nombre2,nombre, killsasesino);
			
		else if (codigo==5){
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
			if (row == NULL)
				printf ("No se han obtenido datos en la consulta\n");
			else{	
				sprintf(buff2,"\0");
				killstotales = atoi (row[0]);
				
				printf ("El total de kills de %s es: %d\n", nombre, killstotales);
				
			}
			buff2[0]=killstotales;
			
			
			
		}
		else if (codigo==1){
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
			if (row != NULL){
				printf ("Usuario existente\n");
				buff2[0]=0;
			}
			else{
				char consulta[300]="";
				char consulta1[200]="SELECT MAX(Usuarios.id) FROM Usuarios";
				err=mysql_query (conn, consulta);
				if (err!=0) {
					printf ("Error al consultar datos de la base %u %s\n", mysql_errno(conn), mysql_error(conn));
					exit (1);
				}
				resultado = mysql_store_result (conn);
				
				row = mysql_fetch_row (resultado);
				int newid=atoi(row[0])+1;
				char finalid=atof(newid);
				
				char pwd[30]="";
				p = strtok( NULL, "/");
				strcpy(pwd,p);
				
				char consulta2[300]="";
				char consulta3[200]="INSERT INTO Usuarios VALUES (";
				strcat(consulta2,consulta3);
				strcat(consulta2,finalid);
				strcat(consulta2,", '");
				strcat(consulta2,nombre);
				strcat(consulta2,"', '");
				strcat(consulta2,pwd);
				strcat(consulta2,"', 1);");
				err=mysql_query (conn, consulta2);
				if (err!=0) {
					printf ("Error al consultar datos de la base %u %s\n", mysql_errno(conn), mysql_error(conn));
					exit (1);
				}				
				buff2[0]=1;
			}
			
			
			
			
		}
		else if (codigo==2){
			char pwd[30]="";
			p = strtok( NULL, "/");
			strcpy(pwd,p);
			
			char consulta[300]="";
			char consultad1[200]={"SELECT Usuarios.Nombre FROM Usuarios WHERE Usuarios.Nombre='"};
			strcat(consulta, consultad1);
			strcat(consulta, nombre);
			strcat(consulta, "' AND Usuarios.Contraseña='");
			strcat(consulta,pwd);
			strcat(consulta,"'");
			
			err=mysql_query (conn, consulta);
			if (err!=0) {
				printf ("Error al consultar datos de la base %u %s\n", mysql_errno(conn), mysql_error(conn));
				exit (1);
			}
			
			resultado = mysql_store_result (conn);
			
			row = mysql_fetch_row (resultado);
			
			if(row!=NULL)
				buff2[0]=1;
			else
				buff2[0]=0;
		}
			
			
			
		write (sock_conn,buff2, strlen(buff2));
			
			// Se acabo el servicio para este cliente
		close(sock_conn); 
	}
		mysql_close (conn);
		exit(0);
		printf("GoodBye");
}
}
