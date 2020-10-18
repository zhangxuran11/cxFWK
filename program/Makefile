CC = g++
CXXFLAGS = -std=c++11 -I../inc/ -I${BOOST_INC} -g -DBOOST_BIND_GLOBAL_PLACEHOLDERS
LDFLAGS = -lpthread

CXX_SRC = eureka.cpp
CXX_OBJS := $(CXX_SRC:.cpp=.ox)

all: eureka example_service

eureka: eureka.cpp
	$(CC) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

example_service: example_service.cpp
	$(CC) $(CXXFLAGS) $^ $(LDFLAGS) -o $@