#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define BUFFER_SIZE 9999
#define SOCKET_PATH "/tmp/admin_socket"

void sendCommand(int serverSocket, const char *command)
{
    write(serverSocket, command, strlen(command) + 1);
}

void receiveResponse(int serverSocket, char *response, int bufferSize)
{
    ssize_t bytesRead = read(serverSocket, response, bufferSize);
    if (bytesRead == -1)
    {
        perror("Failed to receive response");
        exit(1);
    }
}

void handleUserChoice(int serverSocket, int choice)
{
    char response[BUFFER_SIZE];

    switch (choice)
    {
    case 1:
    {
        sendCommand(serverSocket, "list_folders_images");
        receiveResponse(serverSocket, response, BUFFER_SIZE);
        printf("Server response:\n%s\n", response);

        char folderName[BUFFER_SIZE];
        printf("Enter the folder name to open: ");
        scanf("%s", folderName);
        getchar();

        char command[BUFFER_SIZE + 15]; // Increased size to accommodate longer strings
        sprintf(command, "list_images %s", folderName);

        sendCommand(serverSocket, command);
        receiveResponse(serverSocket, response, BUFFER_SIZE);
        printf("Server response:\n%s\n", response);

        char actionChoice[BUFFER_SIZE];
        printf("Do you want to delete or rename an image? (Delete/Rename): ");
        scanf("%s", actionChoice);
        getchar();

        if (strcasecmp(actionChoice, "Delete") == 0)
        {
            char imageName[BUFFER_SIZE];
            printf("Enter the image name to delete: ");
            scanf("%s", imageName);
            getchar();
            int maxFormattedLength = snprintf(NULL, 0, "delete_image %s/%s", folderName, imageName);
            int bufferLength = maxFormattedLength + 1; // Add 1 for the null terminator
            char deleteCommand[bufferLength];

            sprintf(deleteCommand, "delete_image %s/%s", folderName, imageName);

            sendCommand(serverSocket, deleteCommand);
            receiveResponse(serverSocket, response, BUFFER_SIZE);
            printf("Server response:\n%s\n", response);
        }
        else if (strcasecmp(actionChoice, "Rename") == 0)
        {
            char imageName[BUFFER_SIZE];
            printf("Enter the image name to rename: ");
            scanf("%s", imageName);
            getchar();
            char newName[BUFFER_SIZE];
            printf("Enter the new image name: ");
            scanf("%s", newName);
            getchar();
            int maxFormattedLength = snprintf(NULL, 0, "rename_image %s/%s %s/%s", folderName, imageName, folderName, newName);
            int bufferLength = maxFormattedLength + 1; // Add 1 for the null terminator
            char renameCommand[bufferLength];

            sprintf(renameCommand, "rename_image %s/%s %s/%s", folderName, imageName, folderName, newName);

            sendCommand(serverSocket, renameCommand);
            receiveResponse(serverSocket, response, BUFFER_SIZE);
            printf("Server response:\n%s\n", response);
        }

        break;
    }

    case 2:
    {

        sendCommand(serverSocket, "list_folders_logos");
        receiveResponse(serverSocket, response, BUFFER_SIZE);
        printf("Server response:\n%s\n", response);

        char folderName[BUFFER_SIZE];
        printf("Enter the folder name to open: ");
        scanf("%s", folderName);
        getchar();

        char command[BUFFER_SIZE + 20]; // Increased size to accommodate longer strings
        sprintf(command, "list_logos %s", folderName);

        sendCommand(serverSocket, command);
        receiveResponse(serverSocket, response, BUFFER_SIZE);
        printf("Server response:\n%s\n", response);

        char actionChoice[BUFFER_SIZE];
        printf("Do you want to delete or rename an image? (Delete/Rename): ");
        scanf("%s", actionChoice);
        getchar();

        if (strcasecmp(actionChoice, "Delete") == 0)
        {
            char imageName[BUFFER_SIZE];
            printf("Enter the image name to delete: ");
            scanf("%s", imageName);
            getchar();
            int maxFormattedLength = snprintf(NULL, 0, "delete_logo %s/%s", folderName, imageName);
            int bufferLength = maxFormattedLength + 1; // Add 1 for the null terminator
            char deleteCommand[bufferLength];

            sprintf(deleteCommand, "delete_logo %s/%s", folderName, imageName);

            sendCommand(serverSocket, deleteCommand);
            receiveResponse(serverSocket, response, BUFFER_SIZE);
            printf("Server response:\n%s\n", response);
        }
        else if (strcasecmp(actionChoice, "Rename") == 0)
        {
            char imageName[BUFFER_SIZE];
            printf("Enter the image name to rename: ");
            scanf("%s", imageName);
            getchar();
            char newName[BUFFER_SIZE];
            printf("Enter the new image name: ");
            scanf("%s", newName);
            getchar();
            int maxFormattedLength = snprintf(NULL, 0, "rename_logo %s/%s %s/%s", folderName, imageName, folderName, newName);
            int bufferLength = maxFormattedLength + 1; // Add 1 for the null terminator
            char renameCommand[bufferLength];

            sprintf(renameCommand, "rename_logo %s/%s %s/%s", folderName, imageName, folderName, newName);

            sendCommand(serverSocket, renameCommand);
            receiveResponse(serverSocket, response, BUFFER_SIZE);
            printf("Server response:\n%s\n", response);
        }

        break;
    }
    case 3:
    {
        sendCommand(serverSocket, "list_folders_watermarks");
        receiveResponse(serverSocket, response, BUFFER_SIZE);
        printf("Server response:\n%s\n", response);

        char folderName[BUFFER_SIZE];
        printf("Enter the folder name to open: ");
        scanf("%s", folderName);
        getchar();

        char command[BUFFER_SIZE + 23]; // Increased size to accommodate longer strings
        sprintf(command, "list_watermarks %s", folderName);

        sendCommand(serverSocket, command);
        receiveResponse(serverSocket, response, BUFFER_SIZE);
        printf("Server response:\n%s\n", response);

        char actionChoice[BUFFER_SIZE];
        printf("Do you want to delete or rename an image? (Delete/Rename): ");
        scanf("%s", actionChoice);
        getchar();

        if (strcasecmp(actionChoice, "Delete") == 0)
        {
            char imageName[BUFFER_SIZE];
            printf("Enter the image name to delete: ");
            scanf("%s", imageName);
            getchar();
            int maxFormattedLength = snprintf(NULL, 0, "delete_watermark %s/%s", folderName, imageName);
            int bufferLength = maxFormattedLength + 1; // Add 1 for the null terminator
            char deleteCommand[bufferLength];

            sprintf(deleteCommand, "delete_watermark %s/%s", folderName, imageName);

            sendCommand(serverSocket, deleteCommand);
            receiveResponse(serverSocket, response, BUFFER_SIZE);
            printf("Server response:\n%s\n", response);
        }
        else if (strcasecmp(actionChoice, "Rename") == 0)
        {
            char imageName[BUFFER_SIZE];
            printf("Enter the image name to rename: ");
            scanf("%s", imageName);
            getchar();
            char newName[BUFFER_SIZE];
            printf("Enter the new image name: ");
            scanf("%s", newName);
            getchar();
            int maxFormattedLength = snprintf(NULL, 0, "rename_watermark %s/%s %s/%s", folderName, imageName, folderName, newName);
            int bufferLength = maxFormattedLength + 1; // Add 1 for the null terminator
            char renameCommand[bufferLength];

            sprintf(renameCommand, "rename_watermark %s/%s %s/%s", folderName, imageName, folderName, newName);

            sendCommand(serverSocket, renameCommand);
            receiveResponse(serverSocket, response, BUFFER_SIZE);
            printf("Server response:\n%s\n", response);
        }

        break;
    }

    // request the nr of clients connected to the INET server
    case 4:
    {
        sendCommand(serverSocket, "nr_clients");
        receiveResponse(serverSocket, response, BUFFER_SIZE);
        printf("Server response:\n%s\n", response);

        char actionChoice;
        printf("Do you want to disconnect a client? (Y/N): ");
        scanf("%c", &actionChoice);
        getchar();

        if (actionChoice == 'Y' || actionChoice == 'y')
        {
            // Extract the PID from the response
            char pid[10];
            printf("Enter the PID of the client to disconnect: ");
            scanf("%s", pid);

            // Send the disconnect_client command to the server
            char disconnectCommand[100];
            sprintf(disconnectCommand, "disconnect_client %s", pid);
            sendCommand(serverSocket, disconnectCommand);

            // Receive and display the server response
            receiveResponse(serverSocket, response, BUFFER_SIZE);
            printf("Server response:\n%s\n", response);
        }
        break;
    }

    case 5:
    {
        sendCommand(serverSocket, "disconnect_server");
        receiveResponse(serverSocket, response, BUFFER_SIZE);
        printf("Server response:\n%s\n", response);
    }

    case 6:
    {
        sendCommand(serverSocket, "disconnect_server_UNIX");
        break;
        
    }

    case 0:
        printf("Exiting.\n");
        break;
    default:
        printf("Invalid choice. Please try again.\n");
        break;
    }
}


