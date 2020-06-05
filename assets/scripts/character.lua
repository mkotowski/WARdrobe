position = {x, y, z}

enemies = { {hp = 1.0, some = 2.0}}

function functionShadyXX()
    print("la cucaracha la cucharacha \n lal lallalla")
end

function characterStart()
    print("CharacterSTART")

    print("enemies 1 : " .. enemies[1].hp .. " " .. enemies[1].some )

    local enemy = {hp = 2.0, some = 225.0}

    table.insert(enemies, enemy)

    print("enemies 1 : " .. enemies[2].hp .. " " .. enemies[2].some )

    position.x, position.y, position.z = getTransform(entity, componentManager)

    prevRightInput = 0.0
    prevDirectionH = 0.0
    dirZeroHCounter = 0
    prevForwardInput = 0.0
    prevDirectionV = 0.0
    dirZeroVCounter = 0
end

function characterUpdate(dt)
    directionH = 0.0
    directionV = 0.0

    if rightInput - prevRightInput < 0 then
        directionH = -1.0
    elseif rightInput - prevRightInput > 0 then
        directionH = 1.0
    end

    if directionH == 0.0 and dirZeroHCounter == 1 then
        dirZeroHCounter = 0
    elseif directionH == 0.0 and prevDirectionH ~= 0.0 then
        dirZeroHCounter = 1
        directionH = prevDirectionH
    elseif directionH ~= 0.0 then
        dirZeroHCounter = 0
    end

    prevDirectionH = directionH

    if forwardInput - prevForwardInput < 0 then
        directionV = -1.0
    elseif forwardInput - prevForwardInput > 0 then
        directionV = 1.0
    end

    prevDirectionV = directionV
    
    moveObject(directionH * 10.0 * dt, directionV * 10.0 * dt, 0.0)
    prevRightInput = rightInput
    prevForwardInput = forwardInput
end

function characterOnCollisionEnter(tag)
end