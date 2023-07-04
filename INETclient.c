#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SERVER_IP "127.0.0.1"
#define PORT 5678
#define BUFFER_SIZE 9999

// Function to get the size of a file
long get_file_size(FILE *file)
{
    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    rewind(file);
    return size;
}

void view_images(int sock)
{
    // Send request to the server
    char request[] = "3";
    write(sock, request, strlen(request));

    // Receive the number of images from the server
    int num_images;
    if (recv(sock, &num_images, sizeof(num_images), 0) == -1)
    {
        perror("recv");
        return;
    }

    // Receive and print the image names from the server
    printf("Image names:\n");
    char image_name[BUFFER_SIZE];
    for (int i = 0; i < num_images; i++)
    {
        memset(image_name, 0, sizeof(image_name)); // Clear the image_name buffer

        int bytes_received = 0;
        int total_bytes_received = 0;
        while (total_bytes_received < sizeof(image_name) - 1)
        {
            bytes_received = recv(sock, image_name + total_bytes_received, 1, 0);
            if (bytes_received > 0)
            {
                if (image_name[total_bytes_received] == '\0')
                {
                    break;
                }

                total_bytes_received += bytes_received;
            }
            else if (bytes_received == 0)
            {
                fprintf(stdout, "No more data available from the server.\n");
                break;
            }
            else
            {
                perror("recv");
                break;
            }
        }

        if (bytes_received > 0)
        {
            fprintf(stdout, "- %s\n", image_name);
        }
        else
        {
            break;
        }
    }
}

void view_logos(int sock)
{
    // Send request to the server
    char request[] = "4";
    write(sock, request, strlen(request));

    // Receive the number of logos from the server
    int num_logos;
    if (read(sock, &num_logos, sizeof(num_logos)) == -1)
    {
        perror("read");
        return;
    }

    printf("Number of logos: %d\n", num_logos);

    // Receive and print the logo names from the server
    printf("Logo names:\n");
    char logo_name[BUFFER_SIZE];
    for (int i = 0; i < num_logos; i++)
    {
        memset(logo_name, 0, sizeof(logo_name)); // Clear the logo_name buffer

        int bytes_received = 0;
        int total_bytes_received = 0;
        while (total_bytes_received < sizeof(logo_name) - 1)
        {
            bytes_received = recv(sock, logo_name + total_bytes_received, 1, 0);
            if (bytes_received > 0)
            {
                if (logo_name[total_bytes_received] == '\0')
                {
                    break;
                }

                total_bytes_received += bytes_received;
            }
            else if (bytes_received == 0)
            {
                fprintf(stdout, "No more data available from the server.\n");
                break;
            }
            else
            {
                perror("recv");
                break;
            }
        }

        if (bytes_received > 0)
        {
            fprintf(stdout, "- %s\n", logo_name);
        }
        else
        {
            break;
        }
    }
}

