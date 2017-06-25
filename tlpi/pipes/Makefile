include ../Makefile.inc

GEN_EXE = change_case fifo_seqnum_client fifo_seqnum_server \
	pipe_ls_wc pipe_sync popen_glob simple_pipe \
	fifo_seqnum_server_44-3 fifo_seqnum_server_44-4 \
	fifo_seqnum_server_44-6 fifo_seqnum_client_44-6

EXE = ${GEN_EXE} ${LINUX_EXE}

all : ${EXE}

allgen : ${GEN_EXE}

fifo_seqnum_client.o fifo_seqnum_server.o : fifo_seqnum.h

clean : 
	${RM} ${EXE} *.o

showall :
	@ echo ${EXE}

${EXE} : ${TLPI_LIB}		# True as a rough approximation
