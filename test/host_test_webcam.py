import socket
import numpy as np
import cv2
import struct

# Server configuration
HOST = "10.0.0.229"  # Listen on all interfaces
PORT = 5000       # Must match the client's port

# Create and bind socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((HOST, PORT))
server_socket.listen(1)
print(f"Listening on {HOST}:{PORT}...")

conn, addr = server_socket.accept()
print(f"Connected by {addr}")

while True:
    # Receive image size (first 4 bytes)
    data = conn.recv(4)
    if not data:
        break
    
    img_size = struct.unpack(">I", data)[0]  # Convert from bytes to int
    print(f"Receiving {img_size} bytes...")

    # Receive image data
    img_data = b""
    while len(img_data) < img_size:
        packet = conn.recv(img_size - len(img_data))
        if not packet:
            break
        img_data += packet

    # Convert received bytes to numpy array
    img_array = np.frombuffer(img_data, dtype=np.uint8)
    img = cv2.imdecode(img_array, cv2.IMREAD_COLOR)

    if img is not None:
        cv2.imshow("Received Image", img)
        cv2.waitKey(0)  # Wait for a key press
        cv2.destroyAllWindows()  # Close the window
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    else:
        print("Failed to decode image")

conn.close()
server_socket.close()
cv2.destroyAllWindows()
