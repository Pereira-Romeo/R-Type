BINARY_NAME	=	ecs

BUILD_DIR = build

SRC	=	src/main.cpp

OBJ = $(SRC:src/%.cpp=$(BUILD_DIR)/%.o)

CXX	=	clang++

CXXFLAGS =	--std=c++20 -Wall -Wextra -Isrc

LDFLAGS	=	-lsfml-window -lsfml-graphics -lsfml-system -lm

all: $(BINARY_NAME)

$(BUILD_DIR)/%.o:	src/%.cpp
	@ mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BINARY_NAME): $(OBJ)
	$(CXX) -o $(BINARY_NAME) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(BINARY_NAME)

re:     fclean all