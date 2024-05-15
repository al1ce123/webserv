NAME		= webserv
CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98
SRCDIR		= src
SRC 		= $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/*/*.cpp)
OBJDIR		= .obj
OBJ 		= $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
HEADER		= -I ./include

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(HEADER) $(OBJ) -o $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

test:
	$(CXX) $(CXXFLAGS) ./testing/main.cpp -o prog && ./prog && rm prog

debug: fclean $(OBJ)
	$(CXX) -g3 $(CXXFLAGS) $(HEADER) $(OBJ) -o $(NAME) -fsanitize=address

.PHONY: all, clean, fclean, re, debug, test