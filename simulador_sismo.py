import socket
import struct

UDP_IP = "192.168.0.102"
UDP_PORT = 5000

print(f"Enviando alerta estructurada a {UDP_IP}:{UDP_PORT}...")

# Empaquetar dos enteros sin signo de 8 bits ('B' = unsigned char en C)
# Ejemplo: Estación ID = 1, Intensidad = 3
paquete_datos = struct.pack('BB', 1, 3) 

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.sendto(paquete_datos, (UDP_IP, UDP_PORT))
sock.close()

print("¡Paquete estructurado enviado!")