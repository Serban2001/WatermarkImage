#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <pthread.h>
#include <dirent.h>

#define SOCKET_PATH "/tmp/admin_socket"
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

int serverSocket;
int adminConnected = 0;
pthread_t clientThread;
pthread_mutex_t adminMutex = PTHREAD_MUTEX_INITIALIZER;

void handleSignal(int signal)
{
    if (signal == SIGINT)
    {
        if (adminConnected)
        {
            printf("Admin disconnected\n");
            adminConnected = 0;
        }
        pthread_cancel(clientThread);
        close(serverSocket);
        unlink("/tmp/admin_socket");
        exit(0);
    }
}

void debugMessage(const char *message)
{
    printf("DEBUG: %s\n", message);
}

void sendMessage(int socket, const char *message)
{
    write(socket, message, strlen(message) + 1);
}

void receiveMessage(int socket, char *buffer, size_t bufferSize)
{
    ssize_t bytesRead = read(socket, buffer, bufferSize);
    if (bytesRead > 0)
    {
        buffer[bytesRead - 1] = '\0'; // Remove newline character
    }
}

void handleCommand(int clientSocket, const char *command)
{
    char response[BUFFER_SIZE];

    if (strcmp(command, "list_folders_images") == 0)
    {
        debugMessage("Opening Folder");

        // Open the main directory
        DIR *dir = opendir("images");
        if (dir == NULL)
        {
            strcpy(response, "Failed to open Images directory");
        }
        else
        {
            struct dirent *entry;
            strcpy(response, "List of folders:\n");
            // Read the directory entries and append the directory names to the response
            while ((entry = readdir(dir)) != NULL)
            {
                if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                { // Check if it's a directory and not "." or ".."
                    strcat(response, entry->d_name);
                    strcat(response, "\n");
                }
            }
            closedir(dir);
        }
    }
    else if (strcmp(command, "list_folders_logos") == 0)
    {
        debugMessage("Opening Folder");

        // Open the main directory
        DIR *dir = opendir("logos");
        if (dir == NULL)
        {
            strcpy(response, "Failed to open logos directory");
        }
        else
        {
            struct dirent *entry;
            strcpy(response, "List of folders:\n");
            // Read the directory entries and append the directory names to the response
            while ((entry = readdir(dir)) != NULL)
            {
                if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                { // Check if it's a directory and not "." or ".."
                    strcat(response, entry->d_name);
                    strcat(response, "\n");
                }
            }
            closedir(dir);
        }
    }

    else if (strcmp(command, "list_folders_watermarks") == 0)
    {
        debugMessage("Opening Folder");

        // Open the main directory
        DIR *dir = opendir("watermarks");
        if (dir == NULL)
        {
            strcpy(response, "Failed to open Watermarks directory");
        }
        else
        {
            struct dirent *entry;
            strcpy(response, "List of folders:\n");
            // Read the directory entries and append the directory names to the response
            while ((entry = readdir(dir)) != NULL)
            {
                if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                { // Check if it's a directory and not "." or ".."
                    strcat(response, entry->d_name);
                    strcat(response, "\n");
                }
            }
            closedir(dir);
        }
    }

    else if (strncmp(command, "list_images", 11) == 0)
    {
        // Retrieve the folder name
        char *folderName = strchr(command, ' ');
        if (folderName != NULL)
        {
            folderName++; // Move past the space character
            debugMessage("Opening Image Folder");

            // Open the folder specified by the client
            char folderPath[BUFFER_SIZE];
            sprintf(folderPath, "images/%s", folderName);
            DIR *dir = opendir(folderPath);
            if (dir == NULL)
            {
                strcpy(response, "Failed to open folder");
            }
            else
            {
                struct dirent *entry;
                strcpy(response, "List of images:\n");
                // Read the directory entries and append the file names to the response
                while ((entry = readdir(dir)) != NULL)
                {
                    if (entry->d_type == DT_REG)
                    { // Check if it's a regular file
                        strcat(response, entry->d_name);
                        strcat(response, "\n");
                    }
                }
                closedir(dir);
            }
        }
        else
        {
            strcpy(response, "Invalid command format");
        }
    }

    else if (strncmp(command, "list_logos", 10) == 0)
    {
        // Retrieve the folder name
        char *folderName = strchr(command, ' ');
        if (folderName != NULL)
        {
            folderName++; // Move past the space character
            debugMessage("Opening Logos Folder");

            // Open the folder specified by the client
            char folderPath[BUFFER_SIZE];
            sprintf(folderPath, "logos/%s", folderName);
            DIR *dir = opendir(folderPath);
            if (dir == NULL)
            {
                strcpy(response, "Failed to open folder");
            }
            else
            {
                struct dirent *entry;
                strcpy(response, "List of logos:\n");
                // Read the directory entries and append the file names to the response
                while ((entry = readdir(dir)) != NULL)
                {
                    if (entry->d_type == DT_REG)
                    { // Check if it's a regular file
                        strcat(response, entry->d_name);
                        strcat(response, "\n");
                    }
                }
                closedir(dir);
            }
        }
        else
        {
            strcpy(response, "Invalid command format");
        }
    }

    else if (strncmp(command, "list_watermarks", 15) == 0)
    {
        // Retrieve the folder name
        char *folderName = strchr(command, ' ');
        if (folderName != NULL)
        {
            folderName++; // Move past the space character
            debugMessage("Opening Watermarks Folder");

            // Open the folder specified by the client
            char folderPath[BUFFER_SIZE];
            sprintf(folderPath, "watermarks/%s", folderName);
            DIR *dir = opendir(folderPath);
            if (dir == NULL)
            {
                strcpy(response, "Failed to open folder");
            }
            else
            {
                struct dirent *entry;
                strcpy(response, "List of watermarks:\n");
                // Read the directory entries and append the file names to the response
                while ((entry = readdir(dir)) != NULL)
                {
                    if (entry->d_type == DT_REG)
                    { // Check if it's a regular file
                        strcat(response, entry->d_name);
                        strcat(response, "\n");
                    }
                }
                closedir(dir);
            }
        }
        else
        {
            strcpy(response, "Invalid command format");
        }
    }

    else if (strncmp(command, "delete_image ", 13) == 0)
    {
        printf("Delete\n");
        printf("Received command: %s\n", command);
        // Extract the image name to delete
        const char *imageNameWithExtension = command + 13;
        char imageName[BUFFER_SIZE];
        strncpy(imageName, imageNameWithExtension, strlen(imageNameWithExtension) - 4);
        imageName[strlen(imageNameWithExtension) - 4] = '\0';
        printf("%s \n", imageNameWithExtension);
        debugMessage("Deleting Image");

        // Delete the image
        char imagePath[BUFFER_SIZE];
        sprintf(imagePath, "images/%s", imageNameWithExtension);
        printf("Image Path: %s\n", imagePath);
        if (remove(imagePath) == 0)
        {
            sprintf(response, "The %s was deleted", imageNameWithExtension);
        }
        else
        {
            strcpy(response, "Failed to delete image");
        }
    }

    else if (strncmp(command, "delete_logo ", 12) == 0)
    {
        // Extract the logo name to delete
        const char *logoName = command + 12;

        debugMessage("Deleting Logo");

        // Delete the logo
        char logoPath[BUFFER_SIZE];
        sprintf(logoPath, "Logos/%s", logoName);
        if (remove(logoPath) == 0)
        {
            sprintf(response, "The %s was deleted", logoName);
        }
        else
        {
            strcpy(response, "Failed to delete logo");
        }
    }

    else if (strncmp(command, "delete_watermark ", 17) == 0)
    {
        // Extract the watermark name to delete
        const char *watermarkName = command + 17;

        debugMessage("Deleting Watermark");

        // Delete the watermark
        char watermarkPath[BUFFER_SIZE];
        sprintf(watermarkPath, "Watermarks/%s", watermarkName);
        if (remove(watermarkPath) == 0)
        {
            sprintf(response, "The %s was deleted", watermarkName);
        }
        else
        {
            strcpy(response, "Failed to delete watermark");
        }
    }
    else if (strncmp(command, "rename_image ", 13) == 0)
    {
        // Extract the old and new names for the image
        char commandCopy[BUFFER_SIZE];
        strcpy(commandCopy, command);
        const char *commandPtr = commandCopy + 13;
        const char *oldName = strtok((char *)commandPtr, " ");
        const char *newName = strtok(NULL, " ");

        debugMessage("Renaming Image");

        // Rename the image file
        char oldPath[BUFFER_SIZE];
        char newPath[BUFFER_SIZE];
        sprintf(oldPath, "Images/%s", oldName);
        sprintf(newPath, "Images/%s", newName);
        if (rename(oldPath, newPath) == 0)
        {
            sprintf(response, "The image %s was renamed to %s", oldName, newName);
        }
        else
        {
            strcpy(response, "Failed to rename image");
        }
    }
    else if (strncmp(command, "rename_logo ", 12) == 0)
    {
        // Extract the old and new names for the logo
        const char *commandPtr = command + 12;
        const char *oldName = strtok((char *)commandPtr, " ");
        const char *newName = strtok(NULL, " ");

        debugMessage("Renaming Logo");

        // Rename the logo file
        char oldPath[BUFFER_SIZE];
        char newPath[BUFFER_SIZE];
        sprintf(oldPath, "Logos/%s", oldName);
        sprintf(newPath, "Logos/%s", newName);

        printf("Old path: %s\n", oldPath);
        printf("New path: %s\n", newPath);
        if (rename(oldPath, newPath) == 0)
        {
            sprintf(response, "The logo %s was renamed to %s", oldName, newName);
        }
        else
        {
            strcpy(response, "Failed to rename logo");
        }
    }
    else if (strncmp(command, "rename_watermark ", 17) == 0)
    {
        // Extract the old and new names for the watermark
        const char *commandPtr = command + 17;
        const char *oldName = strtok((char *)commandPtr, " ");
        const char *newName = strtok(NULL, " ");

        debugMessage("Renaming Watermark");

        // Rename the watermark file
        char oldPath[BUFFER_SIZE];
        char newPath[BUFFER_SIZE];
        sprintf(oldPath, "Watermarks/%s", oldName);
        sprintf(newPath, "Watermarks/%s", newName);
        if (rename(oldPath, newPath) == 0)
        {
            sprintf(response, "The watermark %s was renamed to %s", oldName, newName);
        }
        else
        {
            strcpy(response, "Failed to rename watermark");
        }
    }
    // get the number of clients connected to the INET server with PORT 2345 and IP_ADRESS "127.0.0.1" and send it to the client
    // use popen function to get the output of the command
    else if (strncmp(command, "nr_clients", 10) == 0)
    {
        char command[100];
        sprintf(command, "sudo netstat -anp | grep 127.0.0.1:5678 | grep ESTABLISHED | grep './INETclient' | awk '{print $7}'");
        FILE *fp = popen(command, "r");
        char result[100];
        int nr_clients = 0;
        int client_pids[100]; // Array to store the client PIDs
        int i = 0;

        // Read the result line by line
        while (fgets(result, sizeof(result), fp) != NULL)
        {
            sscanf(result, "%d", &client_pids[i]); // Extract the client PID from the result
            nr_clients++;                          // Increment the count for each connection
            i++;
        }

        pclose(fp);

        // Build the response
        sprintf(response, "...The number of clients connected to the INET server - %d...\n", nr_clients);
        strcat(response, "List of client PIDs:\n");

        // Append each client PID to the response
        for (int j = 0; j < nr_clients; j++)
        {
            char client_pid[20];
            sprintf(client_pid, "client pid = %d", client_pids[j]);
            strcat(response, client_pid);
            strcat(response, "\n");
        }
    }

    else if (strncmp(command, "disconnect_client", 17) == 0)
    {
        int pid;
        sscanf(command, "%*s %d", &pid);

        // Execute the command to disconnect the client
        char disconnectCommand[100];
        sprintf(disconnectCommand, "sudo kill %d", pid);
        int status = system(disconnectCommand);

        // Send the response back to the client
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            sprintf(response, "Disconnected client with PID %d", pid);
        }
        else
        {
            sprintf(response, "Failed to disconnect client with PID %d", pid);
        }
    }
    else if (strcmp(command, "disconnect_server") == 0)
    {
        // Extract the PID of the INET server
        int server_pid = 0;
        char command[100];
        sprintf(command, "sudo netstat -anp | grep 127.0.0.1:5678 | grep ESTABLISHED | grep './INETserver' | awk '{print $7}'");
        FILE *fp = popen(command, "r");
        char result[100];

        // Read the result line by line
        if (fgets(result, sizeof(result), fp) != NULL)
        {
            sscanf(result, "%d", &server_pid);
        }

        pclose(fp);

        // Execute the command to disconnect the server
        if (server_pid > 0)
        {
            char disconnectCommand[100];
            sprintf(disconnectCommand, "sudo kill %d", server_pid);
            int status = system(disconnectCommand);

            // Send the response back to the client

            if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
            {
                sprintf(response, "Disconnected INET server with PID %d", server_pid);
            }
            else
            {
                sprintf(response, "Failed to disconnect INET server with PID %d", server_pid);
            }
        }
        else
        {

            sprintf(response, "INET server not found");
        }
    }
    else if (strcmp(command, "disconnect_server_UNIX") == 0)
    {
        close(serverSocket);
        unlink("/tmp/admin_socket");
        exit(0);
    }

    else
    {
        strcpy(response, "Unknown command");
    }

    // Send the response back to the client
    sendMessage(clientSocket, response);
}

