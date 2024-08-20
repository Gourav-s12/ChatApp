#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <uuid/uuid.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define CHAT_HISTORY_PATH "chat_history"

// Structure to represent a message
struct Message {
    char sender[37]; // Sender's ID
    char receiver[37]; // Receiver's ID
    char content[BUFFER_SIZE]; // Message content
};

struct ClientInfo {
    int socket;
    struct sockaddr_in address;
    char id[37]; // ID (UUIDv4) for each client
    int is_active;
    int is_faq;
};

struct ClientInfo client_data[MAX_CLIENTS];


void send_faq_user_tag(struct ClientInfo *client_info) {
    // char welcome_message[BUFFER_SIZE];
    // sprintf(welcome_message, "user> ");
    // send(client_info->socket, welcome_message, strlen(welcome_message), 0);
}

void send_welcome_message(struct ClientInfo *client_info) {
    char welcome_message[BUFFER_SIZE];
    sprintf(welcome_message, "Welcome to the server! Your ID is: %s\n", client_info->id);
    send(client_info->socket, welcome_message, strlen(welcome_message), 0);
}

void send_faq_welcome_message(struct ClientInfo *client_info) {
    char welcome_message[BUFFER_SIZE];
    sprintf(welcome_message, "stupidbot> Hi, I am stupid bot, I am able to answer a limited set of your questions");
    send(client_info->socket, welcome_message, strlen(welcome_message), 0);
    send_faq_user_tag(client_info);
}

void send_faq_bye_message(struct ClientInfo *client_info) {
    char welcome_message[BUFFER_SIZE];
    sprintf(welcome_message, "stupidbot> Bye! Have a nice day and do not complain about me");
    send(client_info->socket, welcome_message, strlen(welcome_message), 0);
}

void send_faq_v2_welcome_message(struct ClientInfo *client_info) {
    char welcome_message[BUFFER_SIZE];
    sprintf(welcome_message, "gpt2bot> Hi, I am updated bot, I am able to answer any question be it correct or incorrect");
    send(client_info->socket, welcome_message, strlen(welcome_message), 0);
    send_faq_user_tag(client_info);
}

void send_faq_v2_bye_message(struct ClientInfo *client_info) {
    char welcome_message[BUFFER_SIZE];
    sprintf(welcome_message, "gpt2bot> Bye! Have a nice day and hope you do not have any complaints about me");
    send(client_info->socket, welcome_message, strlen(welcome_message), 0);
}

// Add a function to send a confirmation message to the client
void send_confirmation(struct ClientInfo *client_info, const char *message) {
    int n = send(client_info->socket, message, strlen(message), 0);
    if (n < 0) {
        perror("[-]Error in sending data");
    } else {
        printf("Server: Client (%s) %s\n", client_info->id, message);
    }
}
void send_messege(int socket, const char *id, const char *message) {
    int n = send(socket, message, strlen(message), 0);
    if (n < 0) {
        perror("[-]Error in sending data");
    }
    // else {
    //     printf("Server: Client (%s) %s\n", id, message);
    // }
}

void active_user_process(struct ClientInfo *client_info) {
    char active_users[BUFFER_SIZE];
    int length = 0;

    // Prepare the list of active user IDs
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (client_data[i].socket != 0) {
            // printf("Client (%s) iss active.\n",client_data[i].id);
            length += snprintf(active_users + length, BUFFER_SIZE - length, "%s\n", client_data[i].id);
        }
    }

    // Send the list of active user IDs to the client
    // printf("Client (%s) requested to active.%s\n",client_info->id, active_users);
    send(client_info->socket, active_users, strlen(active_users), 0);
    // Send the answer over the socket
    // send_messege(client_info->socket, client_info->id, active_users );
}

// Function to append messages to the chat history log file
void append_to_chat_history(struct Message *message) {
    FILE *file = fopen(CHAT_HISTORY_PATH, "a");
    if (file == NULL) {
        perror("[-]Error opening chat history file");
        return;
    }
    fprintf(file, "%s\t%s\t%s\t%s\n", message->sender, message->sender, message->receiver, message->content);
    fprintf(file, "%s\t%s\t%s\t%s\n", message->receiver, message->sender, message->receiver, message->content);
    fclose(file);
}

// Function to truncate the chat history file
void truncate_chat_history_file() {
    FILE *file = fopen(CHAT_HISTORY_PATH, "w");
    if (file == NULL) {
        perror("[-]Error opening chat history file");
        return;
    }
    fclose(file);
}

