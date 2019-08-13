

#变量
CC=g++ -std=c++11
INCLUDE=-I../include
CFLAGS=-c -g -Wall $(INCLUDE)
CPPFLAGS=-E-
DLDFLAGS=-lpthread -lssl -lcrypto -lmysqlclient -lsqlite3
TARGET=yun
RM=-rm -rf


#函数
SRCFILE=$(wildcard *.cpp)
DESFILE=$(patsubst %.cpp,%.o,$(SRCFILE))



$(TARGET):$(DESFILE)
	$(CC) $^ $(DLDFLAGS) -o $@

%.o:%.cpp
	$(CC) $(CFLAGS) -rdynamic $<

clean:
	$(RM) $(TARGET) $(DESFILE)
