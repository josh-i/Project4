targets := libuthread.a
objs    := queue.o disk.o fs.o context.o uthread.o

CC      := gcc 
CFLAGS  := -Werror 
CFLAGS  += -g 

LIBFLAGS = -cvq

ifneq ($(V),1) 
Q = @ 
endif

deps := $(patsubst %.o,%.d,$(objs)) 
-include $(deps)

all: $(targets) $(objs)

libuthread.a: $(objs)
	@echo "ar   $@"
	@ar $(LIBFLAGS) $(targets) *.o
    
%.o: %.c
	@echo "CC   $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $< -MMD -MF $(@:.o=.d)

clean:
	@echo "CLEAN" 
	$(Q)rm -f $(targets) $(objs) $(deps)