--Script generated by GroveW

leftFist = {positionRel = {x = 1.0, y = 0.0, z = 3.0}, attackTimeStamp = 0.0, pressed = false, pressedTimeStamp = 0.0,
 isAttacking = false, coroutine = {}, damage = lightDamage}
rightFist = {positionRel = {x = -1.0, y = 0.0, z = 3.0}, attackTimeStamp = 0.0, pressed = false, pressedTimeStamp = 0.0,
 isAttacking = false, coroutine = {}, damage = lightDamage}

fists = {} --[1] for left, [2] for right

defaultDepth = 0.0

combos = { ["whirlwind"] = {"SA", "LA", "LD"}}
currentCombination = {}
detectedCombo = "none"
comboDamage = 50.0

loadFistTime = 0.4
lightDamage = 5.0
strongDamage = 10.0

timeForComboDecrease = 5.0
comboDecreaseTimeStamp = 0.0

function weaponStart()
    defaultDepth = getBoundingBoxDepth(entity, componentManager)
    setBoundingBox(entity, componentManager, false)
    table.insert(fists, entity)
end

function weaponUpdate(dt)

    if time - comboDecreaseTimeStamp >= timeForComboDecrease then
        if #currentCombination > 0 then 
            table.remove( currentCombination, 1 )
        end
    end

    local fist = fists[1] == entity and leftFist or rightFist

    if detectedCombo == "none" then
        setPositionRelativeToPlayer(fist)
    end

    handleAttack(dt, fist)
    handleCombos(dt)
end

function setPositionRelativeToPlayer(fist)
    setTransformRelToRotatingParent(entity, componentManager, player.position.x, player.position.y, player.position.z, 
    fist.positionRel.x, fist.positionRel.y, fist.positionRel.z,
    player.rotation.x, player.rotation.y, player.rotation.z)
end

function handleAttack(dt, fist)
    if rightFist.isAttacking == false and leftFist.isAttacking == false and detectedCombo ~= "none" then
        return
    end

    if fist.pressed == true and time - fist.pressedTimeStamp >= loadFistTime then
        fist.damage = strongDamage
        setColor(entity, componentManager, 1.0, 0.0, 0.0, 1.0)
    end

    if fist.pressed == false and fist.isAttacking == false and isMousePressed(fist) == true then
        fist.pressed = true
        fist.pressedTimeStamp = time
    elseif fist.pressed == true and fist.isAttacking == false and isMousePressed(fist) == false then
        fist.pressed = false
        fist.isAttacking = true
        attackCo = coroutine.create(attackCoroutine)
        fist.coroutine = attackCo
        coroutine.resume( fist.coroutine, dt, fist )
    elseif fist.isAttacking == true then
        coroutine.resume( fist.coroutine, dt, fist )
    end
    
end

function attackCoroutine(dt, fist)

    setBoundingBox(entity, componentManager, true)
    fist.attackTimeStamp = time

    while fist.positionRel.z < 6.0 do
        fist.positionRel.z = fist.positionRel.z + (40.0 * dt)
        dt = coroutine.yield()
    end
    fist.positionRel.z = 6.0

    while time - fist.attackTimeStamp < 0.1 do
        dt = coroutine.yield()
    end

    setBoundingBox(entity, componentManager, false)

    while fist.positionRel.z > 3.0 do
        fist.positionRel.z = fist.positionRel.z - (40.0 * dt)
        dt = coroutine.yield()
    end
    fist.positionRel.z = 3.0
    
    fist.isAttacking = false

    if fist.damage == strongDamage then
        setColor(entity, componentManager, 1.0, 1.0, 1.0, 1.0)
        fist.damage = lightDamage
    end
end

function checkForComboAndFinalizeAttack(fist)
    if detectedCombo ~= "none" then
        fist.isAttacking = false

        if fist.damage == strongDamage then
            setColor(entity, componentManager, 1.0, 1.0, 1.0, 1.0)
            fist.damage = lightDamage
        end

        return true
    end

    return false
end

function lerp(p0, p1, t)
    return (1 - t) * p0 + t * p1
end

function isMousePressed( fist )
    if fist == leftFist then
        return leftMousePressed
    elseif fist == rightFist then
        return rightMousePressed
    end
end

function handleCombos(dt)
    if detectedCombo ~= "none" then
        if leftFist.isAttacking == false and rightFist.isAttacking == false and entity == fists[1] then
            coroutine.resume(comboCo, dt)
        end

        return
    end

    if #currentCombination == 3 then
        local combo = checkCombos()
        if combo ~= "none" then
            detectedCombo = combo

            for i, move in ipairs(currentCombination) do
                currentCombination[i] = nil
            end

            comboCo = coroutine.create(whirlwindCoroutine)
            if leftFist.isAttacking == false and rightFist.isAttacking == false then
                coroutine.resume(comboCo, dt)
            end
        end
    end
end

