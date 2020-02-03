
using TestImages, Colors, ImageView, Images
filename = ARGS[2]
filename = "jpeg/qr-codes-xavier.jpg"
if (!isfile(filename))
    println("File not found!")
end

img = load(filename)


imgg = Gray.(img)
fileout = string(splitext(filename)[1], ".", size(imgg)[2], "x", size(imgg)[1], ".raw")



io = open(fileout,"w")
x = reinterpret(UInt8, imgg')
write(io, x) 
close(io)


io = open(fileout,"r");
y = read!(io, Array{UInt8,2}(undef, 474, 632))'
close(io)