void send_user_process(struct ClientInfo *client_info, const char *id, const char *para_message) {
    char active_users[BUFFER_SIZE];
    int length = 0;

    int i = 0;
    for (; i < MAX_CLIENTS; i++) {
        if(client_data[i].socket != 0 && strcmp(client_data[i].id,id)==0){
            // send message
            char message[BUFFER_SIZE];
            snprintf(message, BUFFER_SIZE, "client(%s)> %s", client_info->id, para_message);

            // histroy
            struct Message message_history;
            strcpy(message_history.sender, client_info->id);
            strcpy(message_history.receiver, client_data[i].id);
            strcpy(message_history.content, para_message); 

            // Append message to chat history
            append_to_chat_history(&message_history);

            // if(client_data[i].is_faq == 0){
                send_messege(client_data[i].socket, client_data[i].id, message);
            // }
            break;
        }
    }
    // send_process(client_info, parameter[1], parameter[2 to rest]);
    if(i == MAX_CLIENTS){
        send_confirmation(client_info, "Messege sent unsuccessful - wrong reciver id - Operation terminated.");
    }else{
        send_confirmation(client_info, "Messege sent successfully - Operation completed.");
    }
}

// Function to read the FAQ file and return the answer corresponding to the question
char *get_answer_from_faq(const char *question) {
    FILE *faq_file = fopen("FAQs.txt", "r");
    if (faq_file == NULL) {
        perror("[-]Error opening FAQ file");
        return "Internal server error";
    }

    char line[BUFFER_SIZE];
    char *token;
    while (fgets(line, BUFFER_SIZE, faq_file) != NULL) {
        token = strtok(line, "|||");
        if (strcmp(token, question) == 0) {
            fclose(faq_file);
            return strtok(NULL, "|||");
        }
    }

    fclose(faq_file);
    return "System Malfunction, I couldn't understand your query.\n";
}

// Function to handle processing client's questions in FAQ mode
// void faq_process(struct ClientInfo *client_info, const char *question) {
//     char *ans = get_answer_from_faq(question);
//     char *answer = strcat("stupidbot> ",ans);
//     send_confirmation(client_info, answer);
// }
void faq_process(struct ClientInfo *client_info, const char *question) {
    char *ans = get_answer_from_faq(question);
    printf("hello\n");
    char *answer = malloc(strlen(ans) + strlen("stupidbot> ") + 1); // Allocate memory for the answer
    if (answer == NULL) {
        // Handle memory allocation failure
        return;
    }
    strcpy(answer, "stupidbot> "); // Copy the prefix
    strcat(answer, ans); // Concatenate the answer
    
    // Remove newline character if it exists at the end of the answer
    size_t len = strlen(answer);
    if (len > 0 && answer[len - 1] == '\n') {
        answer[len - 1] = '\0'; // Replace newline character with null terminator
    }
    
    // Send the answer over the socket
    send_messege(client_info->socket, client_info->id, answer);
    
    // Free dynamically allocated memory
    free(answer);
}

char *trim(char *string)
{
    char *ptr = NULL;
    while (*string == ' ') string++;  // chomp away space at the start
    ptr = string + strlen(string) - 1; // jump to the last char (-1 because '\0')
    while (*ptr == ' '){ *ptr = '\0' ; ptr--; } ; // overwrite with end of string
    return string;  // return pointer to the modified start 
}

void faq_v2_process(struct ClientInfo *client_info, const char *question) {
    char command[BUFFER_SIZE];
    snprintf(command, sizeof(command), "python3 gpt_2_gen.py \"%s\"", question);

    printf("%s\n",command);
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        return;
    }

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // printf("%s", buffer);
        send_messege(client_info->socket, client_info->id, buffer);
    }

    send_faq_user_tag(client_info);
    pclose(fp);
}