void *clientHandler(void *socketPtr)
{
    int clientSocket = *((int *)socketPtr);
    free(socketPtr);

    while (adminConnected)
    {
        char command[BUFFER_SIZE];
        ssize_t bytesRead = recv(clientSocket, command, sizeof(command) - 1, 0);
        if (bytesRead <= 0)
        {
            if (bytesRead == 0)
                printf("Admin disconnected\n");
            else
                perror("Failed to receive command");

            adminConnected = 0;
            break;
        }

        command[bytesRead] = '\0';
        printf("Received command: %s\n", command);

        if (strcmp(command, "exit") == 0)
        {
            break;
        }

        // Handle the command
        handleCommand(clientSocket, command);
    }

    close(clientSocket);
    printf("...Admin socket closed\n");

    return NULL;
}

int main()
{
    // Register the signal handler for SIGINT
    signal(SIGINT, handleSignal);

    // Create a UNIX socket
    serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        perror("Failed to create server socket");
        exit(1);
    }

    // Set up the server address
    struct sockaddr_un serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sun_family = AF_UNIX;
    strncpy(serverAddress.sun_path, "/tmp/admin_socket", sizeof(serverAddress.sun_path) - 1);

    // Bind the socket to the server address
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("Failed to bind server socket");
        exit(1);
    }

    // Listen for client connections
    if (listen(serverSocket, 1) == -1)
    {
        perror("Failed to listen for connections");
        exit(1);
    }

    printf("Server is listening on UNIX socket: /tmp/admin_socket\n");

    while (1)
    {
        // Accept a client connection
        int clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == -1)
        {
            perror("Failed to accept admin connection");
            continue;
        }

        pthread_mutex_lock(&adminMutex); // Acquire the lock

        if (adminConnected)
        {
            printf("Another client tried to connect. Rejecting the connection.\n");
            send(clientSocket, "rejected", sizeof("rejected"), 0); // Send rejection message
            close(clientSocket);
        }
        else
        {
            printf("...Admin connected\n");
            adminConnected = 1;
            send(clientSocket, "accepted", sizeof("accepted"), 0); // Send acceptance message

            int *socketPtr = malloc(sizeof(int));
            *socketPtr = clientSocket;

            // Create a thread to handle the client connection
            if (pthread_create(&clientThread, NULL, clientHandler, socketPtr) != 0)
            {
                perror("Failed to create client thread");
                break;
            }
        }

        pthread_mutex_unlock(&adminMutex); // Release the lock
    }

    close(serverSocket);
    unlink("/tmp/admin_socket");

    return 0;
}