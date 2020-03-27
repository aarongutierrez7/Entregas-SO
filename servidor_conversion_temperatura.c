#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>



int main(int argc, char *argv[])
{
	
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	char peticion[512];
	char respuesta[512];
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
	// establecemos el puerto de escucha
	serv_adr.sin_port = htons(9100);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	
	if (listen(sock_listen, 3) < 0)
		printf("Error en el Listen");
	
	int i;
	// Bucle infinito
	for (;;){
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexion\n");
		//sock_conn es el socket que usaremos para este cliente
		
		int terminar =0;
		// Entramos en un bucle para atender todas las peticiones de este cliente
		//hasta que se desconecte
		while (terminar ==0)
		{
			// Ahora recibimos la petici?n
			ret=read(sock_conn,peticion, sizeof(peticion));
			printf ("Recibido\n");
			
			// Tenemos que añadirle la marca de fin de string 
			// para que no escriba lo que hay despues en el buffer
			peticion[ret]='\0';
			
			
			printf ("Peticion: %s\n",peticion);
			
			// vamos a ver que quieren
			char *p = strtok( peticion, "/");
			int codigo =  atoi (p);
			// Ya tenemos el c?digo de la petici?n
			char temperatura[10];
			float temp;
			char unidad;
			
			if (codigo !=0)
			{
				p = strtok( NULL, "/");
				
				temp = atof (p);
				
				p = strtok( NULL, "/");
				
				strcpy (unidad, p);
				// Ya tenemos la temperatura y su unidad de referencia
				printf ("Codigo: %d, Temperatura: %.2f%s\n", codigo, temp, unidad);
			}
			
			if (codigo ==0) //peticion de desconexion
				terminar=1;
			else if (codigo ==1) //piden la conversion de grados Celsius a Fahrenheit y viceversa
				if (unidad == 'º')
				{
					temp = (temp*1.8) + 32;
					sprintf (respuesta, "La conversion a grados Fahrenheit es: %.2f F", temp);
				}
				else if (unidad == 'F')
				{
					temp = (temp - 32)*1.8;
					sprintf (respuesta, "La conversion a grados Celsius es: %.2f º", temp);
				}
		if (codigo !=0)
		{
		printf ("Respuesta: %s\n", respuesta);
		// Enviamos respuesta
		write (sock_conn,respuesta, strlen(respuesta));
		}
		}
		// Se acabo el servicio para este cliente
		close(sock_conn); 
	}
}
