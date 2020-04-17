#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>
#include <ctype.h>

typedef struct {
	char nombre [20];
	int socket;
} Conectado;

typedef struct {
	Conectado conectados [100];
	int num;
} ListaConectados;

ListaConectados miLista;

int count;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;//ACCESO EXLUYENTE

int addLista (char nombre[20], int socket){
	if (miLista.num == 100)
		return -1;
	else {
		pthread_mutex_lock(&mutex); 
		strcpy(miLista.conectados[miLista.num].nombre, nombre);
		miLista.conectados[miLista.num].socket = socket;
		miLista.num++;
		pthread_mutex_unlock(&mutex); 
		return 0;
	}
}
int DamePos(char nombre[20]){
	int i=0;
	int found = 0;
	while (( i<miLista.num) && !found){
		if ( strcmp(miLista.conectados[i].nombre, nombre)==0)
			found = 1;
		if (!found)
			i++;
	}
	if (found)
		return i;
	else
		return -1;
}
int delLista (char nombre[20]){
	int pos = DamePos(nombre);
	if (pos == -1)
		return -1;
	else {
		int i;
		pthread_mutex_lock(&mutex);
		for(i=pos;i<miLista.num-1;i++){
			strcpy(miLista.conectados[i].nombre, miLista.conectados[i+1].nombre);
			miLista.conectados[i].socket = miLista.conectados[i+1].socket;
		}
		miLista.num--;
		pthread_mutex_unlock(&mutex);
		return 0;
	}
}
int DameSocket (char nombre[20]){
	int i=0;
	int found = 0;
	while (( i<miLista.num) && !found){
		if ( strcmp(miLista.conectados[i].nombre, nombre)==0)
			found = 1;
		if (!found)
			i++;
	}
	if (found)
		return miLista.conectados[i].socket;
	else
		return -1;
}

void DameLista (char respuesta[512]){
	int i=0;
	if (miLista.num == 0)
		sprintf (respuesta, "no hay conectados");
	else {
		strcpy (respuesta, "lista de conectados: ");
		while (i<miLista.num){
			sprintf (respuesta, "%s%s, ", respuesta, miLista.conectados[i].nombre);
			printf ("conectado %d %s\n", i+1, miLista.conectados[i].nombre);
			i++;
		}
	}
}

void *AtenderCliente(void *socket){
	int sock_conn;
	int *s;
	s=(int *) socket;
	sock_conn = *s;
	
	char respuesta[512];
	char peticion[512];
	int ret;
	
	int terminar =0;
	// Entramos en un bucle para atender todas las peticiones de este cliente hasta que se desconecte
	
	while (terminar ==0){
		char nombre[20];
		char passw[20];
		char conectado[20];
		int edad, id;
		// Ahora recibimos la petici?n
		ret=read(sock_conn,peticion, sizeof(peticion));
		printf ("Recibido\n");
		
		// Tenemos que a?adirle la marca de fin de string 
		// para que no escriba lo que hay despues en el buffer
		peticion[ret]='\0';
		printf ("Peticion: %s\n",peticion);
		
		char *p = strtok( peticion, "/");
		int codigo =  atoi (p); //obtenemos codigo del servicio
		printf("codigo %d\n",codigo);
		if (codigo ==0){ //petici?n de desconexi?n
			terminar=1;
			pthread_mutex_lock(&mutex);
			int del = delLista(conectado);
			pthread_mutex_unlock(&mutex);
			if (del == -1)
				printf("error en desconexion\n");
			else
				printf("%s se ha desconectado\n", conectado);
		}
		else if (codigo == 1){ //consulta 1 numero de letras en nombre
			p = strtok(NULL, "/");
			strcpy(nombre,p);
			sprintf(respuesta,"%d",strlen(nombre)); 
		}
		else if (codigo == 2){ //consulta 2 nombre en mayusculas 
			p = strtok(NULL, "/");
			strcpy(nombre, p);
			int i;
			for(i=0; i<strlen(nombre);i++)
				nombre[i] =toupper(nombre[i]);
			strcpy(respuesta, nombre);
		}
		else if (codigo == 3){ //consulta 3 añ￱adir usuario en lista de conectados
			p = strtok(NULL, "/");
			strcpy(nombre, p);
			int add = addLista(nombre, sock_conn);
			if (add == -1)
				printf("lista llena\n");
			else
				printf("Anadid@ %s y Socket %d\n", nombre, DameSocket(nombre));
			sprintf(respuesta, "%d", add);
		}
		else if (codigo == 4) //lista de conectados
			DameLista(respuesta);
		else
				 printf("no hay consulta");
		
		if (codigo !=0){ // enviamos la respuesta al cliente
			printf ("Respuesta: %s\n", respuesta);
			write (sock_conn,respuesta, strlen(respuesta));
		}
		if ((codigo!=0)&&(codigo!=7)){
			pthread_mutex_lock(&mutex); //no interrumpir
			count++;//modificacion estructura compartida
			pthread_mutex_unlock(&mutex); //se puede interrumpir
		}
	}
	close(sock_conn); // Se acabo el servicio para este cliente

}

int main(int argc, char *argv[]){ 
	miLista.num =0;
	struct sockaddr_in serv_adr;
	int sock_conn, sock_listen;
	// INICIALITZACIONS
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket\n");
	// Fem el bind al port
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	
	// asocia el socket a cualquiera de las IP de la m?quina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(9050);// establecemos el puerto de escucha
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind\n");
	
	if (listen(sock_listen, 10) < 0) //
		printf("Error en el Listen\n");
	count =0;
	int i = 0;
	int sockets[100]; //lista de socket(func add y del)
	pthread_t thread[100];
	for (;;){	//depende num socket
		printf ("Escuchando\n");
		sock_conn = accept(sock_listen, NULL, NULL);//socket que usaremos para este cliente
		printf ("He recibido conexion\n");
		sockets[i] = sock_conn;
		pthread_create (&thread[i], NULL, AtenderCliente, &sockets[i]);
		i++;
	}
	exit(0);
}