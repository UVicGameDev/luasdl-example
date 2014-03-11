-- pixel position of the player
gPlayerPosition = { x = 0, y = 0 }
-- position directions: "up", "left", "down", "right"
gPlayerDirection = "down"
-- possible states: "idle", "walking"
gPlayerState = "idle" 

-- set of keys currently held down
gKeysHeld = { }

function OnKeyPressed (k)
    local oldKeyState = gKeysHeld[k]
    gKeysHeld[k] = true

    -- only change direction if this is not a repeated key press
    if oldKeyState ~= gKeysHeld[k] then
        if k == "Up" then
            gPlayerDirection = "up"
            gPlayerState = "walking"
        elseif k == "Down" then
            gPlayerDirection = "down"
            gPlayerState = "walking"
        elseif k == "Left" then
            gPlayerDirection = "left"
            gPlayerState = "walking"
        elseif k == "Right" then
            gPlayerDirection = "right"
            gPlayerState = "walking"
        end
    end
end

function OnKeyReleased (k)
    gKeysHeld[k] = nil

    -- if a direction key was released
    if k == "Up" and gPlayerDirection == "up" or 
       k == "Down" and gPlayerDirection == "down" or
       k == "Left" and gPlayerDirection == "left" or
       k == "Right" and gPlayerDirection == "right" then
       -- if another key is still being pressed, go that way instead
       if gKeysHeld["Up"] then
           gPlayerDirection = "up"
           gPlayerState = "walking"
       elseif gKeysHeld["Down"] then
           gPlayerDirection = "down"
           gPlayerState = "walking"
       elseif gKeysHeld["Left"] then
           gPlayerDirection = "left"
           gPlayerState = "walking"
       elseif gKeysHeld["Right"] then
           gPlayerDirection = "right"
           gPlayerState = "walking"
       else
           gPlayerState = "idle"
       end
   end

end

function Update (dt)
    local playerVelocity = { x = 0, y = 0 }
    
    -- you only have velocity if you're in the walking state
    if gPlayerState == "walking" then
        if gPlayerDirection == "up" then
            playerVelocity.y = -60
        elseif gPlayerDirection == "down" then
            playerVelocity.y = 60
        elseif gPlayerDirection == "left" then
            playerVelocity.x = -60
        elseif gPlayerDirection == "right" then
            playerVelocity.x = 60
        end
    end

    gPlayerPosition.x = gPlayerPosition.x + dt / 1000 * playerVelocity.x
    gPlayerPosition.y = gPlayerPosition.y + dt / 1000 * playerVelocity.y
end
