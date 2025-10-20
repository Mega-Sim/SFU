import sys

if len(sys.argv) != 2 :
	print("Usage:%s <sdo_info_file>" % sys.argv[0])
	print("<sdo_info_file> can be obtained like... /opt/etherlab/bin/ethercat sdos -p<pos>")
	sys.exit(0)

types = {
	'bool': 'CoeValue::BOOL',
	'int8': 'CoeValue::SINT8',
	'int16': 'CoeValue::SINT16',
	'int32': 'CoeValue::SINT32',	
	'uint8': 'CoeValue::UINT8',
	'uint16': 'CoeValue::UINT16',
	'uint32': 'CoeValue::UINT32',	
	'string': 'CoeValue::ARRAY',
	'octet_string':'CoeValue::ARRAY',
	'type': 'CoeValue::UNKNOWN',
}

with open(sys.argv[1]) as f:
	for line in f.readlines():
		line = line.strip()
		if len(line) == 0 or line[:3]=='SDO': continue
		loc, access, typ, bitlen, comment = line.split(',')		
		typ = typ.strip()
		if typ[:4] == 'type': typ = 'type'
		if loc[:2] == '0x':
			index, subindex = loc.split(':')
			print('\t{ %s, 0x%s, %s, %s}, //%s' % (index, subindex, types[typ], bitlen.split()[0], comment))
 
