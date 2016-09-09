include Make.defines

PROGS =	 client server

OPTIONS = -DUNIX  -DANSI


COBJECTS =	DieWithError.o 
CSOURCES =	DieWithError.c 

CPLUSOBJECTS = 

COMMONSOURCES =

CPLUSSOURCES =

all:	${PROGS}


client:		UDPEchoClient2.o $(CPLUSOBJECTS) $(COBJECTS) $(LIBS) $(COMMONSOURCES) $(SOURCES)
		${CC} ${LINKOPTIONS}  $@ UDPEchoClient2.o $(CPLUSOBJECTS) $(COBJECTS) $(LIBS) $(LINKFLAGS)

server:		UDPEchoServer.o $(CPLUSOBJECTS) $(COBJECTS)
		${CC} ${LINKOPTIONS} $@ UDPEchoServer.o $(CPLUSOBJECTS) $(COBJECTS) $(LIBS) $(LINKFLAGS)


.cc.o:	$(HEADERS)
	$(CPLUS) $(CPLUSFLAGS) $(OPTIONS) $<

.c.o:	$(HEADERS)
	$(CC) $(CFLAGS) $(OPTIONS) $<



backup:
	rm -f UDPEcho2REL.tar
	rm -f UDPEcho2REL.tar.gz
	tar -cf UDPEcho2REL.tar *
	gzip -f UDPEcho2REL.tar

clean:
		rm -f ${PROGS} ${CLEANFILES}

depend:
		makedepend UDPEchoClient1.c UDPEchoClient2.c UDPEchoServer.c $(CFLAGS) $(HEADERS) $(SOURCES) $(COMMONSOURCES) $(CSOURCES)
#		mkdep $(CFLAGS) $(HEADERS) $(SOURCES) $(COMMONSOURCES) $(CSOURCES)

# DO NOT DELETE

UDPEchoClient2.o: UDPEcho.h /usr/include/stdio.h /usr/include/features.h
UDPEchoClient2.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
UDPEchoClient2.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
UDPEchoClient2.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
UDPEchoClient2.o: /usr/include/libio.h /usr/include/_G_config.h
UDPEchoClient2.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
UDPEchoClient2.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
UDPEchoClient2.o: /usr/include/bits/waitflags.h
UDPEchoClient2.o: /usr/include/bits/waitstatus.h /usr/include/endian.h
UDPEchoClient2.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
UDPEchoClient2.o: /usr/include/bits/byteswap-16.h /usr/include/sys/types.h
UDPEchoClient2.o: /usr/include/time.h /usr/include/sys/select.h
UDPEchoClient2.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
UDPEchoClient2.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
UDPEchoClient2.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
UDPEchoClient2.o: /usr/include/bits/stdlib-float.h /usr/include/errno.h
UDPEchoClient2.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
UDPEchoClient2.o: /usr/include/asm/errno.h /usr/include/asm-generic/errno.h
UDPEchoClient2.o: /usr/include/asm-generic/errno-base.h /usr/include/string.h
UDPEchoClient2.o: /usr/include/xlocale.h /usr/include/netinet/in.h
UDPEchoClient2.o: /usr/include/stdint.h /usr/include/bits/wchar.h
UDPEchoClient2.o: /usr/include/sys/socket.h /usr/include/sys/uio.h
UDPEchoClient2.o: /usr/include/bits/uio.h /usr/include/bits/socket.h
UDPEchoClient2.o: /usr/include/bits/socket_type.h
UDPEchoClient2.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
UDPEchoClient2.o: /usr/include/asm-generic/socket.h
UDPEchoClient2.o: /usr/include/asm/sockios.h
UDPEchoClient2.o: /usr/include/asm-generic/sockios.h /usr/include/bits/in.h
UDPEchoClient2.o: /usr/include/arpa/inet.h /usr/include/unistd.h
UDPEchoClient2.o: /usr/include/bits/posix_opt.h
UDPEchoClient2.o: /usr/include/bits/environments.h
UDPEchoClient2.o: /usr/include/bits/confname.h /usr/include/getopt.h
UDPEchoClient2.o: /usr/include/netdb.h /usr/include/rpc/netdb.h
UDPEchoClient2.o: /usr/include/bits/netdb.h /usr/include/signal.h
UDPEchoClient2.o: /usr/include/bits/signum.h /usr/include/bits/siginfo.h
UDPEchoClient2.o: /usr/include/bits/sigaction.h
UDPEchoClient2.o: /usr/include/bits/sigcontext.h /usr/include/bits/sigstack.h
UDPEchoClient2.o: /usr/include/sys/ucontext.h /usr/include/bits/sigthread.h
UDPEchoClient2.o: /usr/include/sys/time.h
UDPEchoServer.o: UDPEcho.h /usr/include/stdio.h /usr/include/features.h
UDPEchoServer.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
UDPEchoServer.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
UDPEchoServer.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
UDPEchoServer.o: /usr/include/libio.h /usr/include/_G_config.h
UDPEchoServer.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
UDPEchoServer.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
UDPEchoServer.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
UDPEchoServer.o: /usr/include/endian.h /usr/include/bits/endian.h
UDPEchoServer.o: /usr/include/bits/byteswap.h /usr/include/bits/byteswap-16.h
UDPEchoServer.o: /usr/include/sys/types.h /usr/include/time.h
UDPEchoServer.o: /usr/include/sys/select.h /usr/include/bits/select.h
UDPEchoServer.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
UDPEchoServer.o: /usr/include/sys/sysmacros.h
UDPEchoServer.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
UDPEchoServer.o: /usr/include/bits/stdlib-float.h /usr/include/errno.h
UDPEchoServer.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
UDPEchoServer.o: /usr/include/asm/errno.h /usr/include/asm-generic/errno.h
UDPEchoServer.o: /usr/include/asm-generic/errno-base.h /usr/include/string.h
UDPEchoServer.o: /usr/include/xlocale.h /usr/include/netinet/in.h
UDPEchoServer.o: /usr/include/stdint.h /usr/include/bits/wchar.h
UDPEchoServer.o: /usr/include/sys/socket.h /usr/include/sys/uio.h
UDPEchoServer.o: /usr/include/bits/uio.h /usr/include/bits/socket.h
UDPEchoServer.o: /usr/include/bits/socket_type.h /usr/include/bits/sockaddr.h
UDPEchoServer.o: /usr/include/asm/socket.h /usr/include/asm-generic/socket.h
UDPEchoServer.o: /usr/include/asm/sockios.h
UDPEchoServer.o: /usr/include/asm-generic/sockios.h /usr/include/bits/in.h
UDPEchoServer.o: /usr/include/arpa/inet.h /usr/include/unistd.h
UDPEchoServer.o: /usr/include/bits/posix_opt.h
UDPEchoServer.o: /usr/include/bits/environments.h
UDPEchoServer.o: /usr/include/bits/confname.h /usr/include/getopt.h
DieWithError.o: /usr/include/stdio.h /usr/include/features.h
DieWithError.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
DieWithError.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
DieWithError.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
DieWithError.o: /usr/include/libio.h /usr/include/_G_config.h
DieWithError.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
DieWithError.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
DieWithError.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
DieWithError.o: /usr/include/endian.h /usr/include/bits/endian.h
DieWithError.o: /usr/include/bits/byteswap.h /usr/include/bits/byteswap-16.h
DieWithError.o: /usr/include/sys/types.h /usr/include/time.h
DieWithError.o: /usr/include/sys/select.h /usr/include/bits/select.h
DieWithError.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
DieWithError.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
DieWithError.o: /usr/include/alloca.h /usr/include/bits/stdlib-float.h