void create_watermark(int sock)
{
    // Send request to the server
    char request[] = "5";
    write(sock, request, strlen(request));

    // Receive the number of images from the server
    int num_images;
    if (recv(sock, &num_images, sizeof(num_images), 0) == -1)
    {
        perror("recv");
        return;
    }

    // Receive and print the image names from the server
    printf("Image names:\n");
    char image_name[BUFFER_SIZE];
    for (int i = 0; i < num_images; i++)
    {
        memset(image_name, 0, sizeof(image_name)); // Clear the image_name buffer

        int bytes_received = 0;
        int total_bytes_received = 0;
        while (total_bytes_received < sizeof(image_name) - 1)
        {
            bytes_received = recv(sock, image_name + total_bytes_received, 1, 0);
            if (bytes_received > 0)
            {
                if (image_name[total_bytes_received] == '\0')
                {
                    break;
                }

                total_bytes_received += bytes_received;
            }
            else if (bytes_received == 0)
            {
                fprintf(stdout, "No more data available from the server.\n");
                break;
            }
            else
            {
                perror("recv");
                break;
            }
        }

        if (bytes_received > 0)
        {
            fprintf(stdout, "- %s\n", image_name);
        }
        else
        {
            break;
        }
    }
    char image_choice[BUFFER_SIZE];
    printf("Enter the image choice: ");
    scanf("%s", image_choice);
    getchar(); // Consume the newline character
    printf("image choice: %s\n", image_choice);
    // Send the image choice to the server
    write(sock, image_choice, strlen(image_choice) + 1);

    // Receive the number of logos from the server
    int num_logos;
    if (read(sock, &num_logos, sizeof(num_logos)) == -1)
    {
        perror("read");
        return;
    }

    printf("Number of logos: %d\n", num_logos);

    // Receive and print the logo names from the server
    printf("Logo names:\n");
    char logo_name[BUFFER_SIZE];
    for (int i = 0; i < num_logos; i++)
    {
        memset(logo_name, 0, sizeof(logo_name)); // Clear the logo_name buffer

        int bytes_received = 0;
        int total_bytes_received = 0;
        while (total_bytes_received < sizeof(logo_name) - 1)
        {
            bytes_received = recv(sock, logo_name + total_bytes_received, 1, 0);
            if (bytes_received > 0)
            {
                if (logo_name[total_bytes_received] == '\0')
                {
                    break;
                }

                total_bytes_received += bytes_received;
            }
            else if (bytes_received == 0)
            {
                fprintf(stdout, "No more data available from the server.\n");
                break;
            }
            else
            {
                perror("recv");
                break;
            }
        }

        if (bytes_received > 0)
        {
            fprintf(stdout, "- %s\n", logo_name);
        }
        else
        {
            break;
        }
    }

    char logo_choice[BUFFER_SIZE];
    printf("Enter the logo choice: ");
    scanf("%s", logo_choice);
    getchar(); // Consume the newline character
    printf("logo choice: %s\n", logo_choice);
    // Send the logo choice to the server
    write(sock, logo_choice, strlen(logo_choice) + 1);

    // Send position to the server
    char position[BUFFER_SIZE];
    printf("Position: \n");
    printf("up-left         up         up_right\n");
    printf("middle-left     middle     middle_right\n");
    printf("down-left       down       down_right\n");
    printf("Enter your choice: ");
    scanf("%s", position);
    getchar(); // Consume the newline character
    printf("Pos choice: %s\n", position);
    write(sock, position, strlen(position) + 1);
}

