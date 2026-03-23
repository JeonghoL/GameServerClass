#include <iostream>
#include <WS2tcpip.h>
using namespace std;
#pragma comment(lib, "ws2_32.lib")

constexpr short SERVER_PORT = 3000;
constexpr int BUFFER_SIZE = 4096;

int main()
{
	wcout.imbue(locale("korean"));
	WSADATA wsa_data{};
	WSAStartup(MAKEWORD(2, 2), &wsa_data);
	SOCKET s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);
	SOCKADDR_IN server_addr{};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(s_socket, reinterpret_cast<SOCKADDR*>(&server_addr), sizeof(server_addr));
	listen(s_socket, SOMAXCONN);
	INT addr_len = sizeof(server_addr);
	SOCKET c_socket = WSAAccept(s_socket, reinterpret_cast<SOCKADDR*>(&server_addr), &addr_len, nullptr, 0);

	for (;;) {
		char recv_buffer[BUFFER_SIZE];
		WSABUF recv_wsa_buf{ BUFFER_SIZE, recv_buffer };
		DWORD recv_size = 0;
		DWORD recv_flag = 0;
		WSARecv(c_socket, &recv_wsa_buf, 1, &recv_size, &recv_flag, nullptr, nullptr);
		
		cout << "Received from client: " << recv_buffer;
		cout << ", SIZE: " << recv_size << endl;

		DWORD sent_size = 0;
		WSABUF send_wsa_buf = { recv_size, recv_buffer };
		WSASend(c_socket, &send_wsa_buf, 1, &sent_size, 0, nullptr, nullptr);

		cout << recv_size << " Sent to client\n";
	}
	WSACleanup();
}
