#include <zmq.hpp>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <algorithm>

void send_message(std::string message_string, zmq::socket_t& socket)
{
    zmq::message_t message_back(message_string.size());
    memcpy(message_back.data(), message_string.c_str(), message_string.size());
    if(!socket.send(message_back))
    {
        std::cout << "Error: can't send message from node with pid " << getpid() << "\n";
    }
}

std::string recieve_message(zmq::socket_t& socket)
{
	zmq::message_t reply;
	if (!socket.recv(&reply))
	{
		std::cout << "There's no answer from server\n";
		exit(1);
	}
	std::string message(static_cast<char*>(reply.data()), reply.size());
	return message;
}

void random(std::vector<std::vector<char>>& p)
{
    int j=-1, k, v, l, x[2], y;
    srand(time(0));
    for(l=4; l>0; l--)
        for(k=5;k-l;k--)
        {
            v = 1&rand();
            do for (x[v] = 1 + rand() % 10, x[1 - v] = 1 + rand() % 7, y = j = 0; j - l; y |= p[x[0]][x[1]] != '.', x[1 - v]++, j++); while(y);
            x[1 - v] -= l + 1, p[x[0]][x[1]] = '/', x[v]--, p[x[0]][x[1]] = '/', x [v] += 2, p[x [0]][x[1]] = '/', x[v]--, x[1 - v]++;
            for (j = -1; ++j - l; p[x[0]][x[1]] = 'X', x[v]--, p[x[0]][x[1]] = '/', x[v] += 2, p[x[0]][x[1]] = '/', x[v]--, x[1 - v]++);
            p[x[0]][x[1]] = '/', x[v]--, p[x[0]][x[1]] = '/', x[v]+=2, p[x[0]][x[1]] = '/';
        }
        for (int i = 0; i < 12; ++i)
        {
                std::replace(p[i].begin(), p[i].end(), '/', '.');
        }
}

void flood(std::vector<std::vector<char>>& p)
{
	for (int i = 0; i < 12; i++)
	{
		p[i].clear();
		p[i] = std::vector<char>(12, '.');
	}
}

void print(std::vector<std::vector<char>>& p)
{
	for (int i = 1; i < 11; ++i)
	{
		for (int j = 1; j < 11; ++j)
		{
			std::cout << p[i][j];
		}
		std::cout << "\n";
	}
}

