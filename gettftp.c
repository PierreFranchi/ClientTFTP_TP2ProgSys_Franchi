#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

//constantes 
#define taille_cmd 100
#define port (char*) "69"
const char erreur[taille_cmd]="gettftp + *host* + *file*\n";

//variables
char adresse[taille_cmd];
char fileName[taille_cmd];

//fonctions
void affiche(const char*msg){write(STDOUT_FILENO,msg, strlen(msg));}

int RRQ (char *fileName, char *mode, int sck, struct addrinfo *result){
    char *cmd;
    int snt;
    cmd=malloc(2+strlen(fileName)+1+strlen(mode)+1);
    cmd[0]=0;//2 bytes du opcode
    cmd[1]=1;
    strcpy(&cmd[2], fileName);
    cmd[2+strlen(fileName)]=0;
    strcpy(&cmd[2+strlen(fileName)+1],mode);
    cmd[2+strlen(fileName)+1+strlen(mode)]=0;

    snt=sendto(sck,cmd,2+strlen(mode)+1+strlen(fileName)+1,0,result->ai_addr,result->ai_addrlen);
    free(cmd);
    return(snt);//renvoi la taille de la commande
}
/*
int DATA(char *fileName, char *mode, int sck, struct addrinfo *result){
    char *cmd;
    int snt;
    cmd=malloc(2+2+strlen(data));
    cmd[0]=0;//2 bytes(2octets in english) du opcode
    cmd[1]=1;
    cmd[2]=0;//2 bytes de Bloc#
    cmd[3]=0;
    strcpy(&cmd[4], data);
      
    recvfrom(sfd, cmd, snt, flags, NULL, NULL);
    return 
}*/

//MAIN
int main (int argc, char* argv[]){
    if(argc<3){
        affiche(erreur);
        exit(EXIT_FAILURE);
    }
    argv[argc]=(char*)NULL;
    strcat(strcat(adresse,argv[1]),"\n");
    strcat(strcat(fileName,argv[2]),"\n");
    affiche(adresse);
    affiche(fileName);

    struct addrinfo hints;//pour filtrer les adresses disponibles
    struct addrinfo *result, *rp;//pour stocker tous les sokettes disponibles
    int s,sfd;
    
    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_family=AF_INET;//autorisation des adresses IPv4 uniquement
    hints.ai_socktype=SOCK_DGRAM;//UDP choisi
    hints.ai_flags=0;
    hints.ai_protocol=0;//autres protocoles

    s=getaddrinfo(argv[1],port, &hints, &result);//recherche les adresses correspondantes aux conditions
    if (s != 0) {//en cas d'erreur
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }
    for (rp=result; rp!=NULL; rp=rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;
        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break; // Success 
        close(sfd);
    }
    if (rp == NULL){ //aucune adresse trouvée
        fprintf(stderr, "Could not connect.\n");
        exit(EXIT_FAILURE);
    }
    int tailleCommande=RRQ(argv[2],"octet",sfd,result);
    return tailleCommande;

}