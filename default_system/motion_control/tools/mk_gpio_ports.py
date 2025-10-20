
gpio_in = []
gpio_out = []

with open("master_gpio.txt") as gpio_file :
    for line in gpio_file.readlines():
        if not 'GPIO' in line : continue
        line = line.strip()
        words = line.split()
        portname = words[-1].lower()
        if 'GPIO' in words[-3]:
            x,y = words[-3][4:].split('_') # GPIOx_y
        else:
            x, y = words[-2][4:].split('_')
        if 'INPUT' in line or 'DI' in words:
            gpio_in.append((portname, x, y))
        else:
            gpio_out.append((portname, x, y))

print("DI count = %d, DO count = %d\n" % ( len(gpio_in), len(gpio_out)))
print("Comma version")
print("//... out...")
for i,p in enumerate(gpio_out):
    print('std::make_pair(%s,%d),' % (p[0], i))
print("//... in...")
for i,p in enumerate(gpio_in):
    print('std::make_pair(%s,%d),' % (p[0], i))

print("Quotation version")
print("... out...")
print(",\n".join(map(lambda x : '"%s"' % x[0] , gpio_out)))
print("... in...")
print(",\n".join(map(lambda x : '"%s"' % x[0] , gpio_in)))

print("Get String...")
for p in gpio_out + gpio_in:
    print('        case %s: return "%s";' % (p[0], p[0]))

# output port
print("            // OhtOnboard DO")
for port in gpio_out:
    print('            %-30s = makePortId(0,%s,%s),' % (port[0], port[1], port[2]))

# input port
print("            //OhtOnboard DI")
for port in gpio_in:
    print('            %-30s = makePortId(1,%s,%s),' % (port[0], port[1], port[2]))


print("""        // digital input bit field; input is machine's view, that is, controller's output
        struct OhtOnboardDI {""")
for port in gpio_out:
    print('            unsigned %s:1;' % (port[0]))
print("        }__attribute__((packed));\n")

print(""" 
        struct OhtOnboardDO {""")
for port in gpio_in:
    print('            unsigned %s:1;' % (port[0]))
print("        }__attribute__((packed));\n")



print("# onboard DO")
for i,port in enumerate(gpio_in):
    print('printf("[%2d]%-20s:'%(i,port[0]), '%d\\n",', 'onboard_do_->%s);' % port[0] )
print("# onboard DI")
for i,port in enumerate(gpio_out):
    print('printf("[%2d]%-20s:'%(i,port[0]), '%d\\n",', 'onboard_di_->%s);' % port[0] )
