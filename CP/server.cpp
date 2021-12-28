#include <zmq.hpp>
#include <iostream>
#include <unistd.h>
#include <map>
#include <set>
#include <string>
#include <vector>

void random(std::vector<std::vector<char>>& p)
{
    int j=-1, k, v, l, x[2], y;
    srand(time(0));
    for(l=4; l>0; l--)
        for(k=5;k-l;k--)
        {
            v = 1&rand();
            //v = rand() % 2;
            do for (x[v] = 1 + rand() % 10, x[1 - v] = 1 + rand() % 7, y = j = 0; j - l; y |= p[x[0]][x[1]] != '.', x[1 - v]++, j++); while(y);
            x[1 - v] -= l + 1, p[x[0]][x[1]] = '/', x[v]--, p[x[0]][x[1]]='/', x [v]+=2, p[x [0]][x[1]]='/', x[v]--, x[1 - v]++;
            for (j = -1; ++j - l; p[x[0]][x[1]] = 'X', x[v]--, p[x[0]][x[1]] = '/', x[v]+=2, p[x[0]][x[1]]='/', x[v]--, x[1 - v]++);
            p[x[0]][x[1]] = '/', x[v]--, p[x[0]][x[1]] = '/', x[v]+=2, p[x[0]][x[1]] = '/';
        }
        for (int i = 0; i < 12; ++i)
        {
                std::replace(p[i].begin(), p[i].end(), '/', '.');
        }
}

