#include <SFML/Graphics.hpp>
#include<SFML/Network.hpp>
#include<list>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>



sf::TcpSocket tcpSocket;
bool RunProgram = true;

void receivefromserver()
{
	while (RunProgram)
	{
		sf::Packet ServerPack;
		if (tcpSocket.receive(ServerPack) == sf::Socket::Done)
		{
			std::string Input;
			std::string Name;
			ServerPack >> Input >> Name;
			std::cout << Name << ": " << Input << "\n";
		}
	}
}


int main()
{
	std::string UserName;
	sf::SocketSelector selector;
	sf::TcpListener listener;
	listener.setBlocking(false);
	std::string serverinput;

	std::string Text = "";
	char Eleccion;
	std::cout << "Cliente(C) o Servidor(S)?" << std::endl;
	std::cin >> Eleccion;
	Eleccion = toupper(Eleccion);




	if (Eleccion == 'S')
	{
		// Create a socket to listen to new connections
		std::string Text;
		std::string User;

		sf::TcpListener listener;
		listener.listen(45000);
		listener.setBlocking(false);

		// Create a list to store the future clients
		//std::list<sf::TcpSocket*> clients;
		std::vector < sf::TcpSocket*>clients;

		// Create a selector
		sf::SocketSelector selector;

		// Add the listener to the selector
		selector.add(listener);
		// Endless loop that waits for new connections
		while (true)
		{
			// Make the selector wait for data on any socket
			if (selector.wait())
			{
				// Test the listener
				if (selector.isReady(listener))
				{
					// The listener is ready: there is a pending connection
					sf::TcpSocket* client = new sf::TcpSocket;
					if (listener.accept(*client) == sf::Socket::Done)
					{
						// Add the new client to the clients list
						clients.push_back(client);
						// Add the new client to the selector so that we will
						// be notified when he sends something
						selector.add(*client);
						std::cout << "Se unio una nueva persona al chat" << std::endl;
					}
					else
					{
						// Error, we won't get a new connection, delete the socket
						delete client;
					}
				}
				else
				{
					// The listener socket is not ready, test all other sockets (the clients)
					for (int i = 0; i < clients.size(); i++)
					{

						if (selector.isReady(*clients[i]))
						{
							// The client has sent some data, we can receive it
							sf::Packet packet;

							if (clients[i]->receive(packet) == sf::Socket::Done)
							{
								packet >> Text >> User;
								for (int j = 0; j < clients.size(); j++)
								{
									if (clients[i] != clients[j])
									{
										packet << Text << User;
										//clients[j]->setBlocking(false);
										if (clients[j]->send(packet) != sf::Socket::Done)
										{
											std::cout << "valemos verga";
										}


									}
								}
								/*packet >> Text >> User;

								std::cout << User << ": " << Text << std::endl;*/
							}
						}
					}
				}
			}
		}
	}

	else if (Eleccion == 'C')
	{
		std::cin.ignore();

		listener.listen(45000);

		std::string IPDir;
		sf::Packet Pack;
		std::cout << "Introduzca IP: ";
		std::cin >> IPDir;
		std::cout << "Ingrese su nombre de usuario: ";
		std::cin >> UserName;
		std::cin.ignore();
		std::cin.clear();
		if (tcpSocket.connect(IPDir, 45000) != sf::Socket::Done)
		{
			return -1;
		}
		else

			tcpSocket.setBlocking(false);
		sf::Thread ReceiveThread(&receivefromserver);
		ReceiveThread.launch();
		while (RunProgram)
		{
			std::getline(std::cin, Text);
			std::cin.sync();
			if (Text != "_exit_")
			{
				Pack << Text << UserName;
				if (tcpSocket.send(Pack) != sf::Socket::Done)
				{
					std::cout << "Error al mandar el paquete al servidor.\n";
				}
				else
				{
					Pack.clear();
				}
			}
			else
			{
				RunProgram = false;
			}

		}
		ReceiveThread.terminate();
	}
	else
	{
		std::cout << "Opcion invalida.\n";
	}
	std::cin.ignore();
	std::cin.get();

	return 0;
}