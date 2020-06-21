maxHealth = 500.0
player = {entity = 0, health = maxHealth, position = {x = 0.0, y = 0.0, z = 0.0}, rotation = {x = 0.0, y = 0.0, z = 0.0}}
mouse = {x, z}

bluePillPower = 2.0
redPillPower = 2.0

damageHighTimeStamp = 0.0
speedHighTimeStamp = 0.0
healHighTimeStamp = 0.0
damageHighTime = 20.0
speedHighTime = 10.0
healHighTime = 3.0
isHighDamage = false
isHighSpeed = false
isHighHeal = false
dead = false

normalPlayerSpeed = 10.0
playerSpeed = normalPlayerSpeed

brainDamage = 0.0

damageMultiplier = 1.0
speedMultiplier = 1.0

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

    handleBrain(dt)

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

    if detectedCombo == "dash" then
        return
    end
    
    moveObject(-directionH * playerSpeed, 0.0, directionV * playerSpeed)
    prevRightInput = rightInput
    prevForwardInput = forwardInput
end

function handleBrain(dt)
    if brainDamage > 0.0 then
        if brainDamage >= 100.0 then
            playerGetHit(0.005 * maxHealth * dt)
        end

        brainDamage = brainDamage - (5.0 * dt)

        if brainDamage < 0.0 then
            brainDamage = 0.0
        end

        --updateBrainUI
    end

    if isHighHeal == true and time - healHighTimeStamp > healHighTime then
        isHighHeal = false
    end

    if isHighDamage == true and time - damageHighTimeStamp > damageHighTime then
        isHighDamage = false
        damageMultiplier = 1.0
    end

    if isHighSpeed == true and time - speedHighTimeStamp > speedHighTime then
        isHighSpeed = false
        speedMultiplier = 1.0
        playerSpeed = normalPlayerSpeed
    end

    if isHighDamage == false and isHighHeal == false and isHighSpeed == false then
        setSubroutine(modelShader, animatedModelShader, componentManager, "ColorWhite")
    elseif isHighDamage == true and isHighHeal == false and isHighSpeed == false then
        setSubroutine(modelShader, animatedModelShader, componentManager, "ColorRed")
    elseif isHighDamage == false and isHighHeal == true and isHighSpeed == false then
        setSubroutine(modelShader, animatedModelShader, componentManager, "ColorWavy")
    elseif isHighDamage == false and isHighHeal == false and isHighSpeed == true then
        setSubroutine(modelShader, animatedModelShader, componentManager, "ColorCustom")
    end
end

function characterSetRotation()
    mouse.x, mouse.z = getMouseWorldPos(window, camera)

    local deltaX = mouse.x
    local deltaZ = mouse.z

    local angle = math.atan(deltaZ, deltaX) * 180.0 / 3.14;

    setRotation(entity, componentManager, 0.0, -angle + 90.0, 0.0)
end

function getHigh(type)
    if type == "red" then
        isHighDamage = true
        damageMultiplier = redPillPower * damageMultiplier
        leftFist.damage = leftFist.damage * damageMultiplier
        rightFist.damage = rightFist.damage * damageMultiplier
        brainDamage = brainDamage + 25.0
        damageHighTimeStamp = time
        setSubroutine(modelShader, animatedModelShader, componentManager, "ColorRed")
    elseif type == "green" then
        isHighHeal = true
        player.health = player.health + (maxHealth - player.health)
        healHighTimeStamp = time
        setSubroutine(modelShader, animatedModelShader, componentManager, "ColorWavy")
    elseif type == "blue" then
        isHighSpeed = true
        speedMultiplier = bluePillPower * speedMultiplier
        playerSpeed = playerSpeed * speedMultiplier
        brainDamage = brainDamage + 25.0
        speedHighTimeStamp = time
        setSubroutine(modelShader, animatedModelShader, componentManager, "ColorCustom")
    end
end

function playerGetHit(dmg)
    player.health = player.health - dmg

    --updateHealthUI

    if player.health <= 0.0 then
        playerDie()
    end
end

function playerDie()
    dead = true
    print("RIP")
    setColor(entity, componentManager, 0.0, 0.0, 0.0)
end

function playerPlayAnim(name)
    playAnimation(player.entity, componentManager, name)
end