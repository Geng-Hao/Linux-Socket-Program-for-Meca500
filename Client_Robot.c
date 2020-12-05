 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <arpa/inet.h>
 #include <sys/socket.h>
 #include <pthread.h>


 #define false 0  
 #define true  1
 #define max_filename_size 20
 #define max_message_size 500

 
 unsigned isEnd = false;
 
    
 /*The input argument of the child thread*/
 typedef struct socket_data{
 
   int sock;
   char* path;
 
 }socket_data;

 /*Write function*/
 void* clientWrite(void* socket_info){
      
      // the command to be sent to the robot
      char message[max_message_size];
      
      
      // socket related parameter
      socket_data* sock_ptr = (socket_data*)socket_info;  
      int sock_client = sock_ptr->sock;
      char* path = sock_ptr->path;
      
      
      // open the command program file
      FILE * programFile= fopen(path, "r+");

      if (programFile==NULL) {
	printf("File could not be opened!\n");
  	isEnd = true;
      }
      else{
	
	
	while (1) {  
	   
	 fgets(message,max_message_size,programFile); // read lines into the message string
	 
	 if(feof(programFile))break;
	 
	 write(sock_client,message,strlen(message)+1); // send the command to the robot
	 memset(message, 0, sizeof(message)); // erase the memory space of the message string
	  
	}
	
	fclose(programFile);
	isEnd = true;
      }
      
      pthread_exit(NULL);
 }
 
 
 /*Read function*/
 void* clientRead(void* socket_info){

      socket_data* sock_ptr = (socket_data*)socket_info;  
      int sock_client = sock_ptr->sock;
      
      char receive_message[max_message_size];

      while(1){
       
       if (isEnd == true) break;
       read(sock_client,receive_message,sizeof(receive_message));
       printf("Server:%s\n",receive_message);
       memset(receive_message, 0, sizeof(receive_message));
      
      }

      pthread_exit(NULL);
  }
 

 int main(){
        
        
	// create a socket
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	
	//request form the server(specific IP and port)
        struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;  //using the IPv4 address
	serv_addr.sin_addr.s_addr = inet_addr("192.168.0.100");  //IP dress of the server
	serv_addr.sin_port = htons(10000);  //port
	int conn = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if(conn==-1){
	 printf("Not Connected!\n");
	 return 0;
	}
	
	//write and read the message from the server
	socket_data s;
	
	char path[max_filename_size];
     	printf("Please enter the program file path:");
	scanf("%s",path);
	
	s.sock = sock;
	s.path = path;
	
	pthread_t t_write;
     	pthread_t t_read;
     	
	pthread_create(&t_write,NULL,clientWrite,(void*)&s);
	pthread_create(&t_read,NULL,clientRead,(void*)&s);
	pthread_join(t_write,NULL);  
	pthread_join(t_read,NULL);  
	//close the socket
	close(sock);
	
	return 0;
 }
