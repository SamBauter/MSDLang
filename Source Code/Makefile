INCS = Cont.hpp Env.hpp Expr.h ExprParser.hpp op.hpp pointer.hpp Step.hpp Value.hpp catch.hpp


LIBOBJS = Cont.o Env.o Expr.o ExprParser.o op.o pointer.o Step.o Value.o
OBJS = main.o $(LIBOBJS)

CXXFLAGS = --std=c++11 -O2

all: msdscript libmsdscript.a

msdscript: $(OBJS)
	$(CXX) $(CXXFLAGS) -o msdscript $(OBJS)

libmsdscript.a: $(LIBOBJS)
	$(AR) $(ARFLAGS) libmsdscript.a $(LIBOBJS)

main.o: $(INCS)

Cont.o: $(INCS)

Env.o: $(INCS)

Expr.o: $(INCS)

ExprParser.o: $(INCS)

op.o: $(INCS)

pointer.o: $(INCS)

Step.o: $(INCS)

Value.o: $(INCS)








