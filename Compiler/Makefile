NAME1 = Compiler
NAME2 = Executor
NAME3 = Debugger
CXX = c++
CXXFLAGS = -Wall -Wextra -std=c++17
SRC_DIR = src
OBJ_DIR = obj

COMPILER_SRC = $(SRC_DIR)/mainCompiler.cpp $(SRC_DIR)/lexer.cpp $(SRC_DIR)/tokenizer.cpp $(SRC_DIR)/parser.cpp \
	$(SRC_DIR)/utils.cpp $(SRC_DIR)/VM.cpp $(SRC_DIR)/SymbolTable.cpp
EXEC_SRC = $(SRC_DIR)/mainExec.cpp $(SRC_DIR)/CPU.cpp $(SRC_DIR)/Loader.cpp $(SRC_DIR)/Memory.cpp $(SRC_DIR)/Executor.cpp
DEBUG_SRC = $(SRC_DIR)/mainDebug.cpp $(SRC_DIR)/CPU.cpp $(SRC_DIR)/Loader.cpp $(SRC_DIR)/Memory.cpp $(SRC_DIR)/Debugger.cpp

COMPILER_OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(COMPILER_SRC))
EXEC_OBJ     = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(EXEC_SRC))
DEBUG_OBJ    = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(DEBUG_SRC))

all: $(NAME1) $(NAME2) $(NAME3)

$(NAME1): $(COMPILER_OBJ)
	$(CXX) $(CXXFLAGS) $(COMPILER_OBJ) -o $(NAME1)

$(NAME2): $(EXEC_OBJ)
	$(CXX) $(CXXFLAGS) $(EXEC_OBJ) -o $(NAME2)

$(NAME3): $(DEBUG_OBJ)
	$(CXX) $(CXXFLAGS) $(DEBUG_OBJ) -o $(NAME3)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME1) $(NAME2) $(NAME3)

re: fclean all

.PHONY: all clean fclean re