int main()
{
	zmq::context_t context (1);
        zmq::socket_t socket (context, ZMQ_REQ);
	std::string port;
	std::cout << "Enter the port\n";
	std::cin >> port;
        std::cout << "Connecting to hello world server…" << std::endl;
	unsigned milliseconds;
        std::cout << "Enter the time that socket should wait for answer from server\n";
        std::cin >> milliseconds;
        socket.setsockopt(ZMQ_SNDTIMEO, (int)milliseconds);
	socket.setsockopt(ZMQ_RCVTIMEO, (int)milliseconds);	
        socket.connect ("tcp://localhost:" + port);
	send_message("ID " + std::to_string(getpid()), socket);
	zmq::message_t reply;
	/*if (!socket.recv(&reply))
	{
		std::cout << "There's no answer from server\n";
		return 0;
	}*/
	recieve_message(socket);
	std::cout << "If you want to start a game, input Begin\n";
	std::vector<std::vector<char>> my_field(12, std::vector<char> (12, '.'));
       	std::vector<std::vector<char>> server_field (12, std::vector<char> (12, '.'));
        std::string command;
	bool playing = false;
	while (std::cin >> command)
	{
		if (command == "Begin")
		{
			playing = true;
			int number;
			flood(my_field);
			flood(server_field);
			std::cout << "Do you want to arrange the ships by yourself or generate a random combination? If the first, input 1, else - 2\n";
			do
			{
				std::cin >> number;
				if ((number < 1) || (number > 2))
				{
					std::cout << "Error, input 1 or 2\n";
				}
			}
			while ((number < 1) || (number > 2));
			if (number == 1)
			{
				int amount = 0, amounts[4], v1, v2;
				for (int i = 0; i < 4; ++i)
				{
					amounts[i] = 0;
				}
				char h1, h2;
				std::cout << "You should input 10 fours of symbols: for example, A 1 A 4, or B 3 E 3\n";
				while (amount < 10)
				{
					std::cin >> h1 >> v1 >> h2 >> v2;
					if ((v1 < 1) || (v2 < 1) || (v1 > 10) || (v2 > 10))
					{
						std::cout << "Number must be greater than 0 and less than 11\n";
						continue;
					}
					if (!((h1 >= 'A') && (h1 <= 'J') && (h2 >= 'A') && (h2 <= 'J')))
					{
						std::cout << "Letters must be not less than A and not greater than J\n";
						continue;
					}
					if ((v1 != v2) && (h1 != h2))
					{
						std::cout << "Ship must be parallel to one of the coordinate axis\n";
						continue;
					}
					if ((v1 - v2 > 4) || (h1 - h2 > 4))
					{
						std::cout << "These ships are too long\n";
						continue;
					}
					int ih1 = int(h1) - int('A') + 1, ih2 = int(h2) - int('A') + 1;
					if (v2 < v1)
					{
						std::swap(v1, v2);
					}
					if (ih2 < ih1)
					{
						std::swap(ih1, ih2);
					}
					if (v1 == v2)
					{
						bool possible = true;
						for (int i = ih1 - 1; i < ih2 + 2; ++i)
						{
						        for (int j = -1; j < 2; ++j)
						        {
							        if ((v1 + j > 0) && (v1 + j < 11) && (i > 0) && (i < 11))
							        {
								        if (my_field[v1 + j][i] == 'X')
								        {
									        possible = false;
									        break;
								        }
							        }
						        }
							if (!possible)
							{
								break;
							}		
						}
						if (!possible)
						{
							std::cout << "It is impossible to put the ship, as it will come into contact with another\n";
							continue;
						}
						else
						{
							int number = ih2 - ih1;
							if (amounts[number] == 4 - number)
							{
								std::cout << "You already have enough ships of this type\n";
								continue;
							}
							++amounts[number];							
							for (int i = ih1; i < ih2 + 1; ++i)
							{
								my_field[v1][i] = 'X';
							}
							++amount;
							std::cout << "Successfully created ship\n";
						}
					}
					else if (ih1 == ih2)
					{
						bool possible = true;
                                                for (int i = v1 - 1; i < v2 + 2; ++i)
                                                {
                                                        for (int j = -1; j < 2; ++j)
                                                        {
                                                                if ((ih1 + j > 0) && (ih1 + j < 11) && (i > 0) && (i < 11))
                                                                {
                                                                        if (my_field[i][ih1 + j] == 'X')
                                                                        {
                                                                                possible = false;
                                                                                break;
                                                                        }
                                                                }
                                                        }
                                                        if (!possible)
                                                        {
                                                                break;
                                                        }
                                                }
                                                if (!possible)
                                                {
                                                        std::cout << "It is impossible to put the ship, as it will come into contact with another\n";
                                                        continue;
                                                }
                                                else
                                                {
							int number = v2 - v1;
                                                        if (amounts[number] == 4 - number)
                                                        {
                                                                std::cout << "You already have enough ships of this type\n";
                                                                continue;
                                                        }
							++amounts[number];
                                                        for (int i = v1; i < v2 + 1; ++i)
                                                        {
                                                                my_field[i][ih1] = 'X';
                                                        }
                                                        ++amount;
							std::cout << "Successfully created ship\n";
                                                }
					}
				}
			}
			else if (number == 2)
			{
				std::cout << "Random generations of ships will be displayed, if you choose input 1, else 2 - then another arrangement will be displayed\n";
			        int indicator;
				do
				{
					flood(my_field);
					random(my_field);
					print(my_field);
					/*for (int i = 1; i < 11; ++i)
					{
						for (int j = 1; j < 11; ++j)
						{
							std::cout << my_field[i][j];
						}
						std::cout << "\n";
					}*/
					do
					{
						std::cin >> indicator;
						if ((indicator < 1) || (indicator > 2))
						{
							std::cout << "Indicator must be 1 or 2\n";
						}
					}
					while ((indicator < 1) || (indicator > 2));
				}
				while (indicator != 1);
			}
			send_message("Begin " + std::to_string(getpid()), socket);
			recieve_message(socket);
			std::cout << "Input move\n";
			continue;
		}
		else if (command == "Get")
		{
			send_message("Get " + std::to_string(getpid()), socket);
			recieve_message(socket);
		}
		if (command == "Exit")
		{
			send_message("Exit " + std::to_string(getpid()), socket);
			std::string reply = recieve_message(socket);
			std::cout << reply;
			std::cout << "Input move\n";
			return 0;
		}
		if (command == "Statistics")
		{
			send_message("Statistics " + std::to_string(getpid()), socket);
			std::string reply = recieve_message(socket);
			std::cout << "You have " + reply.substr(0, reply.find(" ")) + " wons and " + reply.substr(reply.find(" ") + 1) + " loses\n";
			std::cout << "Input move\n";
			continue;
		}
		if (command == "My")
		{
			std::cout << "Here is your field\n";
			print(my_field);
			std::cout << "Input move\n";
			continue;
		}
		if (command == "Amount")
		{
			send_message("Amount " + std::to_string(getpid()), socket);
			recieve_message(socket);
			continue;
		}
		if (command == "Server")
		{
			std::cout << "Here is server's field\n";
			print(server_field);
			std::cout << "Input move\n";
			continue;
		}
		if (command == "Turns")
		{
			send_message("Turns " + std::to_string(getpid()), socket);
			recieve_message(socket);
		}
		if (command == "Try")
		{
			if (!playing)
			{
				std::cout << "You aren't playing at the moment. Start a new game\n";
				continue;
			}
			else
			{
				int v;
				char h;
				while (true)
				{
					std::cin >> h >> v;
					if (!((h >= 'A') && (h <= 'J')))
					{
						std::cout << "Letters must be not less than A and not greater than J\n";
						continue;	
					}
					else if ((v < 1) || (v > 10))
					{
						std::cout << "Numbers must be greater than 0 and less than 11\n";
						continue;
					}
					break;
				}
				send_message("Try" + std::to_string(int(h) - int('A')) + std::to_string(v - 1) + " " + std::to_string(getpid()), socket);
				std::string reply = recieve_message(socket);
				std::cout << "Reply: " << reply << "\n";
				if ((reply == "Killed") || (reply == "Wounded"))
				{
					server_field[v][int(h) - int('A') + 1] = 'K';
					if (reply == "Killed")
					{
						std::cout << "You killed one of the server's ships\n";
					}
					else
					{
						std::cout << "You wounded one of the server's ships\n";
					}
					std::cout << "Input move\n";
					continue;
				}
				if (reply == "Another")
				{
					std::cout << "You have already entered these coordinates. Input something new\n";
					continue;
				}
				if (reply == "Won")
				{
					std::cout << "You won this game!\n";
					playing = false;
					continue;
				}
				if (reply == "Missed")
				{
					server_field[v][int(h) - int('A') + 1] = 'w';
					send_message("Do " + std::to_string(getpid()), socket);	
					while (true)
		                        {
						reply = recieve_message(socket);
					        if (reply.substr(0, 3) == "Try")
                                                {
							std::cout << "Server's turn: " << char(int(reply[4] - int('0') + 'A')) << " " << int(reply[3]) - int('0') + 1 << "\n";
                                                        //reply = std::to_string(int(reply[4]) - int('0')) + " " + reply[3];
                                                }
						else
						{
							std::cout << "Server's reply: " << reply << "\n";
						}
						if ((reply == "Lost") || (reply == "Do"))
						{
							break;
						}
						int hor = int(reply[4]) - int('0') + 1, ver = int(reply[3]) - int('0') + 1;
						if (my_field[ver][hor] == 'X')
						{
							reply = "Killed";
							int v = ver, h = hor;
							my_field[v][h] = 'K';
							for (int i = -1; i < 2; i++)
							{
								for (int j = -1; j < 2; ++j)
								{
									if (my_field[v + i][h + j] == '.')
									{
										my_field[v + i][h + j] = 'w';
									}
								}
							}
							while ((v > 1) && (my_field[v][h] == 'K'))
							{
								--v;
							}
							if (my_field[v][h] == 'X')
							{
								reply = "Wounded";
							}
							if (reply == "Killed")
							{
								v = ver; h = hor;
								while ((v < 10) && (my_field[v][h] == 'K'))
								{
									++v;
								}
								if (my_field[v][h] == 'X')
								{
									reply = "Wounded";
								}
								if (reply == "Killed")
								{
									v = ver; h = hor;
									while ((h > 1) && (my_field[v][h] == 'K'))
									{
										--h;
									}
									if (my_field[v][h] == 'X')
									{
										reply = "Wounded";
									}
									if (reply == "Killed")
									{
										v = ver; h = hor;
										while ((h < 10) && (my_field[v][h] == 'K'))
										{
											++h;
										}
										if (my_field[v][h] == 'X')
										{
											reply = "Wounded";
										}
									}
								}
							}
						}
						else
						{
							reply = "Missed";
							my_field[ver][hor] = 'w';
						}
						std::cout << "Our reply is " << reply << "\n";
						send_message(reply + " " + std::to_string(getpid()), socket);
					}
					if (reply == "Lost")
					{
						std::cout << "You lost this game\n";
						playing = false;
						continue;
					}
				}
			}
		}
	}
        return 0;
}
