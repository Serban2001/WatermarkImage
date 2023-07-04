#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <dirent.h>
#include <wand/MagickWand.h>
#include <signal.h>
#include <unistd.h>


#define PORT 5678
#define BUFFER_SIZE 9999
#define MAX_CLIENTS 10

typedef struct
{
    int socket;
    char client_id[100];
} Client;

pthread_t threads[MAX_CLIENTS];
Client *clients[MAX_CLIENTS];
int num_clients = 0;

typedef enum
{
    POSITION_UP_LEFT,
    POSITION_UP,
    POSITION_UP_RIGHT,
    POSITION_MIDDLE_RIGHT,
    POSITION_MIDDLE,
    POSITION_MIDDLE_LEFT,
    POSITION_DOWN_LEFT,
    POSITION_DOWN,
    POSITION_DOWN_RIGHT,
    UNKNOWN
} WatermarkPosition;

WatermarkPosition convertPosition(const char *position)
{
    if (strcmp(position, "up_left") == 0)
        return POSITION_UP_LEFT;
    else if (strcmp(position, "up") == 0)
        return POSITION_UP;
    else if (strcmp(position, "up_right") == 0)
        return POSITION_UP_RIGHT;
    else if (strcmp(position, "middle_right") == 0)
        return POSITION_MIDDLE_RIGHT;
    else if (strcmp(position, "middle") == 0)
        return POSITION_MIDDLE;
    else if (strcmp(position, "middle_left") == 0)
        return POSITION_MIDDLE_LEFT;
    else if (strcmp(position, "down_left") == 0)
        return POSITION_DOWN_LEFT;
    else if (strcmp(position, "down") == 0)
        return POSITION_DOWN;
    else if (strcmp(position, "down_right") == 0)
        return POSITION_DOWN_RIGHT;
    else
        return UNKNOWN;
}

void addWatermark(const char *imagePath, const char *logoPath, WatermarkPosition watermarkPosition, const char *outputPath)
{
    MagickWand *image_wand = NULL;
    MagickWand *logo_wand = NULL;

    MagickWandGenesis();

    image_wand = NewMagickWand();
    logo_wand = NewMagickWand();

    if (MagickReadImage(image_wand, imagePath) == MagickFalse)
    {
        char *description;
        ExceptionType severity;
        description = MagickGetException(image_wand, &severity);
        fprintf(stderr, "Error loading image: %s\n", description);
        description = (char *)MagickRelinquishMemory(description);
        goto cleanup;
    }

    if (MagickReadImage(logo_wand, logoPath) == MagickFalse)
    {
        char *description;
        ExceptionType severity;
        description = MagickGetException(logo_wand, &severity);
        fprintf(stderr, "Error loading logo: %s\n", description);
        description = (char *)MagickRelinquishMemory(description);
        goto cleanup;
    }

    MagickScaleImage(logo_wand, MagickGetImageWidth(image_wand) * 0.2, MagickGetImageHeight(image_wand) * 0.2);

    // Increase transparency (decrease opacity)
    MagickEvaluateImageChannel(logo_wand, AlphaChannel, MultiplyEvaluateOperator, 0.5);

    // Set watermark position based on the provided watermarkPosition argument
    double posX = 0.0;
    double posY = 0.0;
    double imageWidth = (double)MagickGetImageWidth(image_wand);
    double imageHeight = (double)MagickGetImageHeight(image_wand);
    double logoWidth = (double)MagickGetImageWidth(logo_wand);
    double logoHeight = (double)MagickGetImageHeight(logo_wand);

    switch (watermarkPosition)
    {
    case POSITION_UP_LEFT:
        posX = 0.1 * imageWidth;
        posY = 0.1 * imageHeight;
        break;
    case POSITION_UP:
        posX = (imageWidth - logoWidth) / 2.0;
        posY = 0.1 * imageHeight;
        break;
    case POSITION_UP_RIGHT:
        posX = 0.9 * imageWidth - logoWidth;
        posY = 0.1 * imageHeight;
        break;
    case POSITION_MIDDLE_RIGHT:
        posX = 0.9 * imageWidth - logoWidth;
        posY = (imageHeight - logoHeight) / 2.0;
        break;
    case POSITION_MIDDLE:
        posX = (imageWidth - logoWidth) / 2.0;
        posY = (imageHeight - logoHeight) / 2.0;
        break;
    case POSITION_MIDDLE_LEFT:
        posX = 0.1 * imageWidth;
        posY = (imageHeight - logoHeight) / 2.0;
        break;
    case POSITION_DOWN_LEFT:
        posX = 0.1 * imageWidth;
        posY = 0.9 * imageHeight - logoHeight;
        break;
    case POSITION_DOWN:
        posX = (imageWidth - logoWidth) / 2.0;
        posY = 0.9 * imageHeight - logoHeight;
        break;
    case POSITION_DOWN_RIGHT:
        posX = 0.9 * imageWidth - logoWidth;
        posY = 0.9 * imageHeight - logoHeight;
        break;
    default:
        printf("Received unknown watermark position: %d\n", watermarkPosition);
        break;
    }

    MagickCompositeImage(image_wand, logo_wand, OverCompositeOp, posX, posY);

    if (MagickWriteImage(image_wand, outputPath) == MagickFalse)
    {
        printf("Error saving image: %s\n", outputPath);
        goto cleanup;
    }

    printf("Watermark added to image: %s\n", outputPath);

cleanup:
    if (image_wand)
    {
        image_wand = DestroyMagickWand(image_wand);
    }
    if (logo_wand)
    {
        logo_wand = DestroyMagickWand(logo_wand);
    }

    MagickWandTerminus();
}