// Function to send the conversation history to the client
void send_history(struct ClientInfo *client_info, const char *recipient_id) {
    if(strcmp(recipient_id,client_info->id)==0){
        send_confirmation(client_info, "That is your own id , you need to enter other's id.");
        return;
    }


    FILE *file = fopen(CHAT_HISTORY_PATH, "r");
    if (file == NULL) {
        perror("[-]Error opening chat history file");
        return;
    }

    char line[BUFFER_SIZE];
    char *token;
    while (fgets(line, BUFFER_SIZE, file) != NULL) {
        char *user_id = strtok(line, "\t");
        char *sender_id = strtok(NULL, "\t");
        char *receiver_id = strtok(NULL, "\t");
        char *content = strtok(NULL, "\t");
        if (strcmp(user_id, client_info->id) == 0 && strcmp(sender_id, recipient_id) == 0) {
            // Construct the message to include the recipient ID and content
            char message[BUFFER_SIZE];
            snprintf(message, BUFFER_SIZE, "from %s:\n%s", sender_id, content);
            send_messege(client_info->socket, client_info->id, message);
        } else if (strcmp(user_id, client_info->id) == 0 && strcmp(receiver_id, recipient_id) == 0) {
            // Construct the message to include the sender ID and content
            char message[BUFFER_SIZE];
            snprintf(message, BUFFER_SIZE, "to %s:\n%s", receiver_id, content);
            send_messege(client_info->socket, client_info->id, message);
        }
    }

    fclose(file);
    send_confirmation(client_info, "That is all the chats found with the specified recipient in chat history.");
}

// Function to delete chats of the specified recipient from the requesting client's chat history
void delete_recipient_chats(struct ClientInfo *client_info, const char *recipient_id) {
    FILE *input_file = fopen(CHAT_HISTORY_PATH, "r");
    if (input_file == NULL) {
        perror("[-]Error opening chat history file");
        return;
    }

    // Create a temporary file to write the updated chat history
    FILE *output_file = fopen("temp_chat_history.txt", "w");
    if (output_file == NULL) {
        perror("[-]Error creating temporary chat history file");
        fclose(input_file);
        return;
    }

    char line[BUFFER_SIZE];
    char copy_line[BUFFER_SIZE];
    int deleted = 0; // Flag to track if any messages were deleted

    // Read the chat history line by line
    while (fgets(line, BUFFER_SIZE, input_file) != NULL) {
        strcpy(copy_line, line);
        char *user_id = strtok(line, "\t");
        char *sender_id = strtok(NULL, "\t");
        char *receiver_id = strtok(NULL, "\t");
        char *content = strtok(NULL, "\t");

        // Check if the message is between the requesting client and the specified recipient
        if (strcmp(user_id, client_info->id) == 0 && (strcmp(sender_id, recipient_id) == 0 || strcmp(receiver_id, recipient_id) == 0)) {
            deleted = 1; // Set the flag to indicate deletion
            continue; // Skip writing this message to the output file
        }

        // Write the message to the output file
        fputs(copy_line, output_file);
    }

    fclose(input_file);
    fclose(output_file);

    // Replace the original chat history file with the temporary file
    if (deleted) {
        if (rename("temp_chat_history.txt", CHAT_HISTORY_PATH) != 0) {
            perror("[-]Error replacing chat history file");
            remove("temp_chat_history.txt"); // Delete the temporary file
            send_confirmation(client_info, "Failed to delete recipient's chats from chat history.");
            return;
        }
        send_confirmation(client_info, "Recipient's chats deleted successfully from chat history.");
    } else {
        remove("temp_chat_history.txt"); // Delete the temporary file
        send_confirmation(client_info, "No chats found with the specified recipient in chat history.");
    }
}

void delete_all_chats(struct ClientInfo *client_info) {
    FILE *input_file = fopen(CHAT_HISTORY_PATH, "r");
    if (input_file == NULL) {
        perror("[-]Error opening chat history file");
        return;
    }

    // Create a temporary file to write the updated chat history
    FILE *output_file = fopen("temp_chat_history.txt", "w");
    if (output_file == NULL) {
        perror("[-]Error creating temporary chat history file");
        fclose(input_file);
        return;
    }

    char line[BUFFER_SIZE];
    char copy_line[BUFFER_SIZE];

    int deleted = 0; // Flag to track if any messages were deleted

    // Read the chat history line by line
    while (fgets(line, BUFFER_SIZE, input_file) != NULL) {
        strcpy(copy_line, line);
        char *user_id = strtok(line, "\t");
        char *sender_id = strtok(NULL, "\t");
        char *receiver_id = strtok(NULL, "\t");
        char *content = strtok(NULL, "\t");

        // Check if the message is between the requesting client and the specified recipient
        if (strcmp(user_id, client_info->id) == 0) {
            deleted = 1; // Set the flag to indicate deletion
            continue; // Skip writing this message to the output file
        }

        // Write the message to the output file
        fputs(copy_line, output_file);
    }

    fclose(input_file);
    fclose(output_file);

    // Replace the original chat history file with the temporary file
    if (deleted) {
        if (rename("temp_chat_history.txt", CHAT_HISTORY_PATH) != 0) {
            perror("[-]Error replacing chat history file");
            remove("temp_chat_history.txt"); // Delete the temporary file
            send_confirmation(client_info, "Failed to delete chats from chat history.");
            return;
        }
        send_confirmation(client_info, "All chats deleted successfully from chat history.");
    } else {
        remove("temp_chat_history.txt"); // Delete the temporary file
        send_confirmation(client_info, "No chats found in chat history.");
    }
}

