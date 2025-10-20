
port_list = []
board_id = -1
with open("../../ecmaster/emul/oht/oht_io_map.h") as f:
	for line in f.readlines():
		data = line.strip().split();
		if len(data) == 0 or data[0] != 'unsigned':
			if "__attribute__((packed))" in line: 
				pass
			elif  len(data)>2 and data[0] == "struct":
				board_id += 1
				print "        //", data[1][:-2], "DO" if data[1][-2:] == "DI" else "DI"
			else:
				print line,
			continue 
		if data[1][0] == ":": # unused bitfield
			continue 
		out = "            "
		id_name = data[1].split(':')[0]
		port_list.append(id_name)
		port_id = data[2][3:6]
		comment_index = 3
		if len(port_id) == 0:
			port_id  = data[3][1:4]		
			command_index = 4
		port_id = "(%d,%s)" %(board_id, port_id)
		out += "%-28s = make_port_id%s," % (id_name, port_id)
		if len(data) > comment_index:
			out += "  //" + " ".join(data[4:])
		print out 

with open("oht_io_port_map_test.cpp", "w") as f:
	f.write("""
const char* getOhtIoPortString(unsigned portid) 
{
	switch(portid){
%s
	}
}""" % '\n'.join(map(lambda x : '    case %-32s return "%s";' % (x+":",x), port_list)))