void handle_client(Client *client)
{
    printf("Connected client: %s\n", client->client_id);

    // Create a folder for the client's images
    char images_folder[9999];
    snprintf(images_folder, sizeof(images_folder) - 1, "images/%s", client->client_id);
    images_folder[sizeof(images_folder) - 1] = '\0'; // Add null terminator to ensure string termination

    mkdir(images_folder, 0777);

    // Create a folder for the client's logos
    char logos_folder[9999];
    snprintf(logos_folder, sizeof(logos_folder), "logos/%s", client->client_id);
    mkdir(logos_folder, 0777);
    // Create a folder for the client's watermarks
    char watermarks_folder[9999];
    sprintf(watermarks_folder, "watermarks/%s", client->client_id);
    mkdir(watermarks_folder, 0777);
    char buffer[BUFFER_SIZE];
    while (1)
    {

        // Receive client request
        char request[BUFFER_SIZE];
        int request_length = recv(client->socket, request, BUFFER_SIZE, 0);
        if (request_length <= 0)
        {
            break;
        }
        request[request_length] = '\0';
        if (strcmp(request, "0") == 0)
        { // Exit
            break;
        }

        else if (strcmp(request, "1") == 0)
        { // Add image
            // Receive image name from the client
            char image_name[BUFFER_SIZE];
            int name_length = recv(client->socket, image_name, BUFFER_SIZE, 0);
            if (name_length <= 0)
            {
                break;
            }
            image_name[name_length] = '\0';
            printf("%s", image_name);

            // Receive image size from the client
            long image_size;
            if (recv(client->socket, &image_size, sizeof(image_size), 0) == -1)
            {
                perror("recv");
                continue;
            }

            // Receive image data from the client
            char image_path[BUFFER_SIZE];
            int result = snprintf(image_path, sizeof(image_path), "%s/%s", images_folder, image_name);
            if (result >= sizeof(image_path))
            {
                fprintf(stderr, "Increase the buffer size.\n");
            }

            FILE *file = fopen(image_path, "wb");
            if (file == NULL)
            {
                perror("Error opening file");
                continue;
            }

            long bytes_received = 0;
            while (bytes_received < image_size)
            {
                int bytes = recv(client->socket, buffer, BUFFER_SIZE, 0);
                if (bytes <= 0)
                {
                    perror("recv");
                    fclose(file);
                    break;
                }
                fwrite(buffer, sizeof(char), bytes, file);
                bytes_received += bytes;
            }

            fclose(file);

            printf("Image saved for client %s as %s\n", client->client_id, image_path);
        }

        else if (strcmp(request, "2") == 0)
        { // Send logo
            // Receive logo name from the client
            char logo_name[BUFFER_SIZE];
            int name_length = recv(client->socket, logo_name, BUFFER_SIZE, 0);
            if (name_length <= 0)
            {
                break;
            }
            logo_name[name_length] = '\0';

            // Receive logo size from the client
            long logo_size;
            if (recv(client->socket, &logo_size, sizeof(logo_size), 0) == -1)
            {
                perror("recv");
                continue;
            }

            char logo_path[BUFFER_SIZE];
            int result = snprintf(logo_path, sizeof(logo_path), "%s/%s", logos_folder, logo_name);
            if (result >= sizeof(logo_path))
            {
                fprintf(stderr, "Increase the buffer size.\n");
            }
            FILE *file = fopen(logo_path, "wb");
            if (file == NULL)
            {
                perror("Error opening file");
                continue;
            }

            long bytes_received = 0;
            while (bytes_received < logo_size)
            {
                int bytes = recv(client->socket, buffer, BUFFER_SIZE, 0);
                if (bytes <= 0)
                {
                    perror("recv");
                    fclose(file);
                    break;
                }
                fwrite(buffer, sizeof(char), bytes, file);
                bytes_received += bytes;
            }

            fclose(file);

            printf("Logo saved for client %s as %s\n", client->client_id, logo_path);
        }
        else if (strcmp(request, "3") == 0)
        {
            // Send list of images
            DIR *dir;
            struct dirent *entry;
            char images_path[9999];
            sprintf(images_path, "images/%s", client->client_id);

            if ((dir = opendir(images_path)) != NULL)
            {
                // Send the number of images in the folder to the client
                int num_images = 0;
                while ((entry = readdir(dir)) != NULL)
                {
                    if (entry->d_type == DT_REG)
                    {
                        num_images++;
                    }
                }
                closedir(dir);

                if (send(client->socket, &num_images, sizeof(num_images), 0) == -1)
                {
                    perror("send");
                    continue;
                }

                // Send each image name to the client
                dir = opendir(images_path);
                while ((entry = readdir(dir)) != NULL)
                {
                    if (entry->d_type == DT_REG)
                    {
                        char image_name[BUFFER_SIZE];
                        strcpy(image_name, entry->d_name);
                        image_name[strlen(entry->d_name)] = '\0';
                        if (write(client->socket, image_name, strlen(image_name) + 1) == -1)
                        {
                            perror("write");
                            break;
                        }
                        printf("Sent image name: %s\n", image_name);
                    }
                }
                closedir(dir);

                printf("Sent image list to client %s\n", client->client_id);
            }
            else
            {
                perror("opendir");
                continue;
            }
        }

        else if (strcmp(request, "4") == 0)
        {
            // Send list of logos
            DIR *dir;
            struct dirent *entry;
            char logos_path[9999];
            sprintf(logos_path, "logos/%s", client->client_id);

            if ((dir = opendir(logos_path)) != NULL)
            {
                // Send the number of logos in the folder to the client
                int num_logos = 0;
                while ((entry = readdir(dir)) != NULL)
                {
                    if (entry->d_type == DT_REG)
                    {
                        num_logos++;
                    }
                }
                closedir(dir);

                if (send(client->socket, &num_logos, sizeof(num_logos), 0) == -1)
                {
                    perror("send");
                    continue;
                }

                dir = opendir(logos_path);
                while ((entry = readdir(dir)) != NULL)
                {
                    if (entry->d_type == DT_REG)
                    {
                        char logo_name[BUFFER_SIZE];
                        strcpy(logo_name, entry->d_name);
                        logo_name[strlen(entry->d_name)] = '\0';
                        if (write(client->socket, logo_name, strlen(logo_name) + 1) == -1)
                        {
                            perror("write");
                            break;
                        }
                        printf("Sent logo name: %s\n", logo_name);
                    }
                }
                closedir(dir);

                printf("Sent logos list to client %s\n", client->client_id);
            }
            else
            {
                perror("opendir");
                continue;
            }
        }
        else if (strcmp(request, "5") == 0)
        {
            // Send list of images
            DIR *dir;
            struct dirent *entry;
            char images_path[9999];
            sprintf(images_path, "images/%s", client->client_id);
            if ((dir = opendir(images_path)) != NULL)
            {
                // Send the number of images in the folder to the client
                int num_images = 0;
                while ((entry = readdir(dir)) != NULL)
                {
                    if (entry->d_type == DT_REG)
                    {
                        num_images++;
                    }
                }
                closedir(dir);

                if (send(client->socket, &num_images, sizeof(num_images), 0) == -1)
                {
                    perror("send");
                    continue;
                }

                // Send each image name to the client
                dir = opendir(images_path);
                while ((entry = readdir(dir)) != NULL)
                {
                    if (entry->d_type == DT_REG)
                    {
                        char image_name[BUFFER_SIZE];
                        strcpy(image_name, entry->d_name);
                        image_name[strlen(entry->d_name)] = '\0';
                        if (send(client->socket, image_name, strlen(image_name) + 1, 0) == -1)
                        {
                            perror("send");
                            break;
                        }
                        printf("Sent image name: %s\n", image_name);
                    }
                }
                closedir(dir);

                printf("Sent image list to client %s\n", client->client_id);
            }
            else
            {
                perror("opendir");
                continue;
            }

            // Get the image choice from the client
            char image_choice[BUFFER_SIZE]; // Assuming BUFFER_SIZE is an appropriate size for storing the image name
            if (recv(client->socket, image_choice, sizeof(image_choice), 0) == -1)
            {
                perror("recv");
                continue;
            }
            image_choice[sizeof(image_choice) - 1] = '\0';

            printf("Image choice: %s\n", image_choice);

            // Send list of logos
            char logos_path[9999];
            sprintf(logos_path, "logos/%s", client->client_id);

            if ((dir = opendir(logos_path)) != NULL)
            {
                // Send the number of logos in the folder to the client
                int num_logos = 0;
                while ((entry = readdir(dir)) != NULL)
                {
                    if (entry->d_type == DT_REG)
                    {
                        num_logos++;
                    }
                }
                closedir(dir);

                if (send(client->socket, &num_logos, sizeof(num_logos), 0) == -1)
                {
                    perror("send");
                    continue;
                }

                dir = opendir(logos_path);

                // Send each logo name to the client
                while ((entry = readdir(dir)) != NULL)
                {
                    if (entry->d_type == DT_REG)
                    {
                        char logo_name[BUFFER_SIZE];
                        strcpy(logo_name, entry->d_name);
                        logo_name[strlen(entry->d_name)] = '\0'; // Null-terminate the logo name
                        if (send(client->socket, logo_name, strlen(logo_name) + 1, 0) == -1)
                        {
                            perror("send");
                            break;
                        }
                        printf("Sent logo name: %s\n", logo_name);
                    }
                }
                closedir(dir);

                printf("Sent logos list to client %s\n", client->client_id);
            }
            else
            {
                perror("opendir");
                continue;
            }

            // Get the logo choice from the client
            char logo_choice[BUFFER_SIZE]; // Assuming BUFFER_SIZE is an appropriate size for storing the logo name
            if (recv(client->socket, logo_choice, sizeof(logo_choice), 0) == -1)
            {
                perror("recv");
                continue;
            }
            logo_choice[sizeof(logo_choice) - 1] = '\0';

            printf("Logo choice: %s\n", logo_choice);

            // Receive position from the client
            char position[BUFFER_SIZE]; // Adjust the size according to your needs
            if (recv(client->socket, position, sizeof(position), 0) == -1)
            {
                perror("recv");
                continue;
            }
            position[sizeof(position) - 1] = '\0';
            // Convert position to WatermarkPosition enum
            WatermarkPosition watermarkPosition = convertPosition(position);

            // Print the received position
            printf("Received position: %s\n", position);
            printf("Watermark position: %d\n", watermarkPosition);

            // Get the file extension from the image choice
            char image_extension[10];
            const char *image_filename = strrchr(image_choice, '.');
            if (image_filename != NULL)
                strcpy(image_extension, image_filename);
            else
                strcpy(image_extension, ".jpg");

            // Get the file extension from the logo choice
            char logo_extension[10];
            const char *logo_filename = strrchr(logo_choice, '.');
            if (logo_filename != NULL)
                strcpy(logo_extension, logo_filename);
            else
                strcpy(logo_extension, ".jpg");

            // Remove the file extension from the image choice
            char image_name_without_extension[9999];
            if (image_filename != NULL)
                strncpy(image_name_without_extension, image_choice, image_filename - image_choice);
            else
                strcpy(image_name_without_extension, image_choice);
            image_name_without_extension[image_filename - image_choice] = '\0';

            // Remove the file extension from the logo choice
            char logo_name_without_extension[9999];
            if (logo_filename != NULL)
                strncpy(logo_name_without_extension, logo_choice, logo_filename - logo_choice);
            else
                strcpy(logo_name_without_extension, logo_choice);
            logo_name_without_extension[logo_filename - logo_choice] = '\0';

            // Call addWatermark function with the image path, logo path, and watermark position
            char image_path[99999];
            sprintf(image_path, "images/%s/%s", client->client_id, image_choice);
            char logo_path[99999];
            sprintf(logo_path, "logos/%s/%s", client->client_id, logo_choice);
            // Construct the watermark output path
            char watermark_output[99999];
            sprintf(watermark_output, "watermarks/%s/%s-%s%s", client->client_id, image_name_without_extension, logo_name_without_extension, logo_extension);

            // Call the addWatermark function with the updated watermark output path
            addWatermark(image_path, logo_path, watermarkPosition, watermark_output);
        }
        else if (strcmp(request, "6") == 0)
        {
            printf("opt6\n");
            // Send list of watermarks
            DIR *dir;
            struct dirent *entry;
            char watermarks_path[9999];
            sprintf(watermarks_path, "watermarks/%s", client->client_id);
            if ((dir = opendir(watermarks_path)) != NULL)
            {
                // Send the number of watermarks in the folder to the client
                int num_watermarks = 0;
                while ((entry = readdir(dir)) != NULL)
                {
                    if (entry->d_type == DT_REG)
                    {
                        num_watermarks++;
                    }
                }
                closedir(dir);

                if (send(client->socket, &num_watermarks, sizeof(num_watermarks), 0) == -1)
                {
                    perror("send");
                    continue;
                }

                // Send each watermark name to the client
                dir = opendir(watermarks_path);
                while ((entry = readdir(dir)) != NULL)
                {
                    if (entry->d_type == DT_REG)
                    {
                        char watermark_name[BUFFER_SIZE];
                        strcpy(watermark_name, entry->d_name);
                        watermark_name[strlen(entry->d_name)] = '\0';
                        if (send(client->socket, watermark_name, strlen(watermark_name) + 1, 0) == -1)
                        {
                            perror("send");
                            break;
                        }
                        printf("Sent watermark name: %s\n", watermark_name); // Debug print statement
                    }
                }
                closedir(dir);

                printf("Sent watermarks list to client %s\n", client->client_id);
            }
            else
            {
                perror("opendir");
                continue;
            }

            // Get the image choice from the client
            char watermark_choice[BUFFER_SIZE]; // Assuming BUFFER_SIZE is an appropriate size for storing the image name
            if (recv(client->socket, watermark_choice, sizeof(watermark_choice), 0) == -1)
            {
                perror("recv");
                continue;
            }
            watermark_choice[sizeof(watermark_choice) - 1] = '\0';

            printf("Watermark choice: %s\n", watermark_choice);

            // Construct the path to the watermark image
            char watermark_path[BUFFER_SIZE];
            int result = snprintf(watermark_path, sizeof(watermark_path), "watermarks/%s/%s", client->client_id, watermark_choice);
            if (result >= sizeof(watermark_path))
            {
                fprintf(stderr, "Increase the buffer size.\n");
            }

            // Open the watermark image file
            int watermark_fd = open(watermark_path, O_RDONLY);
            if (watermark_fd == -1)
            {
                perror("open");
                continue;
            }

            // Get the watermark image size
            struct stat st;
            if (fstat(watermark_fd, &st) == -1)
            {
                perror("fstat");
                close(watermark_fd);
                continue;
            }
            off_t watermark_size = st.st_size;

            // Send the watermark size to the client
            if (send(client->socket, &watermark_size, sizeof(watermark_size), 0) == -1)
            {
                perror("send");
                close(watermark_fd);
                continue;
            }

            // Send the watermark image to the client
            int bytes_read;
            while ((bytes_read = read(watermark_fd, buffer, BUFFER_SIZE)) > 0)
            {
                if (send(client->socket, buffer, bytes_read, 0) == -1)
                {
                    perror("send");
                    close(watermark_fd);
                    continue;
                }
            }

            printf("Sent watermark image to client %s\n", client->client_id);
        }
    }
    free(client);
    pthread_exit(NULL);
}

