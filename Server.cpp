#define WIN32_LEAN_AND_MEAN // макрос

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
using namespace std;

int main()
{
	// служебная структура для хранение информации (ВЕРСИИ, СТРУКТУРЫ(НАПРИМЕР СЕМЕЙНУЮ)
   // о реализации Windows Sockets
	WSADATA wsaData;	// Структура для хранения данных Winsock API
	SOCKET ListenSocket = INVALID_SOCKET;	// Сокет для входящих соединений (используется для прослушивания новых соединений)
	SOCKET СonnectSocket = INVALID_SOCKET;	// Сокет для приема данных от подключенного клиента
	ADDRINFO hints;	// Структура для хранения сведений о доступных сетевых адресах и портов
	ADDRINFO* addrResult;// Указатель на список найденных сетевых адресов и портов

	const char* sendBuffer = "Hello from Server";	// Буфер для отправки данных клиенту
	char recvBuffer[512];// Буфер для приема данных от клиента
	char recvBuffer1[512];// Буфер для приема данных от клиента

	// старт использования библиотеки сокетов процессом определния версии и структуры
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	int result1 = WSAStartup(MAKEWORD(2, 2), &wsaData);
	// Если произошла ошибка подгрузки библиотеки
	if (result != 0) {
		cout << "WSAStartup failed result" << endl;
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));//нам необходимо изначально занулить память, 1-ый паметр, что зануляем,2-ой сколько
	hints.ai_family = AF_INET;//4-БАЙТНЫЙ Ethernet
	hints.ai_socktype = SOCK_STREAM; //задаем потоковый тип сокета
	hints.ai_protocol = IPPROTO_TCP;// Используем протокол TCP
	hints.ai_flags = AI_PASSIVE;// Пассивная сторона, потому что просто ждет соединения

	// функциия хранит в себе адрес, порт,семейство структур, адрес сокета
	result = getaddrinfo(NULL, "666", &hints, &addrResult);
	// Если инициализация структуры адреса завершилась с ошибкой,
	// выведем сообщением об этом и завершим выполнение программы 
	if (result != 0) {
		cout << "getaddrinfo failed with error" << endl;
		WSACleanup();//очситка WSAStartup
		return 1;
	}


	ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	// Если создание сокета завершилось с ошибкой, выводим сообщение,
   // освобождаем память, выделенную под структуру addr,
	if (ListenSocket == INVALID_SOCKET) {
		cout << "socket error" << endl;
		WSACleanup();//очситка WSAStartup
		freeaddrinfo(addrResult);
		return 1;
	}
	// Привязываем сокет к IP-адресу (соединились с сервером)
	result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	// Если привязать адрес к сокету не удалось, то выводим сообщение
   // об ошибке, освобождаем память, выделенную под структуру addr.
   // и закрываем открытый сокет.
   // Выгружаем DLL-библиотеку из памяти и закрываем программу.
	// Проверяем, выполнилось ли привязка сокета к локальному адресу успешно
	if (result == SOCKET_ERROR) {
		cout << "Binding failed" << endl;
		closesocket(СonnectSocket);// Закрываем сокет для входящих соединений
		ListenSocket = INVALID_SOCKET;// Обнуляем сокет для входящих соединений
		freeaddrinfo(addrResult);// Освобождаем память, выделенную для списка сетевых адресов и портов
		WSACleanup();// Очищаем ресурсы Winsock API
		return 1;// Возвращаем код ошибки
	}

	// Запускаем прослушивание входящих соединений
	result = listen(ListenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		cout << "Listening failed" << endl;
		closesocket(СonnectSocket);// Закрываем сокет для входящих соединений
		ListenSocket = INVALID_SOCKET;// Обнуляем сокет для входящих соединений
		freeaddrinfo(addrResult);// Освобождаем память, выделенную для списка сетевых адресов и портов
		WSACleanup();// Очищаем ресурсы Winsock API
		return 1;// Возвращаем код ошибки
	}

	// Принимаем входящее соединение от клиента
	СonnectSocket = accept(ListenSocket, NULL, NULL);
	if (СonnectSocket == INVALID_SOCKET) {
		cout << "Accepting SOCKET failed" << endl;

		closesocket(СonnectSocket);// Закрываем сокет для входящих соединений
		ListenSocket = INVALID_SOCKET;// Обнуляем сокет для входящих соединений
		freeaddrinfo(addrResult);// Освобождаем память, выделенную для списка сетевых адресов и портов
		WSACleanup();// Очищаем ресурсы Winsock API
		return 1;// Возвращаем код ошибки
	}

	// Закрываем сокет для входящих соединений, так как он нам больше не нужен
	closesocket(ListenSocket);

	// Цикл приема и отправки данных с клиентом
	do {
		// Очищаем буфер приема
		ZeroMemory(recvBuffer, 512);

		result = recv(СonnectSocket, recvBuffer, 512, 0);// Принимаем данные от клиента
		result1 = recv(СonnectSocket, recvBuffer1, 512, 0);// Принимаем данные от клиента

		// Проверяем, получили ли мы данные
		if (result > 0) {
			cout << "Received " << result << " bytes" << endl;
			cout << "Received data " << recvBuffer << endl;
			cout << "Received data " << recvBuffer1 << endl;

			// Отправляем данные клиенту
			result = send(СonnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
			if (result == SOCKET_ERROR) {
				cout << "failed to send" << endl;
				WSACleanup();// Очищаем ресурсы Winsock API
				return 1;// Возвращаем код ошибки
			}
		}
		// Проверяем, закрыто ли соединение с клиентом
		else if (result == 0)
			cout << "Connection closing" << endl;
		// Проверяем, произошла ли ошибка при приеме данных
		else
			cout << "recv failed with error" << endl;
		closesocket(СonnectSocket);// Закрываем сокет для общения с клиентом
		freeaddrinfo(addrResult);// Освобождаем память, выделенную для списка сетевых адресов и портов
		WSACleanup();// Очищаем ресурсы Winsock API
		return 1;// Возвращаем код ошибки

	} while (result > 0);

	// Завершаем передачу данных с клиентом
	result = shutdown(СonnectSocket, SD_SEND);
	if (result == SOCKET_ERROR) {
		// Освобождаем память, выделенную для списка сетевых адресов и портов
		freeaddrinfo(addrResult);
		WSACleanup();// Очищаем ресурсы Winsock API
		return 1;// Возвращаем код ошибки
	}

	closesocket(СonnectSocket);// Закрываем сокет для общения с клиентом

	freeaddrinfo(addrResult);// Освобождаем память, выделенную для списка сетевых адресов и портов

	WSACleanup();// Очищаем ресурсы Winsock API
	return 0;// Возвращаем код
}