include ../Makefile.inc

GEN_EXE = demo_sigio poll_pipes select_mq self_pipe t_select \
		  poll_pipes_63-1 self_pipe_63-5 demo_sigio_63-8

LINUX_EXE = epoll_input

EXE = ${GEN_EXE} ${LINUX_EXE}

all : ${EXE}

allgen : ${GEN_EXE}

clean : 
	${RM} ${EXE} *.o

showall :
	@ echo ${EXE}

${EXE} : ${TLPI_LIB}		# True as a rough approximation
