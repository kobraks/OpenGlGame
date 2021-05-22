--Any global variables will be shown in entity editor layer.

speed = 10

--Colled once per u
--Parameter enttiy 
function OnUpdate(e)
end

--Colled every const update 
--Parameter entitiy, time step
function OnConstUpdate(e, ts)
end

--Called once per entity creation
--Parameter entity
function OnCreate(e)
    Log.Debug("Create");
end

--Called once per destroying entity
--Parameter entity
function OnDestroy(e)
end