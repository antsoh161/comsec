OBJ = base64.o jwp.o
SRC = base64.cpp jwp.cpp
EXE = jwp_converter
CFLAGS += -Wall


$(EXE): $(OBJ)
	g++ $(CFLAGS) -L/usr/lib -I/usr/include -o $(EXE) $(OBJ) -lssl -lcrypto

jwp.o:	jwp.cpp base64.h
	g++ $(CFLAGS) -c jwp.cpp
base64.o: base64.cpp base64.h
	g++ $(CFLAGS) -c base64.cpp 

clean:
	 rm -f $(OBJ) $(EXE)
