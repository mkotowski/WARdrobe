--Script generated by GroveW

function pillGreenStart()
end

function pillGreenUpdate(dt)
end

function pillGreenOnCollisionEnter(box)
    if getTag(box) == "player" then
        consume("green")
    end
end