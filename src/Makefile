all:
	g++ -o ../bin/server server.cpp user.cpp message.cpp -pthread
	g++ -o ../bin/client client.cpp user.cpp message.cpp -pthread
	
runserver:
	./server
	
runclient:
	./client