#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
// #include <readline/readline.h>
// #include <readline/history.h>

#define BUFFER_SIZE 1024

int is_faq = 0;
int sock;

void help_print()
{
    printf("1. “/active” - retrieve a list of active clients\n");
    printf("2. “/send <dest_id> <message>“ - send messages to other clients using their unique IDs\n");
    printf("3. “/logout” - client requests to exit the application. In such cases also, the server must send a message “Bye!! Have a nice day” as an acknowledgement to the client\n");
    printf("4. “/chatbot login“ - to avail the chatbot feature\n");
    printf("5. “/chatbot logout“ - to disable the feature\n");
    printf("6. “/history <recipient_id>“ : the server should retrieve the conversation history between the requesting client and the specified recipient.\n");
    printf("7. “/history_delete <recipient_id>” : delete chats of specified recipient from requesting client chat history.\n");
    printf("8. ““/delete_all” : delete complete chat history of requesting client.\n");

}

void process(char *argu, int client_socket){

    //remove last char enter
    if ((strlen(argu) > 0) && (argu[strlen(argu) -1] == '\n') )
        argu[strlen(argu)-1] = '\0';

    char cmd[BUFFER_SIZE];
    strcpy(cmd,argu);
    char *parameter[BUFFER_SIZE];
    int no_parameter;
    no_parameter = 0;
    char *token_p = strtok(cmd, " ");

    // Keep printing tokens while one of the
    while (token_p != NULL)
    {
        // printf("token %s token\n", token_p);
        parameter[no_parameter] = token_p;
        token_p = strtok(NULL, " ");
        no_parameter++;
    }
    parameter[no_parameter] = NULL;
    
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    //commend handle
    if (strcmp(parameter[0], "/help") == 0)
    {
        help_print();
    }
    else if (is_faq == 0 && strcmp(parameter[0], "/logout") == 0)
    {
        send(client_socket, argu, strlen(argu), 0);
        close(client_socket);
        printf("Disconnected from the server.\n");
        exit(1);
    }
    else if (is_faq == 0 && strcmp(parameter[0], "/active") == 0)
    {
        send(client_socket, argu, strlen(argu), 0);
        bzero(buffer, sizeof(buffer));
        printf("Active user list:\n");
        // active_user_process(client_socket);

    }
    else if (is_faq == 0 && strcmp(parameter[0], "/send") == 0)
    {
        send(client_socket, argu, strlen(argu), 0);
        bzero(buffer, sizeof(buffer));
        // send_process(client_socket, parameter[1], parameter[2 to rest]);
        // recv(client_socket, buffer, sizeof(buffer), 0);
    
        // Print the server's response (e.g., success or error message)
        // printf("%s\n", buffer);

    }
    else if(is_faq == 0 && strcmp(parameter[0], "/chatbot") == 0 && no_parameter > 1 && strcmp(parameter[1], "login") == 0)
    {
        send(client_socket, argu, strlen(argu), 0);
        bzero(buffer, sizeof(buffer));
        is_faq = 1;
        // faq_process(client_socket);
        // printf("now you are login to chatbot faq\n");
    }
    else if(is_faq == 0 && strcmp(parameter[0], "/chatbot_v2") == 0 && no_parameter > 1 && strcmp(parameter[1], "login") == 0)
    {
        send(client_socket, argu, strlen(argu), 0);
        bzero(buffer, sizeof(buffer));
        is_faq = 2;
        // faq_process(client_socket);
        // printf("now you are login to chatbot faq\n");
    }
    else if(is_faq == 1 && strcmp(parameter[0], "/chatbot") == 0 && no_parameter > 1 &&  strcmp(parameter[1], "logout") == 0)
    {
        send(client_socket, argu, strlen(argu), 0);
        bzero(buffer, sizeof(buffer));
        is_faq = 0;
        // faq_process(client_socket);
        // printf("now you are logout from chatbot faq\n");
    }
    else if(is_faq == 2 && strcmp(parameter[0], "/chatbot_v2") == 0 && no_parameter > 1 &&  strcmp(parameter[1], "logout") == 0)
    {
        send(client_socket, argu, strlen(argu), 0);
        bzero(buffer, sizeof(buffer));
        is_faq = 0;
        // faq_process(client_socket);
        // printf("now you are logout from chatbot faq\n");
    }
    else if (is_faq == 0 && strcmp(parameter[0], "/history") == 0)
    {
        send(client_socket, argu, strlen(argu), 0);
        bzero(buffer, sizeof(buffer));

    }
    else if (is_faq == 0 && strcmp(parameter[0], "/history_delete") == 0)
    {
        send(client_socket, argu, strlen(argu), 0);
        bzero(buffer, sizeof(buffer));

    }
    else if (is_faq == 0 && strcmp(parameter[0], "/delete_all") == 0)
    {
        send(client_socket, argu, strlen(argu), 0);
        bzero(buffer, sizeof(buffer));

    }
    else if(is_faq != 0)
    {
        if(argu[0] == '/'){
            printf("System Malfunction, I couldn't understand your query.\nno command are allowed in chatbot faq, type “/help“ to ge details\n");
            return;
        }
        send(client_socket, argu, strlen(argu), 0);
        bzero(buffer, sizeof(buffer));
        // faq_process(client_socket);
        // printf("now you are doing chatbot faq\n");
    }
    else{
        printf("Incorrect commend type “/help“ to get the list of valid commands\n");
    }
}


