CC = gcc
OUT = not_working_game_exe

SRCDIR	= src/
INCDIR	= inc/
OBJDIR	= .obj/

CFILES   = $(wildcard $(SRCDIR)/*.c)

COBJS = $(patsubst $(SRCDIR)%.c,$(OBJDIR)%.o,$(CFILES))

DEPS = $(COBJS:.o=.d) $(CPPOBJS:.o=.d)

CFLAGS = -g -Wall -pthread -I$(INCDIR) -MP -MD
LDLIBS	 = -lraylib -lglfw -lGL -lm -lpthread -ldl -lrt

all: $(OBJDIR) $(OUT)

$(OUT): $(COBJS)
	$(CC) $(CFLAGS) -o $(OUT) $(COBJS) $(LDLIBS)

-include $(DEPS)

$(COBJS):
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $(patsubst $(OBJDIR)%.o,$(SRCDIR)%.c,$(@)) -o $(@)

$(OBJDIR):
	mkdir -p $@
.PHONY: clean
clean:
	rm -f $(COBJS) $(DEPS) $(OUT)
