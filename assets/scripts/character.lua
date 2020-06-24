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
healSlot = "slot"
speedSlot = "slot"
damageSlot = "slot"

dead = false

hitCo = {}
hitCoPlaying = false

normalPlayerSpeed = 10.0
playerSpeed = normalPlayerSpeed

brainDamage = 0.0

damageMultiplier = 1.0
speedMultiplier = 1.0

function characterStart()
    setIndicator(window, "speed_pill_indicator", false)
    setIndicator(window, "damage_pill_indicator", false)
    setIndicator(window, "health_pill_indicator", false)
    setHead(window, 100, false)
    player.entity = entity
end

function characterUpdate(dt)
    handleHead(dt)

    if dead == true then
        return
    end

    player.position.x, player.position.y, player.position.z = getTransform(entity, componentManager)
    player.rotation.x, player.rotation.y, player.rotation.z = getRotation(entity, componentManager)

    handleBrain(dt)

    characterSetRotation()

    prevDirectionV = directionV

    if detectedCombo == "dash" then
        return
    end
    
    handleMovement()
    prevRightInput = rightInput
    prevForwardInput = forwardInput
end

function handleMovement()
    local direction = {x = 0.0, y = 0.0, z = 0.0}

    if forwardInput > 0.0 then
        direction.x = -mouse.x
        direction.z = mouse.z
    elseif forwardInput < 0.0 then
        direction.x = mouse.x
        direction.z = -mouse.z
    end

    if rightInput > 0.0 then
        direction.x = direction.x + mouse.z
        direction.z = direction.z + mouse.x
    elseif rightInput < 0.0 then
        direction.x = direction.x - mouse.z
        direction.z = direction.z - mouse.x
    end

    if direction.x ~= 0.0 or direction.z ~= 0.0 then
        direction.x, direction.y, direction.z = normalize(direction.x, direction.y, direction.z)
    end

    moveObject(-direction.x * playerSpeed, direction.y * playerSpeed, direction.z * playerSpeed)
end

function handleHead(dt)
    if hitCoPlaying == true then
        coroutine.resume(hitCo, dt)
    end
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
        equipment[healSlot] = nil
        setIndicator(window, "health_pill_indicator", false)
    end

    if isHighDamage == true and time - damageHighTimeStamp > damageHighTime then
        isHighDamage = false
        damageMultiplier = 1.0
        equipment[damageSlot] = nil
        setIndicator(window, "damage_pill_indicator", false)
    end

    if isHighSpeed == true and time - speedHighTimeStamp > speedHighTime then
        isHighSpeed = false
        speedMultiplier = 1.0
        playerSpeed = normalPlayerSpeed
        equipment[speedSlot] = nil
        setIndicator(window, "speed_pill_indicator", false)
    end

    if isHighDamage == false and isHighHeal == false and isHighSpeed == false then
        setSubroutine(quadShader, componentManager, "ColorWhite")
    elseif isHighDamage == true and isHighHeal == false and isHighSpeed == false then
        setSubroutine(quadShader, componentManager, "ColorRed")
    elseif isHighDamage == false and isHighHeal == true and isHighSpeed == false then
        setSubroutine(quadShader, componentManager, "ColorWavy")
    elseif isHighDamage == false and isHighHeal == false and isHighSpeed == true then
        setSubroutine(quadShader, componentManager, "ColorCustom")
    end
end

function characterSetRotation()
    mouse.x, mouse.z = getMouseWorldPos(window, camera)

    local deltaX = mouse.x
    local deltaZ = mouse.z

    local angle = math.atan(deltaZ, deltaX) * 180.0 / 3.14;

    setRotation(entity, componentManager, 0.0, -angle + 90.0, 0.0)
end

function getHigh(slot, type)
    if type == "red" then
        isHighDamage = true
        damageMultiplier = redPillPower * damageMultiplier
        leftFist.damage = leftFist.damage * damageMultiplier
        rightFist.damage = rightFist.damage * damageMultiplier
        brainDamage = brainDamage + 25.0
        damageHighTimeStamp = time
        damageSlot = slot
        setIndicator(window, "damage_pill_indicator", true)
        setSubroutine(quadShader, componentManager, "ColorRed")
    elseif type == "green" then
        isHighHeal = true
        player.health = player.health + (maxHealth - player.health)
        setHead(window, 100, false)
        healHighTimeStamp = time
        healSlot = slot
        setIndicator(window, "health_pill_indicator", true)
        setSubroutine(quadShader, componentManager, "ColorWavy")
    elseif type == "blue" then
        isHighSpeed = true
        speedMultiplier = bluePillPower * speedMultiplier
        playerSpeed = playerSpeed * speedMultiplier
        brainDamage = brainDamage + 25.0
        speedHighTimeStamp = time
        speedSlot = slot
        setIndicator(window, "speed_pill_indicator", true)
        setSubroutine(quadShader, componentManager, "ColorCustom")
    end
end

function playerGetHit(dmg)
    player.health = player.health - dmg

    --print("hit", dmg)

    setHead(window, player.health / maxHealth * 100.0, true)

    if hitCoPlaying == false then
        hitCoPlaying = true
        hitCo = coroutine.create(hitCoroutine)
        coroutine.resume(hitCo, 0.0)
    end

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

function hitCoroutine(dt)
    local timeStamp = time

    while time - timeStamp < 1.0 do
        dt = coroutine.yield()
    end

    setHead(window, player.health / maxHealth * 100.0, false)
    hitCoPlaying = false
end

function playerPlayAnim(name)
    playAnimation(player.entity, componentManager, name)
end