int main() {
    char *ip = "127.0.0.1";
    int port = 5566;
    struct sockaddr_in addr;
    fd_set readfds;
    char buffer[BUFFER_SIZE];
    char argu[BUFFER_SIZE];
    int n;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP client socket created.\n");

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    // Connect to server
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("[-]Connection failed");
        exit(1);
    }
    printf("Connected to the server.\n");

    // Receive welcome message and client ID from the server
    n = recv(sock, argu, BUFFER_SIZE, 0);
    if (n <= 0) {
        perror("[-]Error receiving welcome message");
        close(sock);
        exit(1);
    }
    argu[n] = '\0'; // Properly null-terminate the buffer

    printf("Server: %s\n", argu);

    // Check if the maximum number of clients has been reached
    if (strcmp(argu, "Maximum number of clients reached. Try again later.\n") == 0) {
        printf("Exiting...\n");
        close(sock);
        exit(0);
    }
    // printf("hhhhhhhhhhhhhhhhhhhhh");
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(0, &readfds); // Add stdin (0) to file descriptor set
        FD_SET(sock, &readfds); // Add socket to file descriptor set

        // Select activity
        if (select(sock + 1, &readfds, NULL, NULL, NULL) < 0) {
            perror("[-]Select error");
            exit(1);
        }

        // Check if stdin has input
        if (FD_ISSET(0, &readfds)) {
            memset(buffer, 0, sizeof(buffer));
            fgets(buffer, sizeof(buffer), stdin);
            // char *input;
            // input = readline("");

            // input[strcspn(input, "\n")] = '\0';
            // add_history(input);
            // send(sock, buffer, strlen(buffer), 0);
            process(buffer, sock);
        }

        // Check if socket has data to read
        if (FD_ISSET(sock, &readfds)) {
            memset(buffer, 0, sizeof(buffer));
            n = recv(sock, buffer, BUFFER_SIZE, 0);
            if (n<= 0) {
                printf("Server disconnected\n");
                break;
            } else {
                // printf("Server: %s", buffer);
                if (n > 0 && strcmp(buffer, "") != 0) {
                    buffer[n] = '\0';  // Properly null-terminate the received data
                    printf("%s\n", buffer);
                }
                if (n < 0) {
                    perror("[-]Error receiving active user list");
                }
            }
        }
    }

    close(sock);
    return 0;
}
