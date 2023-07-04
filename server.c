#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

bool shouldRestart = true;
bool isSiteServerConnected = true;
bool isUXServerConnected = true;

void *runSiteServer(void *arg)
{
    // Command: Compile and run siteserver
    printf("Compiling and running siteserver...\n");
    system("gcc -o INETserver INETserver.c `pkg-config --cflags --libs MagickWand`");
    system("./INETserver");

    // If the INET server disconnects, set the flag to false
    isSiteServerConnected = false;

    pthread_exit(NULL);
}

void *runUXServer(void *arg)
{
    // Command: Remove admin_socket and compile and run UXserver
    printf("Removing admin_socket and compiling and running UXserver...\n");
    system("gcc -o UXserver UXserver.c");
    system("./UXserver");

    // If the UX server disconnects, set the flag to false
    isUXServerConnected = false;

    pthread_exit(NULL);
}

int main()
{
    pthread_t siteServerThread, uxServerThread;

    // Create the threads
    if (pthread_create(&siteServerThread, NULL, runSiteServer, NULL) != 0)
    {
        fprintf(stderr, "Failed to create siteserver thread\n");
        return 1;
    }
    if (pthread_create(&uxServerThread, NULL, runUXServer, NULL) != 0)
    {
        fprintf(stderr, "Failed to create UXserver thread\n");
        return 1;
    }
    

    // Set the restart flag to false to stop the threads
    shouldRestart = false;

    // Wait for the threads to finish
    pthread_join(siteServerThread, NULL);
    pthread_join(uxServerThread, NULL);

    // Check if both servers got disconnected
    if (!isSiteServerConnected && !isUXServerConnected)
    {
        printf("Both the INET server and the UX server got disconnected. Stopping the program.\n");
        return 1;
    }

    return 0;
}
