Scene = 
{
    Title = "Untilted";
    
    Entities = 
    {
        {
            Id = 1;

            TagComponent = {
                Tag = "Cow"
            };

            TransformComponent = 
            {
                --Optionaly Position = { x = 0; y = 0; z= 0 };
                Position = { 0, 0, 0 };
                Scale = { 0, 0, 0};
                Rotation = { 0, 0, 0 };
            };

            ModelComponent = 
            {
                Drawable = true; --optional in case of true
                Path = "Cow.obj"
            };

            LuaScriptComponent = 
            {
                Path = "script.lua";
                Properties = { --Not working yet
                    Speed = 0.5;
                };
            };
        };
        {
            TagComponent = {
                Tag = "Camera"
            };

            TransformComponent = 
            {
                Position = { 0, 0, 0 };
                Scale = { 0, 0, 0};
                Rotation = { 0, 0, 1 };
            };

            CameraComponent = 
            {
                    
                Type = CameraType.Perspective;

                NearClip  = 0.001;
                FarClip = 1000;
                FOV = 45; --in case of Perspective camera type
                Size = 10; --Ignored if not Orthographic camera type


                Primary = true;
                FixedAspectRatio = false;

                --Ignored in case of FixedAspectRatio = false
                AspectRatio = 800 / 600;
            };
        };
        {
            TagComponent = {
                Tag = "DirectionalLight";
            };

            TransformComponent = {
                --Ignored in case of directional light
                Position = {0, 0, 0};
                --Direction ignored if not Directional light
                Rotation = { 0, -1, 0};
            };

            LightComponent = 
            {
                Active = true; --Redundand in case of false

                Type = LightType.Directional; --Possible values LightType.Directional, LightType.Spot, LightType.Point

                DiffuseColor = { 255, 255, 255};
                AmbientColor = { 255, 255, 255};
                SpecularColor = { 255, 255, 255};
        
                --Ifnored if not spot light or point light
                Constant = 0;
                Linear = 0;
                Quadratic = 0;
        
                --Ignored if not Spot light
                CutOff = 0;
                OuterCutOff = 0;

                --Optional spot light texture Cookie
                LightCookie = "flashlight.png";
            };
        };
        {
            TagComponent = {
                "PointLight0";
            };

            TransformComponent = {
                Position = { 5, 5, 5};
                Rotation = { 0, 0, 0};
            };

            LightComponent = 
            {
                Active = true;
                
                Type = LightType.Point;

                DiffuseColor = { 255, 0, 0};
                AmbientColor = { 255, 0, 0};
                SpecularColor = { 255, 0, 0};

                Constant = 0;
                Linear = 0;
                Quadratic = 0;
            };
        }
    };
};