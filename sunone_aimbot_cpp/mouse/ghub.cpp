#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

// Link the Windows Socket Library (Required for networking)
#pragma comment(lib, "ws2_32.lib")

#include "ghub.h"

// --- CONFIGURATION ---
// [IMPORTANT] Change this to the IP of your Gaming PC (PC 2) running the Python script
#define SERVER_IP "192.168.1.100" 
#define SERVER_PORT 5000

// Internal variables for networking
SOCKET udpSocket = INVALID_SOCKET;
sockaddr_in destAddr;
bool networkInitialized = false;

// Helper to start the network connection once
void EnsureNetwork() {
    if (networkInitialized) return;

    WSADATA wsaData;
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "[Network] WSAStartup failed" << std::endl;
        return;
    }

    // Create UDP Socket (SOCK_DGRAM)
    udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET) {
        std::cerr << "[Network] Failed to create socket" << std::endl;
        return;
    }

    // Setup Destination Address
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &destAddr.sin_addr);

    networkInitialized = true;
    std::cout << "[Network] UDP Driver Initialized. Target: " << SERVER_IP << std::endl;
}

// Constructor: Initializes the network immediately
GhubMouse::GhubMouse()
{
    EnsureNetwork();
    gmok = true; // Set flag to true so the bot logic proceeds
}

// Destructor: Clean up the socket
GhubMouse::~GhubMouse()
{
    if (udpSocket != INVALID_SOCKET) {
        closesocket(udpSocket);
    }
    WSACleanup();
}

// --- CORE MOVEMENT FUNCTION ---
bool GhubMouse::mouse_xy(int x, int y)
{
    if (!networkInitialized) EnsureNetwork();
    
    // Optimization: Don't send packets if there is no movement
    if (x == 0 && y == 0) return true;

    // PACKET STRUCTURE: 4 bytes total
    // We pack two 16-bit integers (short) into a raw buffer.
    // This matches the Python struct.unpack('hh') format.
    short buffer[2];
    buffer[0] = (short)x;
    buffer[1] = (short)y;

    // Send UDP packet instantly (Fire and Forget)
    int result = sendto(udpSocket, (char*)buffer, sizeof(buffer), 0, (sockaddr*)&destAddr, sizeof(destAddr));
    
    return (result != SOCKET_ERROR);
}

// --- CLICK FUNCTIONS ---
bool GhubMouse::mouse_down(int key)
{
    if (!networkInitialized) EnsureNetwork();
    
    // Send "CLICK" command
    // Note: The Python script currently treats "CLICK" as a full down+up click.
    const char* msg = "CLICK";
    sendto(udpSocket, msg, 5, 0, (sockaddr*)&destAddr, sizeof(destAddr));
    
    return true;
}

bool GhubMouse::mouse_up(int key)
{
    // Since "CLICK" handled everything, we do nothing here.
    // If you want separate Down/Up events, you need to update the Python server 
    // to handle "DOWN" and "UP" strings separately.
    return true;
}

bool GhubMouse::mouse_close()
{
    if (udpSocket != INVALID_SOCKET) {
        closesocket(udpSocket);
        udpSocket = INVALID_SOCKET;
        networkInitialized = false;
    }
    return true;
}
