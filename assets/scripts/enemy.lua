hitInterval = 0.1
globalComboLeftHitStamp = 0.0
globalComboRightHitStamp = 0.0

minimalDistance = 2.0
maximalDistance = 8.0
unconsciousnessTime = 2.0

enemies = {}
lamps = {}

enemySpeed = 1.0

function enemyStart()
    local enemy = {health = 100.0, position = {x = 0.0, y = 0.0, z = 0.0}, velocity = {x = 0.0, y = 0.0, z = 0.0}, 
    leftHitTimeStamp = 0.0, rotation = {x = 0.0, y = 0.0, z = 0.0},
    rightHitTimeStamp = 0.0, unconscious = false, colliderEntity = -1, attackCo = {}, attackCoFunc = {}}
    enemy.position.x, enemy.position.y, enemy.position.z = getTransform(entity, componentManager)
    enemy.velocity.x, enemy.velocity.y, enemy.velocity.z = getVelocity(entity, componentManager)
    enemies[entity] = enemy
end

function enemyUpdate(dt)
    enemy = enemies[entity]
    enemy.position.x, enemy.position.y, enemy.position.z = getTransform(entity, componentManager)
    enemy.velocity.x, enemy.velocity.y, enemy.velocity.z = getVelocity(entity, componentManager)
    enemy.rotation.x, enemy.rotation.y, enemy.rotation.z = getRotation(entity, componentManager)

    if isUnconscious() == true then
        return
    end

    enemySetRotation()
    handleBehaviour(dt)
end

function isUnconscious()
    if enemy.unconscious == false then
        return false
    elseif time - enemy.leftHitTimeStamp > unconsciousnessTime and time - enemy.rightHitTimeStamp > unconsciousnessTime then
        enemy.unconscious = false
        return false
    else 
        return true
    end
end

function distance(e1, e2)
    return math.sqrt( (e1.position.x - e2.position.x) * (e1.position.x - e2.position.x) 
    + (e1.position.y - e2.position.y) * (e1.position.y - e2.position.y)
    + (e1.position.z - e2.position.z) * (e1.position.z - e2.position.z) )
end

function enemySetRotation()
    local deltaX = player.position.x - enemy.position.x
    local deltaZ = player.position.z - enemy.position.z

    local angle = math.atan(deltaZ, deltaX) * 180.0 / 3.14;

    setRotation(entity, componentManager, 0.0, -angle + 90.0, 0.0)
end

function handleBehaviour(dt)
    local xDir = player.position.x - enemy.position.x
    local yDir = 0.0
    local zDir = player.position.z - enemy.position.z

    local chasePlayer = false
    local attackPlayer = false
    local distance = math.sqrt( (xDir * xDir) + (zDir * zDir) )
    chasePlayer = distance < maximalDistance and true or false

    if lamps[enemy.colliderEntity].attacking == true then
        continueAttack(dt)
        return
    end

    if chasePlayer == false then --idle behaviour
        xDir = 0.0
        zDir = 0.0
    elseif distance > minimalDistance then --chase behaviour
        xDir, yDir, zDir = normalize(xDir, yDir, zDir)
        if isAnyAnimationPlaying(entity, componentManager) == false then
            enemyPlayAnim("Walking")
        end
    else --attack behaviour
        xDir = 0.0
        zDir = 0.0

        if lamps[enemy.colliderEntity].attacking == false and 
        time - lamps[enemy.colliderEntity].attackTimeStamp > lamps[enemy.colliderEntity].hitInterval then
            enemyPlayAnim("Attack")
            performAttack(dt)
            return
        elseif time > 5.0 then
            continueAttack(dt)
            return
        end
    end
    
    moveObject(xDir * enemySpeed, yDir, zDir * enemySpeed)
end

function normalize(x, y, z)
    local length = math.sqrt( (x * x) + (y * y) + (z * z) )

    if length == 0.0 then
        return 0.0, 0.0, 0.0
    end

    return x / length, y / length, z / length
end

function enemyOnCollisionEnter(box)
    local enemy = enemies[entity]
    local tag = getTag(box)

    if tag == "playerWeaponLeft" and time - enemy.leftHitTimeStamp > hitInterval 
    and leftFist.attackTimeStamp ~= enemy.leftHitTimeStamp then
        enemy.leftHitTimeStamp = leftFist.attackTimeStamp

        --print("LEFT")

        if globalComboLeftHitStamp ~= leftFist.attackTimeStamp then
            globalComboLeftHitStamp = leftFist.attackTimeStamp

            table.insert( currentCombination, leftFist.isStrong == true and "SL" or "LL" )

            if #currentCombination == 4 then
                table.remove( currentCombination, 1 )
            end

            comboDecreaseTimeStamp = time
        end
        getHit(enemy, leftFist.damage)
    end

    if tag == "playerWeaponRight" and time - enemy.rightHitTimeStamp > hitInterval 
    and rightFist.attackTimeStamp ~= enemy.rightHitTimeStamp then
        enemy.rightHitTimeStamp = rightFist.attackTimeStamp

        --print("RIGHT")

        if globalComboRightHitStamp ~= rightFist.attackTimeStamp then
            globalComboRightHitStamp = rightFist.attackTimeStamp
            table.insert( currentCombination, rightFist.isStrong == true and "SR" or "LR" )

            if #currentCombination == 4 then
                table.remove( currentCombination, 1 )
            end

            comboDecreaseTimeStamp = time
        end
        getHit(enemy, rightFist.damage)
    end
end

function getHit( enemy, dmg )
    
    if detectedCombo == "none" or detectedCombo == "detected" then
        --print("NO COMBO HIT", dmg, enemy.unconscious, detectedCombo)
        enemy.health = enemy.health - dmg
    elseif detectedCombo == "whirlwind" then
        applyForce(entity, componentManager, (player.position.x - enemy.position.x), 0.0, 
        (player.position.z - enemy.position.z), -600.0)

        if enemy.unconscious == false then
            enemy.unconscious = true
            enemy.health = enemy.health - dmg
            --print("COMBOHIT", dmg)
        end
    else
        if enemy.unconscious == false then
            enemy.unconscious = true
            enemy.health = enemy.health - dmg
            --print("COMBOHIT", dmg)
        end
    end

    if detectedCombo ~= "dash" then
        shake()
    end

    if enemy.health <= 0.0 then
        Die()
        return
    end
end

function Die()
    lamps[enemies[entity].colliderEntity] = nil
    destroyEntity(gameplayManager, enemies[entity].colliderEntity)
    enemies[entity] = nil
    destroyEntity(gameplayManager, entity)
end

function enemyPlayAnim(name)
    playAnimation(entity, componentManager, name)
end