int main()
{
    int clientSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        perror("Failed to create socket");
        exit(1);
    }

    struct sockaddr_un serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sun_family = AF_UNIX;
    strcpy(serverAddress.sun_path, SOCKET_PATH);

    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("Failed to connect to server");
        exit(1);
    }

    // Receive connection status from the server
    char connectionStatus[10];
    ssize_t bytesRead = recv(clientSocket, connectionStatus, sizeof(connectionStatus) - 1, 0);
    if (bytesRead <= 0)
    {
        perror("Failed to receive connection status");
        exit(1);
    }
    connectionStatus[bytesRead] = '\0';

    if (strcmp(connectionStatus, "rejected") == 0)
    {
        printf("Another admin is already connected. Please try again later.\n");
        close(clientSocket);
        exit(0);
    }

    int choice;
    do
    {
        printf("\nMenu:\n");
        printf("1. Images\n");
        printf("2. Logos\n");
        printf("3. Watermarks\n");
        printf("4. INET clients connected\n");
        printf("5. Disconnect the INET server\n");
        printf("6. Disconnect the UNIX server\n");
        printf("0. Exit\n");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        getchar();

        handleUserChoice(clientSocket, choice);

    } while (choice != 0 && choice != 6);

    close(clientSocket);

    return 0;
}
