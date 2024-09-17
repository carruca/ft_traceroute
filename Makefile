NAME    	= ft_traceroute

SRCSPATH	= src/
OBJSPATH	= obj/
SRCSFILES	= \
						ft_traceroute.c 		\
						init.c 		\
						run.c 		\
						recv.c 		\
						xmit.c 		\
						utils.c
SRCS			= $(addprefix $(SRCSPATH), $(SRCSFILES))
OBJS			= $(patsubst $(SRCSPATH)%, $(OBJSPATH)%, $(SRCS:.c=.o))

CC      	= gcc
CFLAGS  	= -Wall -Werror -Wextra
INC 			= -I.
LDFLAGS 	= -lm
FSANITIZE = -fsanitize=address -g3

all: $(NAME)

$(OBJSPATH)%.o: $(SRCSPATH)%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME) $(LDFLAGS)

SILENT += print
print:
	echo $(SRCS)
	echo $(OBJS)

config: $(NAME)
	sudo chown root:root $(NAME)
	sudo chmod u+s $(NAME)

sanitize: LDFLAGS += $(FSANITIZE)
sanitize: $(NAME)

tags:
	ctags $(SRCS)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.SILENT: $(SILENT)
