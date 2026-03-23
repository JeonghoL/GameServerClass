#include <iostream>
#include <WS2tcpip.h>
using namespace std;
#pragma comment(lib, "ws2_32.lib")

const char* SERVER_IP = "127. 0. 0. 1";
constexpr short SERVER_PORT = 54000;
constexpr int BUFFER_SIZE = 4096;

int main()
{
	wcout.imbue(locale("korean"));
	WSAStartup(MAKEWORD(2, 2), nullptr);
	SOCKET s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
	SOCKADDR_IN server_addr{};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);
	WSAConnect(s_socket, reinterpret_cast<SOCKADDR*>(&server_addr), sizeof(server_addr), nullptr, nullptr, nullptr, nullptr);

	for (;;) {
		char buffer[BUFFER_SIZE];
		cout << "Enter message to send: ";
		cin.getline(buffer, BUFFER_SIZE);

		WSABUF wsa_buf{ strlen(buffer) + 1, buffer };
		DWORD sent_size = 0;
		WSASend(s_socket, &wsa_buf, 1, &sent_size, 0, nullptr, nullptr);

		char recv_buffer[BUFFER_SIZE]{};
		WSABUF recv_wsa_buf{ BUFFER_SIZE, recv_buffer };
		DWORD recv_size = 0;
		DWORD recv_flag = 0;
		WSARecv(s_socket, &recv_wsa_buf, 1, &recv_size, &recv_flag, nullptr, nullptr);

		cout << "Received from server: " << recv_buffer << endl;
		cout << ", SIZE: " << recv_size << endl;
	}
}
