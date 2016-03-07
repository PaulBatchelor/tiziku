--require "dummy"
math.randomseed(os.time())
blink = 0

colors = {
    {212, 200, 184, 255},
    {170, 142, 130, 255},
    {100, 121, 82, 255},
    {62, 62, 74, 255},
    {39, 41, 53, 255},
}

nsizes = 5

sizes = {
    {1, 1, 1, 1},
    {1, 2, 3, 4},
    {10, 2, 3, 5},
    {8, 2, 4, 1},
    {0, 2, 3, 0},
}

prev = -1
trig = -1

Square = {}

function Square:new()
    o = { 
        pos = 0, 
        ppos = 0,
        width = 0.25,
        pwidth = 0.25,
        cs = 0.991,
        blnk = 0
    }
    setmetatable(o, self)
    self.__index = self
    return o
end

function Square:newcolor(clr)
    self.clr[1] = clr[1]
    self.clr[2] = clr[2]
    self.clr[3] = clr[3]
    self.clr[4] = clr[4]
end

function Square:initcolor(clr)
    self.clr = {0, 0, 0, 255}
    self.pclr = {0, 0, 0, 255}
end

function Square:draw()
    local cs_b = 1.0 - self.cs
    local cs = self.cs

    local r, g, b, a


    my_pos = cs_b * self.pos + cs * self.ppos
    my_width = cs_b * self.width + cs * self.pwidth
    r = cs_b * self.clr[1] + cs * self.pclr[1]
    g = cs_b * self.clr[2] + cs * self.pclr[2]
    b = cs_b * self.clr[3] + cs * self.pclr[3]
    a = cs_b * self.clr[4] + cs * self.pclr[4] 

    self.ppos = my_pos
    self.pwidth = my_width

    self.pclr[1] = r
    self.pclr[2] = g
    self.pclr[3] = b
    self.pclr[4] = a

    if self.blnk == 0 then 
        rgba(r, g, b, a)
    else
        rgba(0, 0, 0, a)
    end
    rect(
    math.ceil(width * my_pos + 0.5), 
    0, 
    math.ceil(width * my_width + 0.5), 
    height)
end

function getscale(s)
    size = 0
    for n = 1, 4 do
        size = size + s[n]
    end
    return 1.0 / size
end

s = {}
nsquares = 4

time = 0
scale = getscale(sizes[1])
print("the scale is " .. scale)
cpos = 0
for i = 1, nsquares do
    s[i] = Square:new()
    s[i]:initcolor({0, 0, 0})
    color = math.random(4)
    s[i]:newcolor(colors[color])
    if i == 1 then 
        s[i].pos = 0
        s[i].ppos = 0
    else 
        s[i].pos = cpos + scale * sizes[1][i - 1]
        s[i].ppos = s[i].pos
        cpos = cpos + scale * sizes[1][i - 1]
    end
    s[i].width = scale * sizes[1][i]
end

clock = 0
function run()
    trig = get_chan(0)
    cpos = 0
    scale = 0

    if(trig ~= prev and prev ~= -1) then
        prop = math.random(nsizes)
        scale = getscale(sizes[prop])
    end

    clock = (clock + 1) % 3
    if clock == 0 then
        if blink == 1 then blink = 0 else blink = 1 end
    end

    for i = 1, nsquares do
        if(trig ~= prev and prev ~= -1) then
            color = math.random(5)
            s[i]:newcolor(colors[color])
            if i == 1 then 
                s[i].pos = 0
            else 
                s[i].pos = cpos + scale * sizes[prop][i - 1]
                cpos = cpos + scale * sizes[prop][i - 1]
            end
            s[i].width = scale * sizes[prop][i]
        end 
        s[i].blnk = get_chan(i)
        s[i]:draw()
        
    end
    prev = trig
end
