hitInterval = 0.1
globalComboLeftHitStamp = 0.0
globalComboRightHitStamp = 0.0

enemies = {}

function enemyStart()
    local enemy = {health = 100.0, leftHitTimeStamp = 0.0, rightHitTimeStamp = 0.0, gotWhirlwind = false}
    enemies[entity] = enemy
end

function enemyUpdate(dt)
    if detectedCombo == "none" then
        enemies[entity].gotWhirlwind = false
    end
end

function enemyOnCollisionEnter(box)
    local enemy = enemies[entity]
    if getTag(box) == "playerWeaponLeft" and time - enemy.leftHitTimeStamp > hitInterval 
    and leftFist.attackTimeStamp ~= enemy.leftHitTimeStamp then
        enemy.leftHitTimeStamp = leftFist.attackTimeStamp

        if globalComboLeftHitStamp ~= leftFist.attackTimeStamp then
            globalComboLeftHitStamp = leftFist.attackTimeStamp
            table.insert( currentCombination, leftFist.damage == strongDamage and "SL" or "LL" )

            if #currentCombination == 4 then
                table.remove( currentCombination, 1 )
            end

            comboDecreaseTimeStamp = time
        end
        getHit(enemy, leftFist.damage)
    end

    if getTag(box) == "playerWeaponRight" and time - enemy.rightHitTimeStamp > hitInterval 
    and rightFist.attackTimeStamp ~= enemy.rightHitTimeStamp then
        enemy.rightHitTimeStamp = rightFist.attackTimeStamp

        if globalComboRightHitStamp ~= rightFist.attackTimeStamp then
            globalComboRightHitStamp = rightFist.attackTimeStamp
            table.insert( currentCombination, rightFist.damage == strongDamage and "SR" or "LR" )

            if #currentCombination == 4 then
                table.remove( currentCombination, 1 )
            end

            comboDecreaseTimeStamp = time
        end
        getHit(enemy, rightFist.damage)
    end
end

function getHit( enemy, dmg )
    enemy.health = enemy.health - dmg

    if enemy.health <= 0.0 then
    end

    if detectedCombo ~= "none" and enemy.gotWhirlwind == false then
        local x, y, z = getTransform(entity, componentManager)
        applyForce(entity, componentManager, (player.position.x - x), (player.position.y - y), 
        (player.position.z - z), -400.0)
        enemy.gotWhirlwind = true
    end
end