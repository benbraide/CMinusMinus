#include <iostream>

#include "io/io_stream_writer.h"
#include "memory/memory_object.h"

int main(){
	struct test{};

	test ti;
	sizeof(test);
	sizeof(ti);

	cminus::io::stream_writer sw(std::cout);
	sw.write_scalar(0);

	cminus::io::tabbed_stream_writer<cminus::io::wide_stream_writer> wsw(std::wcout);
	wsw.write_scalar('b');
	wsw.write_buffer("en10");

	return 0;
}