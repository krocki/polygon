.SUFFIXES:
TARGETS=poly canvas

CC=gcc
CFLAGS=-g -Wfatal-errors -O0 -Wall -Wfatal-errors
DEPS:=$(wildcard *.h) Makefile
LFLAGS=-lm

OS:=$(shell uname)
ifeq ($(OS),Darwin) # Mac OS
  GL_FLAGS=-lglfw -framework OpenGL -lpthread
  CFLAGS:=$(CFLAGS) -DAPPLE -DGL_SILENCE_DEPRECATION
else # Linux or other
  GL_FLAGS=-lglfw -lGL -lpthread
  LFLAGS:=$(LFLAGS) -L/usr/lib -L/usr/lib64
endif

LFLAGS:=$(LFLAGS) $(GL_FLAGS)

#########

all: $(TARGETS) $(DEPS)

#%.o: %.c $(DEPS)
#	$(CC) $(CFLAGS) -c $< -o $@

%: %.c $(DEPS)
	$(CC) $(CFLAGS) $< -o $@ $(LFLAGS)

clean:
	rm -rf *.o $(TARGETS)
