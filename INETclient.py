import socket
import struct
import os

BUFFER_SIZE = 1024
PORT = 5678
SERVER_IP = "172.20.10.8"


def get_file_size(file):
    file.seek(0, os.SEEK_END)
    size = file.tell()
    file.seek(0, os.SEEK_SET)
    return size


def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        # Create client socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        serv_addr = (SERVER_IP, PORT)

        # Connect to the server
        sock.connect(serv_addr)

        # Get client ID
        client_id = input("Enter client name: ")

        # Send client ID to the server
        sock.send(client_id.encode())

        while True:
            # Display menu
            print("\n--- MENU ---")
            print("1. Add image")
            print("2. Send logo")
            print("3. View images")
            print("4. View logos")
            print("5. Create a watermark image")
            print("6. View watermarks")
            print("0. Exit")

            request = input("Choose an option: ")

            if request == "0":
                # Send request to the server
                sock.send(request.encode())
                break  # Exit the loop and terminate the client
            elif request == "1":
                # Send request to the server
                sock.send(request.encode())

                # Get image path from the user
                image_path = input("Enter image path: ")

                # Extract the image name from the path
                image_name = image_path.split("/")[-1]

                # Send image name to the server
                sock.send(image_name.encode())

                # Open the image file
                try:
                    file = open(image_path, "rb")
                except IOError:
                    print("Error opening file")
                    continue

                # Get the file size
                file_size = get_file_size(file)

                # Send the file size to the server
                sock.send(struct.pack("l", file_size))

                # Read and send the image file in chunks
                while True:
                    data = file.read(BUFFER_SIZE)
                    if not data:
                        break
                    sock.send(data)

                file.close()

                print("Image sent to server")
            elif request == "2":
                # Send request to the server
                sock.send(request.encode())

                # Get logo path from the user
                logo_path = input("Enter logo path: ")

                # Extract the logo name from the path
                logo_name = logo_path.split("/")[-1]

                # Send logo name to the server
                sock.send(logo_name.encode())

                # Open the logo file
                try:
                    file = open(logo_path, "rb")
                except IOError:
                    print("Error opening file")
                    continue

                # Get the logo size
                logo_size = get_file_size(file)

                # Send the logo size to the server
                sock.send(struct.pack("l", logo_size))

                # Read and send the logo file in chunks
                while True:
                    data = file.read(BUFFER_SIZE)
                    if not data:
                        break
                    sock.send(data)

                file.close()

                print("Logo sent to server")
            elif request == "3":
                # View images
                view_images(sock)
            elif request == "4":
                # View logos
                view_logos(sock)
            elif request == "5":
                # Create a watermark image
                create_watermark(sock)
            elif request == "6":
                # View watermarks
                view_watermarks(sock)
            else:
                print("Invalid option")

    finally:
        sock.close()


def view_images(sock):
    # Send request to the server
    request = "3"
    sock.send(request.encode())

    # Receive the number of images from the server
    num_images_data = sock.recv(4)
    num_images = struct.unpack("i", num_images_data)[0]

    # Receive and print the image names from the server
    print("Image names:")
    for _ in range(num_images):
        image_name = ""
        while True:
            data = sock.recv(1).decode()
            if data == "\0":
                break
            image_name += data

        print("- " + image_name)


def view_logos(sock):
    # Send request to the server
    request = "4"
    sock.send(request.encode())

    # Receive the number of logos from the server
    num_logos_data = sock.recv(4)
    num_logos = struct.unpack("i", num_logos_data)[0]

    print("Number of logos:", num_logos)

    # Receive and print the logo names from the server
    print("Logo names:")
    for _ in range(num_logos):
        logo_name = ""
        while True:
            data = sock.recv(1).decode()
            if data == "\0":
                break
            logo_name += data

        print("- " + logo_name)


def create_watermark(sock):
    # Send request to the server
    request = "5"
    sock.send(request.encode())

    # Receive the number of images from the server
    num_images_data = sock.recv(4)
    num_images = struct.unpack("i", num_images_data)[0]

    print("Image names:")
    for _ in range(num_images):
        image_name = ""
        while True:
            data = sock.recv(1).decode()
            if data == "\0":
                break
            image_name += data

        print("- " + image_name)

    image_choice = input("Enter the image choice: ")
    sock.send(image_choice.encode())

    # Receive the number of logos from the server
    num_logos_data = sock.recv(4)
    num_logos = struct.unpack("i", num_logos_data)[0]

    print("Number of logos:", num_logos)

    print("Logo names:")
    for _ in range(num_logos):
        logo_name = ""
        while True:
            data = sock.recv(1).decode()
            if data == "\0":
                break
            logo_name += data

        print("- " + logo_name)

    logo_choice = input("Enter the logo choice: ")
    sock.send(logo_choice.encode())

    position = input(
        "Position:\n"
        "up-left         up         up_right\n"
        "middle-left     middle     middle_right\n"
        "down-left       down       down_right\n"
        "Enter your choice: "
    )
    sock.send(position.encode())


def view_watermarks(sock):
    # Send request to the server
    request = "6"
    sock.send(request.encode())

    # Receive the number of watermarks from the server
    num_watermarks_data = sock.recv(4)
    num_watermarks = struct.unpack("i", num_watermarks_data)[0]

    print("Number of watermarks:", num_watermarks)

    # Receive and print the watermark names from the server
    print("Watermarks names:")
    for _ in range(num_watermarks):
        watermark_name = ""
        while True:
            data = sock.recv(1).decode()
            if data == "\0":
                break
            watermark_name += data

        print("- " + watermark_name)

    watermarks_choice = input("Enter the watermark choice: ")
    sock.send(watermarks_choice.encode())

    # Receive the watermark size from the server
    watermark_size_data = sock.recv(8)
    watermark_size = struct.unpack("q", watermark_size_data)[0]

    savePath = input(
        "Enter the path where you want to save the image (default is current directory): "
    )
    savePath = savePath.strip()
    if len(savePath) == 0:
        savePath = "."

    outputPath = f"{savePath}/received_watermark.jpg"

    with open(outputPath, "wb") as output_file:
        bytes_received = 0
        while bytes_received < watermark_size:
            bytes_to_receive = min(4096, watermark_size - bytes_received)
            data = sock.recv(bytes_to_receive)
            if not data:
                break
            output_file.write(data)
            bytes_received += len(data)

    print("Received watermark image from the server and saved it to:", outputPath)


if __name__ == "__main__":
    main()
