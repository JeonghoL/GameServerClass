#include <iostream>
#include <WS2tcpip.h>
#pragma comment(lib, "WS2_32.lib")
using namespace std;

constexpr short SERVER_PORT = 3000;
constexpr int BUFFER_SIZE = 4096;

char g_recv_buffer[BUFFER_SIZE];
char g_send_buffer[BUFFER_SIZE];
WSABUF g_recv_wsa_buf{ BUFFER_SIZE, g_recv_buffer };
WSABUF g_send_wsa_buf{ BUFFER_SIZE, g_send_buffer };
WSAOVERLAPPED g_recv_overlapped{}, g_send_overlapped{};
SOCKET g_s_socket;

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

void CALLBACK send_callback(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

void send_to_client(int size)
{

	g_send_wsa_buf.len = size;
	memcpy(g_send_buffer, g_recv_buffer, size);
	ZeroMemory(&g_send_overlapped, sizeof(g_send_overlapped));
	DWORD sent_size = 0;
	int result = WSASend(g_s_socket, &g_send_wsa_buf, 1, &sent_size, 0, &g_send_overlapped, send_callback);
	if (result == SOCKET_ERROR) {
		error_display(L"Failed to send data", WSAGetLastError());
		exit(1);
	}
}

void CALLBACK recv_callback(DWORD error, DWORD bytes_transferred, LPWSAOVERLAPPED overlapped, DWORD flags)
{
	if (error != 0) {
		cout << "Connection with the client has been lost.\n" << endl;
		closesocket(g_s_socket);
		return;
	}

	int len = bytes_transferred;
	if (len >= BUFFER_SIZE) len = BUFFER_SIZE - 1;
	g_recv_buffer[len] = '\0';

	cout << "Received from CLIENT: SIZE: " << bytes_transferred
		<< " MESSAGE : " << g_recv_buffer << endl;

	send_to_client(bytes_transferred);
}

void do_recv()
{
	DWORD recv_flag = 0;
	ZeroMemory(&g_recv_overlapped, sizeof(g_recv_overlapped));
	int result = WSARecv(g_s_socket, &g_recv_wsa_buf, 1, nullptr, &recv_flag, &g_recv_overlapped, recv_callback);
	if (result == SOCKET_ERROR) {
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING) {
			error_display(L"Failed to receive data", err_no);
			exit(1);
		}
	}
}

void CALLBACK send_callback(DWORD error, DWORD bytes_transferred, LPWSAOVERLAPPED overlapped, DWORD flags)
{
	if (error != 0) {
		error_display(L"Failed to send data", WSAGetLastError());
		return;
	}
	wcout << "Sent to client: SIZE: " << bytes_transferred << endl;
	do_recv();
}

int main()
{
	setlocale(LC_ALL, "korean");
	WSADATA wsa_data{};
	WSAStartup(MAKEWORD(2, 2), &wsa_data);
	SOCKET a_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr{};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(a_socket, reinterpret_cast<SOCKADDR*>(&server_addr), sizeof(server_addr));
	listen(a_socket, SOMAXCONN);
	INT addr_len = sizeof(server_addr);

	for (;;) {
		wcout << L"Waiting for client to connect...\n";
		g_s_socket = WSAAccept(a_socket, reinterpret_cast<SOCKADDR*>(&server_addr), &addr_len, nullptr, 0);
		wcout << L"Client Connected!!\n";
		if (g_s_socket == INVALID_SOCKET) {
			int err_no = WSAGetLastError();
			error_display(L"Failed to accept", err_no);
			exit(1);
		}
		do_recv();
	}
	WSACleanup();
}