void view_watermarks(int sock)
{
    // Send request to the server
    char request[] = "6";
    write(sock, request, strlen(request));
    // Receive the number of images from the server
    int num_watermarks;
    read(sock, &num_watermarks, sizeof(num_watermarks));

    printf("Number of watermarks: %d\n", num_watermarks);

    // Receive and print the image names from the server
    printf("Watermarks names:\n");
    char watermark_name[BUFFER_SIZE];
    for (int i = 0; i < num_watermarks; i++)
    {
        memset(watermark_name, 0, sizeof(watermark_name)); // Clear the watermark_name buffer

        int bytes_received = 0;
        int total_bytes_received = 0;
        while (total_bytes_received < sizeof(watermark_name) - 1)
        {
            bytes_received = recv(sock, watermark_name + total_bytes_received, 1, 0);
            if (bytes_received > 0)
            {
                if (watermark_name[total_bytes_received] == '\0')
                {
                    break;
                }

                total_bytes_received += bytes_received;
            }
            else if (bytes_received == 0)
            {
                fprintf(stdout, "No more data available from the server.\n");
                break;
            }
            else
            {
                perror("recv");
                break;
            }
        }

        if (bytes_received > 0)
        {
            fprintf(stdout, "- %s\n", watermark_name);
        }
        else
        {
            break;
        }
    }

    char watermarks_choice[BUFFER_SIZE];
    printf("Enter the watermark choice: ");
    scanf("%s", watermarks_choice);
    getchar(); // Consume the newline character
    printf("image choice: %s\n", watermarks_choice);
    // Send the image choice to the server
    write(sock, watermarks_choice, strlen(watermarks_choice) + 1);

    // Receive the watermark size from the server
    off_t watermark_size;
    if (recv(sock, &watermark_size, sizeof(watermark_size), 0) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }

    // Ask the user to enter the path where the image will be saved
    char savePath[BUFFER_SIZE];
    printf("Enter the path where you want to save the image (default is current directory): ");
    fgets(savePath, sizeof(savePath), stdin);
    savePath[strcspn(savePath, "\n")] = '\0'; // Remove the trailing newline character

    // Use the provided savePath or the current directory as the default
    if (strlen(savePath) == 0)
    {
        strcpy(savePath, ".");
    }

    // Construct the file path for saving the image
    char outputPath[BUFFER_SIZE + 23];
    snprintf(outputPath, sizeof(outputPath), "%s/received_watermark.jpg", savePath);

    // Receive the watermark image from the server
    char buffer[BUFFER_SIZE];
    size_t bytes_received = 0;
    FILE *output_file = fopen(outputPath, "wb");
    if (output_file == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    while (bytes_received < watermark_size)
    {
        int bytes_to_receive = BUFFER_SIZE;
        if (watermark_size - bytes_received < BUFFER_SIZE)
        {
            bytes_to_receive = watermark_size - bytes_received;
        }

        int bytes = recv(sock, buffer, bytes_to_receive, 0);
        if (bytes == -1)
        {
            perror("recv");
            fclose(output_file);
            exit(EXIT_FAILURE);
        }

        fwrite(buffer, sizeof(char), bytes, output_file);
        bytes_received += bytes;
    }

    fclose(output_file);

    printf("Received watermark image from the server and saved it to: %s\n", outputPath);
}

int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char request[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    // Create client socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IP address from string to binary form
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Get client ID
    char client_id[100];
    printf("Enter client name: ");
    fgets(client_id, sizeof(client_id), stdin);
    client_id[strcspn(client_id, "\n")] = '\0';

    // Send client ID to the server
    write(sock, client_id, strlen(client_id));

    while (1)
    {

        
        // Display menu
        printf("\n--- MENU ---\n");
        printf("1. Add image\n");
        printf("2. Send logo\n");
        printf("3. View images\n");
        printf("4. View logos\n");
        printf("5. Create a watermark image\n");
        printf("6. View watermarks\n");
        printf("0. Exit\n");
        printf("Choose an option: ");
        fgets(request, sizeof(request), stdin);
        request[strcspn(request, "\n")] = '\0';

        if (strcmp(request, "0") == 0)
        {
            // Send request to the server
            write(sock, request, strlen(request));
            break; // Exit the loop and terminate the client
        }
        else if (strcmp(request, "1") == 0)
        {
            // Send request to the server
            write(sock, request, strlen(request));

            // Get image path from the user
            char image_path[BUFFER_SIZE];
            printf("Enter image path: ");
            fgets(image_path, sizeof(image_path), stdin);
            image_path[strcspn(image_path, "\n")] = '\0';

            // Extract the image name from the path
            char *image_name = strrchr(image_path, '/');
            if (image_name != NULL)
            {
                image_name++; // Skip the '/'
            }
            else
            {
                image_name = image_path; // Use the full path as the name
            }

            // Send image name to the server
            write(sock, image_name, strlen(image_name));

            // Open the image file
            FILE *file = fopen(image_path, "rb");
            if (file == NULL)
            {
                perror("Error opening file");
                continue;
            }

            // Get the file size
            long file_size = get_file_size(file);

            // Send the file size to the server
            write(sock, &file_size, sizeof(file_size));

            // Read and send the image file in chunks
            char buffer[BUFFER_SIZE];
            size_t read_length;
            while ((read_length = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0)
            {
                write(sock, buffer, read_length);
            }
            fclose(file);

            printf("Image sent to server\n");
        }
        else if (strcmp(request, "2") == 0)
        {
            // Send request to the server
            write(sock, request, strlen(request));

            // Get logo path from the user
            char logo_path[BUFFER_SIZE];
            printf("Enter logo path: ");
            fgets(logo_path, sizeof(logo_path), stdin);
            logo_path[strcspn(logo_path, "\n")] = '\0';

            // Extract the logo name from the path
            char *logo_name = strrchr(logo_path, '/');
            if (logo_name != NULL)
            {
                logo_name++; // Skip the '/'
            }
            else
            {
                logo_name = logo_path; // Use the full path as the name
            }

            // Send logo name to the server
            write(sock, logo_name, strlen(logo_name));

            // Open the logo file
            FILE *file = fopen(logo_path, "rb");
            if (file == NULL)
            {
                perror("Error opening file");
                continue;
            }

            // Get the logo size
            long logo_size = get_file_size(file);

            // Send the logo size to the server
            write(sock, &logo_size, sizeof(logo_size));

            // Read and send the logo file in chunks
            char buffer[BUFFER_SIZE];
            size_t read_length;
            while ((read_length = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0)
            {
                write(sock, buffer, read_length);
            }
            fclose(file);

            printf("Logo sent to server\n");
        }
        else if (strcmp(request, "3") == 0)
        {
            // View images
            view_images(sock);
        }
        else if (strcmp(request, "4") == 0)
        {
            // View logos
            view_logos(sock);
        }
        else if (strcmp(request, "5") == 0)
        {
            // create the watermark
            create_watermark(sock);
        }

        else if (strcmp(request, "6") == 0)
        {
            // View watermarks
            view_watermarks(sock);
        }
        else
        {
            printf("Invalid option\n");
        }
    }

    close(sock);
    return 0;
}
