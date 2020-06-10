attacked = false
hitInterval = 0.5
hitTimeStamp = 0.0

function enemyStart()
end

function enemyUpdate(dt)
    if attacked == true and time - hitTimeStamp > hitInterval then
        attacked = false
    end
end

function enemyOnCollisionEnter(box)
    if getTag(box) == "playerWeapon" and attacked == false then
        local x, y, z = getTransform(entity, componentManager)
        applyForce(entity, componentManager, (player.position.x - x), (player.position.y - y), 
        (player.position.z - z), -500.0)
        attacked = true
        hitTimeStamp = time
    end
end