void send_message(std::string message_string, zmq::socket_t& socket)
{
    zmq::message_t message_back(message_string.size());
    memcpy(message_back.data(), message_string.c_str(), message_string.size());
    if(!socket.send(message_back))
    {
        std::cout << "Error: can't send message from node with pid " << getpid() << "\n";
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
        zmq::socket_t socket (context, ZMQ_REP);
	std::string port, reply;
	std::cout << "Enter the port\n";
	std::cin >> port;
	socket.bind("tcp://*:" + port);
	unsigned milliseconds;
	std::cout << "Enter the time that socket should wait for answer from client and send message to client (it is a single value)\n";
	std::cin >> milliseconds;
	socket.setsockopt(ZMQ_SNDTIMEO, (int)milliseconds);
	std::map<int, std::pair<unsigned, unsigned>> statistics;
	std::map<int, std::pair<unsigned, unsigned>> amount;
	std::map<int, std::pair<std::vector<std::vector<char>>, std::vector<std::vector<char>>>> fields;
	std::map<int, std::vector<std::pair<unsigned, unsigned>>> possible_turns;
	std::map<int, std::pair<unsigned, unsigned>> last_commands;
	std::map<int, bool> finishing;
	std::map<int, std::vector<std::pair<unsigned, unsigned>>> variants;
	while (true) 
	{
		zmq::message_t request;
		socket.recv(&request);
		std::string message(static_cast<char*>(request.data()), request.size()), reply;
		std::string command = message.substr(0, message.find(" "));
		int ID = std::stoi(message.substr(message.find(" ") + 1));
		std::cout << message << "\n";
		if (command == "ID")
		{
			statistics[ID] = {0, 0};
			amount[ID] = {0, 0};
			//reply = "OK";
			send_message("OK", socket);
		}
		else if (command == "Statistics")
		{
			if (statistics.find(ID) != statistics.end())
			{
				std::pair<unsigned, unsigned> numbers = statistics[ID];
				reply = std::to_string(numbers.first) + " " + std::to_string(numbers.second);
			}
			else
			{
				reply = "Error: player with such ID already exists";
			}
			send_message(reply, socket);
		}
		else if (command == "Get")
		{
			print(fields[ID].first);
			send_message("OK", socket);	
		}
		else if (command == "Exit")
		{
			if (statistics.find(ID) != statistics.end())
			{
				statistics.erase(ID);
				amount.erase(ID);
				fields.erase(ID);
				possible_turns.erase(ID);
				last_commands.erase(ID);
				finishing.erase(ID);
				variants.erase(ID);
			}
			//reply = "It was nice to play with you, bye!";
			send_message("It was nice to play with you, bye!", socket);
		}	
		else if (command == "Begin")
		{
			//std::cout << "Recieved Begin\n";
			amount[ID] = {0, 0};
			std::vector<std::vector<char>> server_field (12, std::vector<char>(12, '.'));
			std::vector<std::vector<char>> player_field (12, std::vector<char>(12, '.'));
			//std::cout << "Created vectors\n";
			random(server_field);
			fields[ID] = {server_field, player_field};
			//std::cout << "Created fields\n";
			server_field.clear();
			player_field.clear();
			//std::cout << "Cleared vectors\n";
			std::vector<std::pair<unsigned, unsigned>> turns (100);
			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < 10; ++j)
				{
					turns[i * 10 + j] = {i, j};
				}
			}
			//std::cout << "Created turns\n";
			possible_turns[ID] = turns;
			finishing[ID] = false;
			last_commands[ID] = {-1, -1};
			variants[ID] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
			turns.clear();
			//reply = "Start";
			send_message("Start", socket);
		}
		else if (command.substr(0, 3) == "Try")
		{
			std::string coordinates = message.substr(0, message.find(" "));
			std::cout << coordinates << "\n";
			unsigned horizontal = unsigned(coordinates[3]) - unsigned('0') + 1;
		        unsigned vertical = unsigned(coordinates[4]) - unsigned('0') + 1;
			std::cout << horizontal << " " << vertical << "\n";
			std::pair<std::vector<std::vector<char>>, std::vector<std::vector<char>>> squares = fields[ID];
			if (squares.first[vertical][horizontal] == 'X')
			{
				squares.first[vertical][horizontal] = 'K';
				//std::vector<std::pair<unsigned, unsigned>> turns = possible_turns[ID];
				for (int i = -1; i < 2; ++i)
				{
					for (int j = -1; j < 2; ++j)
					{
						if ((vertical + i > 0) && (vertical + i < 11) && (horizontal + j > 0) && (horizontal + j < 11))
						{
							if (squares.first[vertical + i][horizontal + j] == '.')
							{
								squares.first[vertical + i][horizontal + j] = 'w';
								/*std::vector<std::pair<int, int>>::iterator it;
								if ((it = std::find(turns.begin(), turns.end(), std::make_pair(vertical + i, horizontal + j))) != turns.end())
								{
								        arr.erase(arr.begin() + std::distance(turns.begin(), it);
								}*/

							}
						}
					}
				}
				//possible_turns[ID] = turns;
				reply = "Killed";
				int v = vertical, h = horizontal;
				while ((v > 1) && (squares.first[v][h] == 'K'))
				{
					--v;
				}
				if (squares.first[v][h] == 'X')
				{
					reply = "Wounded";
				}
				if (reply == "Killed")
				{
					v = vertical; h = horizontal;
					while ((v < 10) && (squares.first[v][h] == 'K'))
					{
						++v;
					}
					if (squares.first[v][h] == 'X')
					{
						reply = "Wounded";
					}
					if (reply == "Killed")
					{
						v = vertical; h = horizontal;
                                        	while ((h > 1) && (squares.first[v][h] == 'K'))
                                        	{
                                                	--h;
                                        	}
						if (squares.first[v][h] == 'X')
                                        	{
                                                	reply = "Wounded";
                                        	}
						if (reply == "Killed")
						{
							v = vertical; h = horizontal;
							while ((h < 10) && (squares.first[v][h] == 'K'))
							{
								++h;
							}
							if (squares.first[v][h] == 'X')
							{
								reply = "Wounded";
							}
						}
					}
				}
				if (reply == "Killed")					
				{
					amount[ID] = {++amount[ID].first, amount[ID].second};
					if (amount[ID].first == 10)
					{	
						reply = "Won";
						statistics[ID] = {++statistics[ID].first, statistics[ID].second};
					}
				}
			}
			else if ((squares.first[vertical][horizontal] == 'K') || (squares.first[vertical][horizontal] == 'w'))
			{
				reply = "Another";
			}
			else if (squares.first[vertical][horizontal] == '.')
			{
				reply = "Missed";
				squares.first[vertical][horizontal] = 'w';
			}
			fields[ID] = {squares.first, squares.second};
			send_message(reply, socket); 
		}
		else if (command == "Amount")
		{
			std::cout << "Amount: " << amount[ID].first << "\n";
			send_message("OK", socket);
		}
		else if (command == "Turns")
		{
			std::vector<std::pair<unsigned, unsigned>> turns = possible_turns[ID];
			for (int i = 0; i < turns.size(); i++)
			{
				std::cout << turns[i].first << " " << turns[i].second << "\n";
			}
			std::cout << "Length is " << turns.size() << "\n";
			send_message("Ok", socket);
		}
		else if ((command == "Do") || (command == "Killed"))
		{
			if (command == "Killed")
			{
				std::pair<std::vector<std::vector<char>>, std::vector<std::vector<char>>> squares = fields[ID];
				std::vector<std::pair<unsigned, unsigned>> turns = possible_turns[ID];
				unsigned vertical = last_commands[ID].first, horizontal = last_commands[ID].second;
				squares.second[vertical][horizontal] = 'K';
				//turns.erase(turns.begin() + std::distance(turns.begin(), std::find(turns.begin(), turns.end(), std::make_pair(vertical, horizontal))));
				for (int i = -1; i < 2; ++i)
				{
					for (int j = -1; j < 2; ++j)
					{
						if ((vertical + i > 0) && (vertical + i < 11) && (horizontal + j > 0) && (horizontal + j < 11))
						{
							std::vector<std::pair<unsigned, unsigned>>::iterator it;
							if ((it = std::find(turns.begin(), turns.end(), std::make_pair(vertical + i, horizontal + j))) != turns.end())
							{
								turns.erase(turns.begin() + std::distance(turns.begin(), it));
							}
							if (squares.second[vertical + i][horizontal + j] == '.')
							{
								squares.second[vertical + i][horizontal + j] = 'w';
								/*std::vector<std::pair<unsigned, unsigned>>::iterator it;
								if ((it = std::find(turns.begin(), turns.end(), std::make_pair(vertical + i, horizontal + j))) != turns.end())
								{
									turns.erase(turns.begin() + std::distance(turns.begin(), it));
								}*/
							}
						}
					}
				}
				fields[ID] = {squares.first, squares.second};
				possible_turns[ID] = turns;
				finishing[ID] = false;
				variants[ID] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
				amount[ID] = {amount[ID].first, ++amount[ID].second};
				if (amount[ID].second == 10)
				{
					reply = "Lost";
					amount[ID] = {0, 0};
				}
			}
			if (!finishing[ID])
			{
				int length = possible_turns[ID].size();
				srand(time(0));
				int number = rand() % length;
				std::vector<std::pair<unsigned, unsigned>> coordinates = possible_turns[ID];
				std::pair<unsigned, unsigned> turn = coordinates[number];
				std::cout << "Turn is " << turn.first << " " << turn.second << "\n";
				coordinates.erase(coordinates.begin() + number);
				possible_turns[ID] = coordinates;
				last_commands[ID] = turn;
				reply = "Try" + std::to_string(turn.first) + std::to_string(turn.second);
				std::cout << "Tried coordinates " << turn.first << " " << turn.second << "\n";
			}
			else
			{
				int length, number, k = 1;
				std::vector<std::pair<unsigned, unsigned>> positions;
                        	std::pair<unsigned, unsigned> turn;
				do
				{
					length = variants[ID].size();
					number = rand() % length;
				        positions = variants[ID];
					turn = positions[number];
               				positions.erase(positions.begin() + number);
				}
				while ((length > 0) && (!((last_commands[ID].first + turn.first > 0) && (last_commands[ID].first + turn.first < 11) && (last_commands[ID].second + turn.second > 0) 
					&& (last_commands[ID].second + turn.second < 11))));
				std::pair<std::vector<std::vector<char>>, std::vector<std::vector<char>>> squares = fields[ID];
				std::vector<std::pair<unsigned, unsigned>> turns = possible_turns[ID];
				unsigned vertical, horizontal;
				do
				{
					send_message("Try" + std::to_string(last_commands[ID].first + turn.first * k) + std::to_string(last_commands[ID].second + turn.second * k), socket);
					++k;
					zmq::message_t answer;
					socket.recv(&answer);
					std::string string(static_cast<char*>(answer.data()), answer.size());
					reply = string.substr(0, string.find(" "));		
					vertical = last_commands[ID].first + turn.first * k, horizontal = last_commands[ID].second + turn.second * k;
					turns.erase(turns.begin() + std::distance(turns.begin(), std::find(turns.begin(), turns.end(), std::make_pair(vertical, horizontal))));
					if ((reply == "Wounded") || (reply == "Killed"))
					{
						squares.second[vertical][horizontal] = 'K';
						for (int i = -1; i < 2; ++i)
						{
							for (int j = -1; j < 2; ++j)
							{
								if ((vertical + i > 0) && (vertical + i < 11) && (horizontal + j > 0) && (horizontal + j < 11))
								{
									std::vector<std::pair<unsigned, unsigned>>::iterator it;
                                                                        if ((it = std::find(turns.begin(), turns.end(), std::make_pair(vertical + i, horizontal + j))) != turns.end())
                                                                        {
                                                                                turns.erase(turns.begin() + std::distance(turns.begin(), it));
                                                                        }
									if (squares.second[vertical + i][horizontal + j] == '.')
									{
										squares.second[vertical + i][horizontal + j] = 'w';
										/*std::vector<std::pair<unsigned, unsigned>>::iterator it;
										if ((it = std::find(turns.begin(), turns.end(), std::make_pair(vertical + i, horizontal + j))) != turns.end())
										{
											turns.erase(turns.begin() + std::distance(turns.begin(), it));
										}*/
									}
								}
							}
						}
					}
					else if (reply == "Missed")
					{
						squares.second[vertical][horizontal] = 'w';
					}
					//fields[ID] = {squares.first, squares.second};
				}
				while (reply == "Wounded");	
				fields[ID] = {squares.first, squares.second};
				possible_turns[ID] = turns;
				variants[ID] = positions;
				if (reply == "Missed")
				{
					reply = "Do";
				}
				else if (reply == "Killed")
				{
					finishing[ID] = false;
					variants[ID] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
					amount[ID] = {amount[ID].first, ++amount[ID].second};
					if (amount[ID].second == 10)
					{
						reply = "Lost";
						amount[ID] = {0, 0};
					}
					else
					{
						int length = possible_turns[ID].size();
						srand(time(0));
						int number = rand() % length;
						std::vector<std::pair<unsigned, unsigned>> coordinates = possible_turns[ID];
						std::pair<unsigned, unsigned> turn = coordinates[number];
						coordinates.erase(coordinates.begin() + number);
						possible_turns[ID] = coordinates;
						last_commands[ID] = turn;
						reply = "Try" + std::to_string(turn.first) + std::to_string(turn.second);
						std::cout << "Tried coordinates " << turn.first << " " << turn.second << "\n";
					}
				}
			}
			send_message(reply, socket);	
		}
		else if (command == "Missed")
		{
			std::pair<std::vector<std::vector<char>>, std::vector<std::vector<char>>> squares = fields[ID];
                        squares.second[last_commands[ID].first][last_commands[ID].second] = 'w';
                        fields[ID] = {squares.first, squares.second};
			send_message("Do", socket);
		}
		else if (command == "Wounded")
		{
			finishing[ID] = true;
			int length, number, k = 1;
			std::vector<std::pair<unsigned, unsigned>> positions;
			std::pair<unsigned, unsigned> turn;
			do
			{
				length = variants[ID].size();
				number = rand() % length;
				positions = variants[ID];
				turn = positions[number];
				positions.erase(positions.begin() + number);
			}
			while ((length > 0) && (!((last_commands[ID].first + turn.first > 0) && (last_commands[ID].first + turn.first < 11) && (last_commands[ID].second + turn.second > 0)
				&& (last_commands[ID].second + turn.second < 11))));
			std::pair<std::vector<std::vector<char>>, std::vector<std::vector<char>>> squares = fields[ID];
			std::vector<std::pair<unsigned, unsigned>> turns = possible_turns[ID];
                        unsigned vertical, horizontal;
			do
			{
				send_message("Try" + std::to_string(last_commands[ID].first + turn.first * k) + std::to_string(last_commands[ID].second + turn.second * k), socket);
				++k;
				zmq::message_t answer;
				socket.recv(&answer);
				std::string string(static_cast<char*>(answer.data()), answer.size());
				reply = string.substr(0, string.find(" "));
				vertical = last_commands[ID].first + turn.first * k, horizontal = last_commands[ID].second + turn.second * k;
				turns.erase(turns.begin() + std::distance(turns.begin(), std::find(turns.begin(), turns.end(), std::make_pair(vertical, horizontal))));
				if ((reply == "Wounded") || (reply == "Killed"))
				{
					squares.second[vertical][horizontal] = 'K';
					for (int i = -1; i < 2; ++i)
					{
						for (int j = -1; j < 2; ++j)
						{
							if ((vertical + i > 0) && (vertical + i < 11) && (horizontal + j > 0) && (horizontal + j < 11))
							{
								std::vector<std::pair<unsigned, unsigned>>::iterator it;
								if ((it = std::find(turns.begin(), turns.end(), std::make_pair(vertical + i, horizontal + j))) != turns.end())
								{
									turns.erase(turns.begin() + std::distance(turns.begin(), it));
								}
								if (squares.second[vertical + i][horizontal + j] == '.')
								{
									squares.second[vertical + i][horizontal + j] = 'w';
									std::vector<std::pair<unsigned, unsigned>>::iterator it;
									/*if ((it = std::find(turns.begin(), turns.end(), std::make_pair(vertical + i, horizontal + j))) != turns.end())
									{
										turns.erase(turns.begin() + std::distance(turns.begin(), it));
									}*/
								}
							}
						}
					}
				}
				else if (reply == "Missed")
				{
					squares.second[vertical][horizontal] = 'w';
				}
				//fields[ID] = {squares.first, squares.second};
			}
			while (reply == "Wounded");
			fields[ID] = {squares.first, squares.second};
			variants[ID] = positions;
			if (reply == "Missed")
			{
				reply = "Do";
			}
			else if (reply == "Killed")
			{
				finishing[ID] = false;
				variants[ID] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
				amount[ID] = {amount[ID].first, ++amount[ID].second};
				if (amount[ID].second == 10)
				{
					reply = "Lost";
					amount[ID] = {0, 0};
					statistics[ID] = {statistics[ID].first, ++statistics[ID].second};
				}
				else
				{
					int length = possible_turns[ID].size();
					srand(time(0));
					int number = rand() % length;
					std::vector<std::pair<unsigned, unsigned>> coordinates = possible_turns[ID];
					std::pair<unsigned, unsigned> turn = coordinates[number];
					coordinates.erase(coordinates.begin() + number);
					possible_turns[ID] = coordinates;
					last_commands[ID] = turn;
					reply = "Try" + std::to_string(turn.first) + std::to_string(turn.second);
					std::cout << "Tried coordinates " << turn.first << " " << turn.second << "\n";
				}
			}
			send_message(reply, socket);
		}
		/*
		else if (command == "Left")
		{
			std::pair<unsigned, unsigned> last_command = last_commands[ID];
			last_command = {last_command.first, last_command.second - 1};
			last_commands[ID] = last_command;
			reply = "Try" + std::to_string(last_command.first) + std::to_string(last_command.second - 1);
		}		
 		else if (command == "Right")
                {
                        std::pair<unsigned, unsigned> last_command = last_commands[ID];
                        last_command = {last_command.first, last_command.second + 1};
                        last_commands[ID] = last_command;
                        reply = "Try" + std::to_string(last_command.first) + std::to_string(last_command.second + 1);
                }
                else if (command == "Up")
                {
                        std::pair<unsigned, unsigned> last_command = last_commands[ID];
                        last_command = {last_command.first - 1, last_command.second};
                        last_commands[ID] = last_command;
                        reply = "Try" + std::to_string(last_command.first - 1) + std::to_string(last_command.second);
                }
                else if (command == "Down")
                {
                        std::pair<unsigned, unsigned> last_command = last_commands[ID];
                        last_command = {last_command.first + 1, last_command.second};
                        last_commands[ID] = last_command;
                        reply = "Try" + std::to_string(last_command.first + 1) + std::to_string(last_command.second);
                }
		send_message(reply, socket);*/
	}
	return 0;
}
