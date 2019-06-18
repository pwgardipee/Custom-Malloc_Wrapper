CC = gcc 
WARNING_FLAGS = -Wall -Wextra -g -O0
EXE = Prog4Test
SCAN_BUILD_DIR = scan-build-out
#NAME = advanced_testcase4

all: 537malloc.o range_tree.o rb_tree.o $(NAME).o
	$(CC) -o $(EXE) 537malloc.o range_tree.o rb_tree.o $(NAME).o


# main.c is your testcase file name
$(NAME).o: $(NAME).c 537malloc.h
	$(CC) $(WARNING_FLAGS) -c $(NAME).c 

# Include all your .o files in the below rule
obj: 537malloc.o range_tree.o rb_tree.o


537malloc.o: 537malloc.c 537malloc.h range_tree.h
	$(CC) $(WARNING_FLAGS) -c 537malloc.c

range_tree.o: range_tree.c range_tree.h rb_tree.h
	$(CC) $(WARNING_FLAGS) -c range_tree.c

rb_tree.o: rb_tree.c rb_tree.h
	$(CC) $(WARNING_FLAGS) -c rb_tree.c

	
clean:
	rm $(EXE) *.o
	rm -rf $(SCAN_BUILD_DIR)

#
# Run the Clang Static Analyzer
#
scan-build: clean
	scan-build -o $(SCAN_BUILD_DIR) make

#
# View the one scan available using firefox
#
scan-view: scan-build
	firefox -new-window $(SCAN_BUILD_DIR)/*/index.html 
