all: libsimmons.so

libsimmons.so: spawn.o
	g++ -shared -static -lc -Wl,-soname,libsimmons.so.1 -o nativelib/libsimmons.so obj/spawn.o 

spawn.o: 
	g++ -DLINUX -I /usr/lib/jvm/java-6-sun/include/ -I/usr/lib/jvm/java-6-sun/include/linux -c -fPIC c_src/com_htssoft_simmons_Spawn.cpp -o obj/spawn.o

javah:
	javah -classpath bin -d c_src com.htssoft.simmons.Spawn

clean:
	rm obj/*
	rm nativelib/*.so
