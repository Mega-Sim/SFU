#!/opt/anaconda3/bin/python
import xmltodict, yaml, sys

if len(sys.argv) != 2:
    print("Usage)%s xmlfile_without_ext\n" % sys.argv[0])
    sys.exit(1)

filename = sys.argv[1]

try:
    with open('%s.xml'%filename, 'rb') as f :
        d = xmltodict.parse(f, dict_constructor=dict)

    with open('%s.yml'%filename, 'w') as f :
        f.write(yaml.dump(d))
except:
    print("Fail to convert %s.xml to %s.yml" %(filename, filename))