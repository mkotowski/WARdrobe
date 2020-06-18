player = {entity = 0, health = 100.0, position = {x = 0.0, y = 0.0, z = 0.0}, rotation = {x = 0.0, y = 0.0, z = 0.0}}
mouse = {x, y}

highTimeStamp = 0.0
highTime = 0.0
isHigh = false
dead = false
hitTimeStamp = 0.0

function characterStart()
    prevRightInput = 0.0
    prevDirectionH = 0.0
    dirZeroHCounter = 0
    prevForwardInput = 0.0
    prevDirectionV = 0.0
    dirZeroVCounter = 0
    player.entity = entity
end

function characterUpdate(dt)
    if dead == true then
        return
    end

    player.position.x, player.position.y, player.position.z = getTransform(entity, componentManager)
    player.rotation.x, player.rotation.y, player.rotation.z = getRotation(entity, componentManager)

    if isHigh == true and time - highTimeStamp > highTime then
        isHigh = false
        
        leftFist.damage = lightDamage
        rightFist.damage = lightDamage
        setSubroutine(modelShader, animatedModelShader, componentManager, "ColorWhite")
    end

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

function getHigh(type)
    if type == "red" then
        leftFist.damage = leftFist.damage * 2.0
        rightFist.damage = rightFist.damage * 2.0
        isHigh = true
        highTimeStamp = time
        highTime = 5.0
        setSubroutine(modelShader, animatedModelShader, componentManager, "ColorRed")
    end
end

function playerGetHit(dmg)

    if time - hitTimeStamp < hitInterval then
        return
    end

    hitTimeStamp = time
    player.health = player.health - dmg

    if player.health <= 0.0 then
        playerDie()
    end
end

function playerDie()
    dead = true
    setColor(entity, componentManager, 0.0, 0.0, 0.0)
end

function playerPlayAnim(name)
    playAnimation(player.entity, componentManager, name)
end