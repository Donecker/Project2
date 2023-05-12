# Target library
lib := libuthread.a

# Compiler
CC = gcc

# Flags
CFLAGS = -Wall -Wextra -Werror -MMD

# Extra for debugging, remove this for submission
CFLAGS += -g

# Objects
objs := queue.o uthread.o context.o preempt.o sem.o 

#Format message
ifneq ($(V),1)
Q = @
endif

#All makes should require lib
all: $(lib)

# Dependency tracking
deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

# Lib should require objects in the object variable
$(lib): $(objs)
	@echo "AR $@"
	$(Q)ar rcs $@ $^

# Objects reqruire C files
%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

# Remove all generated files
clean:
	@echo "clean"
	$(Q)rm -f $(targets) $(objs) $(deps) $(lib)
