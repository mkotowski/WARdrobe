print("Movement")

function movementStart()
    
end

function movementUpdate(dt)

end

function moveObject(moveX, moveY, moveZ)
    posX, posY, posZ = getTransform(entity, componentManager)

    posX = posX + moveX
    posY = posY + moveY
    posZ = posZ + moveZ

    setTransform(entity, componentManager, posX, posY, posZ)
end