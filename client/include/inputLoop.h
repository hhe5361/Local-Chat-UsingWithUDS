#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>

int main(){
    int fd;
    struct sockaddr_un addr;

    //unix domain socket creation
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    
    if(fd < 0){
        perror("socket error");
        return -1;
    }

    //socket define 
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "/tmp/local_chat_socket"); //socket listening through this file.
    unlink("/tmp/local_chat_socket"); //remove any previous socket file

    //이제 bind
    if(bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0){
        perror("bind error");
        return -1;
    }

    printf("Socket creation is finished.\n");

    //accept loop
    
        

}

