EXEC_NAME=evol
SRCS=main.cpp

CPPFLAGS+=-std=c++11 -g
OBJS=$(subst .cpp,.o,$(SRCS))

#building the application
all: $(EXEC_NAME)

$(EXEC_NAME): $(OBJS)
	$(CXX) -o $(EXEC_NAME) $(OBJS)

#building the dependency file
DEPEND_FILE=./.depend
depend: $(DEPEND_FILE)

$(DEPEND_FILE): $(SRCS)
	$(RM) $(DEPEND_FILE)
	$(CXX) $(CPPFLAGS) -MM $^>>$(DEPEND_FILE)

include $(DEPEND_FILE)

#clean
clean:
	$(RM) $(OBJS) $(DEPEND_FILE) $(EXEC_NAME)

