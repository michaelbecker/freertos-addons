# DETECT OS
OS := $(shell uname)

LDFLAGS += -Wall -Werror -Wextra -Wpedantic -O0 -g

ifneq ($(OS), Darwin)
# You cannot use the -pthread flag when linking in Darwin
LDFLAGS += -pthread
endif