void disconnect_all_clients() {
    printf("Number of clients: %d\n", num_clients);
    for (int i = 0; i < num_clients; i++) {
        close(clients[i]->socket);
        free(clients[i]);
    }
    num_clients = 0;
}


void signal_handler(int sig)
{
    if (sig == SIGINT)
    {
        printf("\nServer terminating...\n");
        disconnect_all_clients();
        exit(0);
    }
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    signal(SIGINT, signal_handler);
    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the specified IP and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started. Waiting for clients...\n");

    while (1)
    {
        // Accept new client connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Create a new client object
        Client *client = (Client *)malloc(sizeof(Client));
        client->socket = new_socket;

        // Get client ID
        memset(client->client_id, 0, sizeof(client->client_id));
        read(new_socket, client->client_id, sizeof(client->client_id));

        // Create a new thread to handle the client
        pthread_create(&threads[num_clients], NULL, (void *)handle_client, (void *)client);
        clients[num_clients] = client;
        num_clients++;

        printf("Number of clients: %d\n", num_clients);

        if (num_clients >= MAX_CLIENTS)
        {
            // Maximum number of clients reached, wait for threads to finish
            for (int i = 0; i < num_clients; i++)
            {
                pthread_join(threads[i], NULL);
            }
            disconnect_all_clients();
        }
    }

    disconnect_all_clients();
    close(server_fd);
    disconnect_all_clients();
    return 0;
}