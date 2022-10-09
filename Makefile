quiz: main.o dbFuncs.o requests.o utils.o
	g++ main.o dbFuncs.o requests.o utils.o -lPocoNetSSL -lPocoFoundation -lPocoNet -lPocoUtil -lmariadbcpp -o quiz

main.o: main.cpp
	g++ main.cpp  -lPocoNetSSL -lPocoFoundation -lPocoNet -lPocoUtil -lmariadbcpp -c

requests.o: implemetations/requests.cpp headers/requests.h
	g++ implemetations/requests.cpp  -lPocoNetSSL -lPocoFoundation -lPocoNet -lPocoUtil -lmariadbcpp -c
dbFuncs.o: implemetations/dbFuncs.cpp headers/dbFuncs.h
	g++ implemetations/dbFuncs.cpp  -lPocoNetSSL -lPocoFoundation -lPocoNet -lPocoUtil -lmariadbcpp -c
utils.o: implemetations/utils.cpp headers/utils.h
	g++ implemetations/utils.cpp  -lPocoNetSSL -lPocoFoundation -lPocoNet -lPocoUtil -lmariadbcpp -c

clean:
	rm *.o

run: quiz
	./quiz

build: quiz
	rm *.o
	