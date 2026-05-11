NAME1 = Compiler
NAME2 = VirtualMachine

CXX = c++
CXXFLAGS = -Wall -Wextra -std=c++17

SRC_DIR = src

COMPILER_SRC = $(SRC_DIR)/mainCompiler.cpp $(SRC_DIR)/lexer.cpp $(SRC_DIR)/tokenizer.cpp $(SRC_DIR)/parser.cpp $(SRC_DIR)/VM.cpp $(SRC_DIR)/SymbolTable.cpp
COMPILER_OBJ = $(COMPILER_SRC:.cpp=.o)

VM_SRC = $(SRC_DIR)/mainVM.cpp $(SRC_DIR)/VM.cpp $(SRC_DIR)/SymbolTable.cpp
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