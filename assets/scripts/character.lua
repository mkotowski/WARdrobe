player = {hp = 100, position = {x = 0.0, y = 0.0, z = 0.0}, rotation = {x = 0.0, y = 0.0, z = 0.0}}
mouse = {x, y}

function functionShadyXX()
end

function characterStart()
    prevRightInput = 0.0
    prevDirectionH = 0.0
    dirZeroHCounter = 0
    prevForwardInput = 0.0
    prevDirectionV = 0.0
    dirZeroVCounter = 0
end

function characterUpdate(dt)
    player.position.x, player.position.y, player.position.z = getTransform(entity, componentManager)
    player.rotation.x, player.rotation.y, player.rotation.z = getRotation(entity, componentManager)

    characterSetRotation()

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
    
    moveObject(-directionH * 5.0, 0.0, directionV * 5.0)
    prevRightInput = rightInput
    prevForwardInput = forwardInput
end

function characterSetRotation()
    local mouseWorldX, mouseWorldZ = getMouseWorldPos(window, camera)

    local deltaX = mouseWorldX
    local deltaZ = mouseWorldZ

    local angle = math.atan(deltaZ, deltaX) * 180.0 / 3.14;

    setRotation(entity, componentManager, 0.0, -angle + 90.0, 0.0)
end