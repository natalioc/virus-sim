Contagion: main.o initialize.o regiondata.o agent.o analysis.o
	g++ main.o initialize.o regiondata.o agent.o analysis.o -o Contagion

main.o: main.cpp
	g++ -c main.cpp
	
initialize.o: initialize.cpp
	g++ -c initialize.cpp
	
regiondata.o: regiondata.cpp
	g++ -c regiondata.cpp

agent.o: agent.cpp
	g++ -c agent.cpp

analysis.o: analysis.cpp
	g++ -c analysis.cpp

clean:
		rm *.o Contagion