import socket
import struct

# 0.0.0.0 significa que la laptop escuchará en todas sus conexiones de red (incluyendo la IP 192.168.0.100)
UDP_IP = "0.0.0.0" 
UDP_PORT = 5000

# Crear el socket UDP y anclarlo al puerto
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print(f"💻 Servidor Central (Python) iniciado.")
print(f"Escuchando alertas en el puerto {UDP_PORT}...\n")

# Ciclo infinito para quedarse escuchando siempre
while True:
    # Esperar hasta recibir un paquete (se queda pausado aquí)
    data, addr = sock.recvfrom(1024)
    
    # Verificar que el paquete tenga exactamente los 2 bytes de nuestra estructura en C
    if len(data) == 2:
        # Desempaquetar los 2 bytes sin signo ('BB')
        station_id, intensity = struct.unpack('BB', data)
        
        print("-" * 40)
        print("🚨 ¡ALERTA SÍSMICA RECIBIDA! 🚨")
        print(f"📍 Origen IP: {addr[0]}")
        print(f"📡 Estación ID: {station_id}")
        print(f"⚠️ Nivel de Intensidad: {intensity}")
        print("-" * 40 + "\n")
    else:
        print(f"Paquete ignorado. Tamaño incorrecto: {len(data)} bytes de {addr[0]}")