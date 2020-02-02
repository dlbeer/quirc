using TestImages, Colors, ImageView, Images
img = testimage("mandrill")

imgg = Gray.(img)
imshow(imgg)

io = open("test.raw","w")
write(io, reinterpret(UInt, imgg')) 
close(io)