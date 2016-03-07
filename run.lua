--require "dummy"
math.randomseed(os.time())

colors = {
    {212, 200, 184, 255},
    {170, 142, 130, 255},
    {100, 121, 82, 255},
    {62, 62, 74, 255},
    {39, 41, 53, 255},
}

prev = -1
trig = -1

Square = {}

function Square:new()
    o = { 
        pos = 0, 
        width = 0.25,
        cs = 0.996,
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

    if(trig ~= prev and prev ~= -1) then
        color = math.random(4)
        self:newcolor(colors[color])
    end 

    r = cs_b * self.clr[1] + cs * self.pclr[1]
    g = cs_b * self.clr[2] + cs * self.pclr[2]
    b = cs_b * self.clr[3] + cs * self.pclr[3]
    a = cs_b * self.clr[4] + cs * self.pclr[4]

    self.pclr[1] = r
    self.pclr[2] = g
    self.pclr[3] = b
    self.pclr[4] = a

    rgba(r, g, b, a)
    rect(
    math.ceil(width * self.pos + 0.5), 
    0, 
    math.ceil(width * self.width + 0.5), 
    height)
end

s = {}
nsquares = 4
res = 1.0 / nsquares

time = 0

for i = 1, nsquares do
    s[i] = Square:new()
    s[i]:initcolor({0, 0, 0})
    color = math.random(4)
    s[i]:newcolor(colors[color])
    s[i].pos = (i - 1) * res
    s[i].width = res
end

function run()
    trig = get_chan(0)
    for i = 1, nsquares do
        s[i]:draw()
    end
    prev = trig
end
