#include <iostream>
#include <zmq.hpp>
#include <unistd.h>

int main()
{
    zmq::context_t context(1);
    zmq::socket_t main_socket(context, ZMQ_REP);
    mainn_socket.setsockopt(ZMQ_RCVTIMEO, 3000);
    std::string adr = "tcp://127.0.0.1:300";
    std::string command;
    int child_id = 0;
    while(true)
    {
	std::cout << "Enter command\n";
	std::cin >> command;
        if (command == "create")
	{
            if(!child_id)
	    {
                int id;
		std::cin >> id;
                int temp = id - 1;
                while(true)
		{
                    try 
		    {
                        main_socket.bind(adr + std::to_string(++temp));
                        break;
                    }
		    catch(...) 
		    {
                    }
                }
		std::string new_adr = adr + std::to_string(temp);
                char* address_of_child = new char[new_adr.size() + 1];
                memcpy(address_of_child, new_adr.c_str(), new_adr.size() + 1);
                char* id_of_child = new char[std::to_string(id).size() + 1];
                memcpy(id_of_child, std::to_string(id).c_str(), std::to_string(id).size() + 1);
                char* arguments[] = {"./child_node", address_of_child, id_of_child, NULL};
                int process = fork();
                if (process == -1) 
		{
                    std::cout << "Unable to create first worker node\n";
                    id = 0;
                    return 1;
                } 
		else if (!process)
		{
                    execv("./child_node", arguments);
                }
	       	else 
		{
                    child_id = id;
                }
                zmq::message_t message;
                main_socket.recv(&message);
		std::string recieved_message(static_cast<char*>(message.data()), message.size());
		std::cout << recieved_message << "\n";
                delete [] address_of_child;
                delete [] id_of_child;
            } 
	    else 
	    {
                int id;
		std::cin >> id;
		std::string message_string = command + " " + std::to_string(id);
                zmq::message_t message(message_string.size());
                memcpy(message.data(), message_string.c_str(), message_string.size());
                main_socket.send(message);
                main_socket.recv(&message);
		std::string recieved_message(static_cast<char*>(message.data()), message.size());
		std::cout << recieved_message << "\n";
            }
        } 
	else if(command == "exec")
	{
            int id, value;
	    std::string parameter;
	    std::cin >> id >> parameter;
	    std::string message_string = command + " " + std::to_string(id) + " " +  parameter;
            zmq::message_t message(message_string.size());
            memcpy(message.data(), message_string.c_str(), message_string.size());
            main_socket.send(message);
            main_socket.recv(&message);
    	    std::string recieved_message(static_cast<char*>(message.data()), message.size());
            std::cout << recieved_message << "\n";
        }
       	else if (command == "heartbeat")
	{
            int time, amount;
	    std::cin >> time >> amount;
            for (int j = 0; j < amount; j++)
            {
		std::string message_string = command + " " + std::to_string(time);
                zmq::message_t message(message_string.size());
                memcpy(message.data(), message_string.c_str(), message_string.size());
                main_socket.send(message);
                main_socket.recv(&message);
	        std::string recieved_message(static_cast<char*>(message.data()), message.size());
	        if (recieved_message != "OK")
	        {
		    std::cout << "Unavailable nodes: ";
	        }
	        std::cout << recieved_message << "\n";
	        usleep((unsigned)((unsigned long long)(1000) * time));
            }
        }
       	else if (command == "kill")
	{
            int id;
	    std::cin >> id;
            if(!child_id)
	    {
	        std::cout << "Error: there isn't nodes\n";
            }
	    else if (child_id == id)
	    {
	        std::string kill_message = "terminate";
                zmq::message_t message(kill_message.size());
                memcpy(message.data(), kill_message.c_str(), kill_message.size());
                main_socket.send(message);
		std::cout << "Tree deleted successfully\n";
		child_id = 0;
            } 
	    else 
	    {
	        std::string kill_message = command + " " + std::to_string(id);
                zmq::message_t message(kill_message.size());
                memcpy(message.data(), kill_message.c_str(), kill_message.size());
                main_socket.send(message);
                main_socket.recv(&message);
		std::string received_message(static_cast<char*>(message.data()), message.size());
		std::cout << received_message << "\n";
            }
        } 
	else if(command == "exit")
	{
	    if (child_id)
            {
		std::string kill_message = "terminate";
                zmq::message_t message(kill_message.size());
                memcpy(message.data(), kill_message.c_str(), kill_message.size());
                main_socket.send(message);
		std::cout << "Tree deleted successfully\n";
            }
            main_socket.close();
            context.close();
            return 0;
        } 
	else 
	{
	    std::cout << "Error: incorrect command\n";
        }
    }
}
