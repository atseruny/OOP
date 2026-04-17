NAME1 = Compiler
NAME2 = VirtualMachine

CXX = c++
CXXFLAGS = -Wall -Wextra -std=c++17

COMPILER_SRC = mainCompiler.cpp ../parser_v3/lexer.cpp ../parser_v3/tokenizer.cpp parser.cpp VM.cpp SymbolTable.cpp
COMPILER_OBJ = $(COMPILER_SRC:.cpp=.o)

VM_SRC = mainVM.cpp VM.cpp SymbolTable.cpp
VM_OBJ = $(VM_SRC:.cpp=.o)

all: $(NAME1) $(NAME2)

$(NAME1): $(COMPILER_OBJ)
	$(CXX) $(CXXFLAGS) $(COMPILER_OBJ) -o $(NAME1)

$(NAME2): $(VM_OBJ)
	$(CXX) $(CXXFLAGS) $(VM_OBJ) -o $(NAME2)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(COMPILER_OBJ) $(VM_OBJ)

fclean: clean
	rm -f $(NAME1) $(NAME2)

re: fclean all

.PHONY: all clean fclean re