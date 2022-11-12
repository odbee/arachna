
def hex_to_rgb(hex):
    if (hex[0]=='#'):
        hex = hex[1:]
    result=tuple(int(hex[i:i+2], 16) for i in (0, 2, 4)) 
    
    return result

def print_h2r(tup):
    print("{ %4.2ff, %4.2ff, %4.2ff,1.0f} " % (float(tup[0])/255,float(tup[1])/255,float(tup[2])/255))


print_h2r(hex_to_rgb("#FFC800"))
print(hex_to_rgb("#FFC800"))
while (True):
    val = input("hex val:")
    print_h2r(hex_to_rgb(val))
