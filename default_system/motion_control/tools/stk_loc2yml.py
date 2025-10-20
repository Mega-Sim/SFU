
loc_file = "../config/stk_shelf_location.loc"

print("  Shelves:")
with open(loc_file) as file:
    for line in file:
        line = line.strip()
        if len(line) == 0: break
        id, data = line.split('=')
        z,x,r,y,p = map(lambda x: x.strip(), data.split(','))
        print("  - { id: %s, z: %s, x: %s, r: %s, y: %s}" % (id, z, x, r, y))