void client_handler(struct ClientInfo *client_info, char *argu) {

    // struct ClientInfo *client_info = (struct ClientInfo *)arg;

    char buffer[BUFFER_SIZE];
    strcpy(buffer, argu);

    char question_buffer[BUFFER_SIZE];
    strcpy(question_buffer ,buffer);
    char *parameter[BUFFER_SIZE];
    int no_parameter;
    no_parameter = 0;
    char *token_p = strtok(buffer, " ");

    // Keep printing tokens while one of the
    while (token_p != NULL && no_parameter<2)
    {
        // printf("token %s token\n", token_p);
        parameter[no_parameter] = token_p;
        token_p = strtok(NULL, " ");
        no_parameter++;
    }
    if (token_p != NULL) {
        // Allocate memory for the last parameter
        parameter[2] = malloc(BUFFER_SIZE);
        if (parameter[2] == NULL) {
            // Handle memory allocation failure
            perror("[-]Error allocating memory");
            exit(1);
        }
        strcpy(parameter[2], token_p);

        // Concatenate the remaining tokens into the last parameter
        token_p = strtok(NULL, ""); // Use empty delimiter to read the rest of the string
        while (token_p != NULL) {
            strcat(parameter[2], " "); // Add space before concatenating the token
            strcat(parameter[2], token_p);
            token_p = strtok(NULL, ""); // Use empty delimiter to read the rest of the string
        }
    }

    // // Print the parameters (for testing)
    // for (int i = 0; i < 3; i++) {
    //     if (parameter[i] != NULL) {
    //         printf("Parameter %d: %s\n", i, parameter[i]);
    //     }
    // }

    if (client_info->is_faq == 0){
        if (strcmp(parameter[0], "/active") == 0) {
            printf("Client (%s) requested to active.\n",client_info->id);
            active_user_process(client_info);
            send_confirmation(client_info, "Active user list send successfully - Operation completed.");

        } else if (strcmp(parameter[0], "/send") == 0) {
            send_user_process(client_info, parameter[1], parameter[2]);

        } else if (strcmp(parameter[0], "/logout") == 0) {
            printf("Client (%s) requested to disconnect.\n",client_info->id);
            // break;
        } else if (strcmp(parameter[0], "/chatbot") == 0 && strcmp(parameter[1], "login") == 0){
            client_info->is_faq = 1;
            send_faq_welcome_message(client_info);
        } else if (strcmp(parameter[0], "/chatbot_v2") == 0 && strcmp(parameter[1], "login") == 0){
            client_info->is_faq = 2;
            send_faq_v2_welcome_message(client_info);
        } else if (strcmp(parameter[0], "/history") == 0 && no_parameter>1) {
            // Check if the command format is correct
            // if (parameter[1] == NULL) {
            //     send_confirmation(client_info, "Usage: /history <recipient_id>");
            //     continue;
            // }
            // Extract recipient ID
            char *recipient_id = parameter[1];
            send_history(client_info, recipient_id);
        }else if (strcmp(parameter[0], "/history_delete") == 0  && no_parameter>1) {
            
            char *recipient_id = parameter[1];
            printf("%s hello\n",recipient_id);
            delete_recipient_chats(client_info, recipient_id);
            
        }else if (strcmp(parameter[0], "/delete_all") == 0) {
            
            delete_all_chats(client_info);
        } else {
            // For any unrecognized command
            send_confirmation(client_info, "Unrecognized command.");
        }
    } else if(client_info->is_faq == 1){
        if(strcmp(parameter[0], "/chatbot") == 0 && strcmp(parameter[1], "logout") == 0){
            client_info->is_faq = 0;
            send_faq_bye_message(client_info);
        }else{
            trim(question_buffer);
            strcat(question_buffer , " ");
            faq_process(client_info, question_buffer);
            
        }
    }else{
        if(strcmp(parameter[0], "/chatbot_v2") == 0 && strcmp(parameter[1], "logout") == 0){
            client_info->is_faq = 0;
            send_faq_v2_bye_message(client_info);
        }else{
            trim(question_buffer);
            faq_v2_process(client_info, question_buffer);
            
        }
    }
}