function whirlwindCoroutine(dt)
    setBoundingBox(fists[1], componentManager, true)
    setBoundingBox(fists[2], componentManager, true)
    leftFist.attackTimeStamp = time
    rightFist.attackTimeStamp = time
    leftFist.damage = comboDamage / 2.0
    rightFist.damage = comboDamage / 2.0

    while leftFist.positionRel.z < 4.5 and rightFist.positionRel.z < 4.5 do
        if leftFist.positionRel.z < 4.5 then
            leftFist.positionRel.z = leftFist.positionRel.z + (80.0 * dt)
            setTransformRelToRotatingParent(fists[1], componentManager, player.position.x, player.position.y, player.position.z, 
            leftFist.positionRel.x, leftFist.positionRel.y, leftFist.positionRel.z,
            player.rotation.x, player.rotation.y, player.rotation.z)
        end
        if rightFist.positionRel.z < 4.5 then
            rightFist.positionRel.z = rightFist.positionRel.z + (80.0 * dt)
            setTransformRelToRotatingParent(fists[2], componentManager, player.position.x, player.position.y, player.position.z, 
            rightFist.positionRel.x, rightFist.positionRel.y, rightFist.positionRel.z,
            player.rotation.x, player.rotation.y, player.rotation.z)
        end
        dt = coroutine.yield()
    end
    leftFist.positionRel.z = 4.5
    rightFist.positionRel.z = 4.5

    currentDepth = defaultDepth

    setBoundingBoxDepth(fists[1], componentManager, defaultDepth, defaultDepth + 0.33)
    setBoundingBoxDepth(fists[2], componentManager, defaultDepth, defaultDepth + 0.33)

    local startingRotY = player.rotation.y
    local rotationY = startingRotY

    repeat
        rotationY = rotationY + (1440.0 * dt)
        setTransformRelToRotatingParent(fists[1], componentManager, player.position.x, player.position.y, player.position.z, 
        leftFist.positionRel.x, leftFist.positionRel.y, leftFist.positionRel.z,
        player.rotation.x, rotationY, player.rotation.z)
        setTransformRelToRotatingParent(fists[2], componentManager, player.position.x, player.position.y, player.position.z, 
        rightFist.positionRel.x, rightFist.positionRel.y, rightFist.positionRel.z,
        player.rotation.x, rotationY, player.rotation.z)
        dt = coroutine.yield()
    until rotationY - 360 > startingRotY
    setTransformRelToRotatingParent(fists[1], componentManager, player.position.x, player.position.y, player.position.z, 
        leftFist.positionRel.x, leftFist.positionRel.y, leftFist.positionRel.z,
        player.rotation.x, startingRotY, player.rotation.z)
    setTransformRelToRotatingParent(fists[2], componentManager, player.position.x, player.position.y, player.position.z, 
        rightFist.positionRel.x, rightFist.positionRel.y, rightFist.positionRel.z,
        player.rotation.x, startingRotY, player.rotation.z)

    while currentDepth > defaultDepth do
        currentDepth = currentDepth - (1.0 * dt)
        setBoundingBoxDepth(fists[1], componentManager, defaultDepth, currentDepth)
        setBoundingBoxDepth(fists[2], componentManager, defaultDepth, currentDepth)
        dt = coroutine.yield()
    end

    setBoundingBoxDepth(fists[1], componentManager, defaultDepth, defaultDepth)
    setBoundingBoxDepth(fists[2], componentManager, defaultDepth, defaultDepth)

    while leftFist.positionRel.z > 3.0 and rightFist.positionRel.z > 3.0 do
        leftFist.positionRel.z = leftFist.positionRel.z - (40.0 * dt)
        rightFist.positionRel.z = rightFist.positionRel.z - (40.0 * dt)
        setTransformRelToRotatingParent(fists[1], componentManager, player.position.x, player.position.y, player.position.z, 
            leftFist.positionRel.x, leftFist.positionRel.y, leftFist.positionRel.z,
            player.rotation.x, player.rotation.y, player.rotation.z)
        setTransformRelToRotatingParent(fists[2], componentManager, player.position.x, player.position.y, player.position.z, 
            rightFist.positionRel.x, rightFist.positionRel.y, rightFist.positionRel.z,
            player.rotation.x, player.rotation.y, player.rotation.z)
        dt = coroutine.yield()
    end
    leftFist.positionRel.z = 3.0
    rightFist.positionRel.z = 3.0

    setBoundingBox(fists[1], componentManager, false)
    setBoundingBox(fists[2], componentManager, false)
    leftFist.damage = lightDamage
    rightFist.damage = lightDamage

    detectedCombo = "none"
end

--combos = { ["whirlwind"] = {"SA", "LA", "LD"}}
function checkCombos()
    for i, combo in pairs(combos) do
        if checkCombo(combo) == true then
            return i
        end
    end

    return "none"
end

function checkCombo(combo)
    for j, action in ipairs(currentCombination) do
        local power = string.sub( combo[j], 1, 1 )
        local type = string.sub( combo[j], 2, 2 )
        local actionPower = string.sub( action, 1, 1 )
        local actionType = string.sub( action, 2, 2 )

        if power ~= actionPower or type ~= 'D' and type ~= 'A' and type ~= actionType or j > 1 and type == 'D' 
        and currentCombination[j] == currentCombination[j-1] then
            return false
        end
    end

    return true
end