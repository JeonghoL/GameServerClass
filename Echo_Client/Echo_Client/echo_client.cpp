#include <iostream>
#include <WS2tcpip.h>
using namespace std;
#pragma comment(lib, "ws2_32.lib")

const char* SERVER_IP = "127.0.0.1";
constexpr short SERVER_PORT = 3000;
constexpr int BUFFER_SIZE = 4096;

void error_display(const wchar_t* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	wcout << msg;
	wcout << L" === Error " << lpMsgBuf << endl;
	while (true); // 디버깅 용
	LocalFree(lpMsgBuf);
}

int main()
{
	wcout.imbue(locale("korean"));
	WSADATA wsa_data{};
	WSAStartup(MAKEWORD(2, 2), &wsa_data);
	SOCKET s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);
	SOCKADDR_IN server_addr{};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);
	int result = WSAConnect(s_socket, reinterpret_cast<SOCKADDR*>(&server_addr), sizeof(server_addr), nullptr, nullptr, nullptr, nullptr);

	if (result == SOCKET_ERROR) {
		error_display(L"Cannot connect to server", WSAGetLastError());
		return 1;
	}

	for (;;) {
		char buffer[BUFFER_SIZE];
		cout << "Enter message to send: ";
		cin.getline(buffer, BUFFER_SIZE);

		WSABUF wsa_buf{ static_cast<ULONG>(strlen(buffer) + 1), buffer };
		DWORD sent_size = 0;
		int result = WSASend(s_socket, &wsa_buf, 1, &sent_size, 0, nullptr, nullptr);
		if (result == SOCKET_ERROR) {
			error_display(L"Failed to send data", WSAGetLastError());
			return 1;
		}

		char recv_buffer[BUFFER_SIZE]{};
		WSABUF recv_wsa_buf{ BUFFER_SIZE, recv_buffer };
		DWORD recv_size = 0;
		DWORD recv_flag = 0;
		WSARecv(s_socket, &recv_wsa_buf, 1, &recv_size, &recv_flag, nullptr, nullptr);

		cout << "Received from server: " << recv_buffer;
		cout << ", SIZE: " << recv_size << endl;
	}
	WSACleanup();
}