int main() {
    char *ip = "127.0.0.1";
    int port = 5566;
    int server_sock, client_socks[MAX_CLIENTS];
    struct sockaddr_in server_addr, client_addr;
    fd_set readfds, master;
    int max_sd, activity, i, valread, num_clients = 0;
    socklen_t addr_size;
    char buffer[1024];
    char arr[MAX_CLIENTS][37];

    // Create server socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // Bind server socket
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("[-]Bind error");
        exit(1);
    }
    printf("[+]Bind to the port number: %d\n", port);

    // Listen
    if (listen(server_sock, 5) < 0) {
        perror("[-]Listen error");
        exit(1);
    }
    printf("Listening...\n");

    // Truncate chat history file when the server starts
    truncate_chat_history_file();

    // Initialize client sockets array
    for (i = 0; i < MAX_CLIENTS; ++i) {
        // client_socks[i] = 0;
        client_data[i].socket = 0;
        client_data[i].is_faq = 0;
        strcpy(client_data[i].id, "");
    }

    FD_ZERO(&master);
    FD_SET(server_sock, &master);
    max_sd = server_sock;

    while (1) {
        readfds = master;

        // Select activity
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("[-]Select error");
            exit(1);
        }

        // Handle incoming connections
        if (FD_ISSET(server_sock, &readfds)) {
            addr_size = sizeof(client_addr);
            int new_client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
            if (new_client_sock < 0) {
                perror("[-]Accept error");
                exit(1);
            }

            if (num_clients >= MAX_CLIENTS) {
                printf("Maximum number of clients reached. Rejecting new connection.\n");
                // Inform the client that maximum clients reached
                send(new_client_sock, "Maximum number of clients reached. Try again later.\n", strlen("Maximum number of clients reached. Try again later.\n"), 0);
                close(new_client_sock);
            } else {
                printf("[+]New connection, socket fd is %d, ip is : %s, port : %d\n",
                       new_client_sock, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                // Add new client socket to array of sockets
                for (i = 0; i < MAX_CLIENTS; ++i) {
                    if (client_data[i].socket == 0) {
                        client_data[i].socket = new_client_sock;
                        client_data[i].is_faq = 0;
                        client_data[i].address = client_addr;
                        uuid_t uuid;
                        uuid_generate_random(uuid);
                        uuid_unparse_lower(uuid, client_data[i].id);

                        // Send welcome message with the generated ID
                        send_welcome_message(&client_data[i]);
                        num_clients++;
                        break;
                    }
                }

                FD_SET(new_client_sock, &master);
                if (new_client_sock > max_sd) {
                    max_sd = new_client_sock;
                }
            }
        }

        // Handle client activity
        for (i = 0; i < MAX_CLIENTS; ++i) {
            int sd = client_data[i].socket;
            if (FD_ISSET(sd, &readfds)) {
                memset(buffer, 0, BUFFER_SIZE);
                valread = read(sd, buffer, sizeof(buffer));
                if (valread == 0) {
                    // Disconnect
                    // getpeername(sd, (struct sockaddr *)&client_addr, &addr_size);
                    printf("[-]Client (%s) disconnected from, ip %s, port %d\n", client_data[i].id,
                           inet_ntoa(client_data[i].address.sin_addr), ntohs(client_data[i].address.sin_port));
                    close(sd);
                    FD_CLR(sd, &master);

                    client_data[i].socket = 0;
                    client_data[i].is_faq = 0;
                    strcpy(client_data[i].id, "");
                    num_clients--;
                } else {
                    // Echo message to other clients
                    buffer[valread] = '\0';
                    printf("[+]Client (%s) requesting: %s\n", client_data[i].id, buffer);
                    // for (int j = 0; j < MAX_CLIENTS; ++j) {
                    //     if (client_data[j].socket != 0 && client_data[j].socket != sd) {
                    // send(sd, buffer, strlen(buffer), 0);
                    //     }
                    // }
                    client_handler(&client_data[i], buffer);
                }
            }
        }
    }

    close(server_sock);
    return 0;
}
