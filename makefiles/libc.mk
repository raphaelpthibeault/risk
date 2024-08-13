include Make.defaults

TARGET := $(BINDIR)/libc.a

all: $(TARGET)

.PHONY: all

include Make.rules

