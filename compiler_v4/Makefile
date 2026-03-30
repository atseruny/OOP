NAME = ast
CXX = c++
CXXFLAGS = -Wall -Wextra 

SRC = main.cpp ../parser_v3/lexer.cpp ../parser_v3/tokenizer.cpp parser.cpp \
		VM.cpp SymbolTable.cpp
OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
		$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
		$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: clean all

.PHONY:	all clean fclean re