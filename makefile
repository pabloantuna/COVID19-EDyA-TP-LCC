INTERPRETE = interprete
SRC = main.c
RLIST = rlist
TABLA = tablahash
REGISTRO = registro
GNUPLOT = gnuplot_i
COMP = gcc
CFLAGS 	= -O3 -I
ARGSCOMP = -Wall -Wextra -Werror -std=c99 -g
CFLAGS 	= -O3 -I./src

all: $(INTERPRETE)

$(INTERPRETE): $(RLIST).o $(TABLA).o $(REGISTRO).o $(GNUPLOT).o $(SRC)
	$(COMP) $(ARGSCOMP) -o $(INTERPRETE) $(SRC) $(RLIST).o $(TABLA).o $(REGISTRO).o $(GNUPLOT).o

$(TABLA).o: ./TablaHash/$(TABLA).c $(RLIST).o $(REGISTRO).o
	$(COMP) $(ARGSCOMP) -c ./TablaHash/$(TABLA).c

$(RLIST).o: ./RList/$(RLIST).c $(REGISTRO).o
	$(COMP) $(ARGSCOMP) -c ./RList/$(RLIST).c

$(REGISTRO).o: ./Registro/$(REGISTRO).c
	$(COMP) $(ARGSCOMP) -c ./Registro/$(REGISTRO).c

$(GNUPLOT).o: ./gnuplot_i/$(GNUPLOT).c ./gnuplot_i/$(GNUPLOT).h
	$(COMP) $(CFLAGS) -c ./gnuplot_i/$(GNUPLOT).c

clean:
	rm *.o