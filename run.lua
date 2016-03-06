phs = 0
function run()
    size = 11
    for y = 1, 64 do
        for x = 1, 64 do
            test(10 + x * size, 10 + y * size, size * 0.5 * math.cos(phs + 0.1 * x))
        end
    end
    phs = phs + 0.1
    phs = math.fmod(phs, 2.0 * math.pi)
    --print(phs)
end
