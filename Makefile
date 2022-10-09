# run: quiz
# 	./quiz

quiz: main.o dbFuncs.o requests.o utils.o
	g++ main.o dbFuncs.o requests.o utils.o -lPocoNetSSL -lPocoFoundation -lPocoNet -lPocoUtil -lmariadbcpp -o quiz

main.o: main.cpp
	g++ main.cpp  -lPocoNetSSL -lPocoFoundation -lPocoNet -lPocoUtil -lmariadbcpp -c

requests.o: implemetations/requests.cpp
	g++ implemetations/requests.cpp  -lPocoNetSSL -lPocoFoundation -lPocoNet -lPocoUtil -lmariadbcpp -c
dbFuncs.o: implemetations/dbFuncs.cpp
	g++ implemetations/dbFuncs.cpp  -lPocoNetSSL -lPocoFoundation -lPocoNet -lPocoUtil -lmariadbcpp -c

utils.o: implemetations/utils.cpp
	g++ implemetations/utils.cpp  -lPocoNetSSL -lPocoFoundation -lPocoNet -lPocoUtil -lmariadbcpp -c

clean:
	rm *.o

build: quiz
	rm *.o
	